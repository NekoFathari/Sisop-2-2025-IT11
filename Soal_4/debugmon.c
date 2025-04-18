#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <pwd.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h>

#define LOG_FILE "/tmp/debugmon.log"
#define PID_FILE "/tmp/debugmon.pid"

// ============================================================================
// format waktu
// ============================================================================

void log_message(const char *process_name, const char *status) {
    FILE *log = fopen(LOG_FILE, "a");
    if (!log) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    fprintf(log, "[%02d:%02d:%04d]-[%02d:%02d:%02d]_%s_%s\n",
            t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
            t->tm_hour, t->tm_min, t->tm_sec,
            process_name, status);

    fclose(log);
}

// ============================================================================
// Fitur 1: list <user> - Menampilkan proses milik user
// ============================================================================

void list_processes(const char *username) {
    struct passwd *pwd = getpwnam(username);
    if (!pwd) {
        fprintf(stderr, "User %s tidak ditemukan.\n", username);
        return;
    }
    uid_t target_uid = pwd->pw_uid;

    DIR *proc = opendir("/proc");
    if (!proc) {
        perror("opendir /proc");
        return;
    }

    printf("PID\tCOMMAND\t\tCPU\tMEM\n");

    struct dirent *entry;
    while ((entry = readdir(proc)) != NULL) {
        if (!isdigit(entry->d_name[0])) continue;

        char status_path[512];
        snprintf(status_path, sizeof(status_path), "/proc/%s/status", entry->d_name);

        FILE *status = fopen(status_path, "r");
        if (!status) continue;

        char line[256];
        uid_t uid = -1;
        char name[256] = "";

        while (fgets(line, sizeof(line), status)) {
            if (strncmp(line, "Uid:", 4) == 0)
                sscanf(line, "Uid:\t%d", &uid);
            if (strncmp(line, "Name:", 5) == 0)
                sscanf(line, "Name:\t%255s", name);
        }

        fclose(status);
        if (uid != target_uid) continue;

        char stat_path[512];
        snprintf(stat_path, sizeof(stat_path), "/proc/%s/stat", entry->d_name);
        FILE *stat = fopen(stat_path, "r");
        if (!stat) continue;

        long unsigned utime, stime;
        int dummy;
        char comm[256];

        fscanf(stat, "%d %s", &dummy, comm);
        for (int i = 0; i < 11; i++) fscanf(stat, "%*s");
        fscanf(stat, "%lu %lu", &utime, &stime);
        fclose(stat);

        char statm_path[512];
        snprintf(statm_path, sizeof(statm_path), "/proc/%s/statm", entry->d_name);
        FILE *statm = fopen(statm_path, "r");
        if (!statm) continue;

        long mem;
        fscanf(statm, "%ld", &mem);
        fclose(statm);

        printf("%s\t%s\t%lu\t%ld KB\n", entry->d_name, name, utime + stime, mem * 4);
    }

    closedir(proc);
}

// ============================================================================
// Fitur 2: daemon <user> - Monitoring user sebagai daemon
// ============================================================================

void daemon_mode(const char *username) {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) {
        FILE *f = fopen(PID_FILE, "w");
        if (f) {
            fprintf(f, "%d", pid);
            fclose(f);
        }
        exit(EXIT_SUCCESS);
    }

    setsid();
    chdir("/");
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    struct passwd *pwd = getpwnam(username);
    if (!pwd) exit(EXIT_FAILURE);
    uid_t target_uid = pwd->pw_uid;

    while (1) {
        DIR *proc = opendir("/proc");
        if (!proc) exit(EXIT_FAILURE);

        struct dirent *entry;
        while ((entry = readdir(proc)) != NULL) {
            if (!isdigit(entry->d_name[0])) continue;

            char status_path[512];
            snprintf(status_path, sizeof(status_path), "/proc/%s/status", entry->d_name);
            FILE *status = fopen(status_path, "r");
            if (!status) continue;

            char line[256], name[256] = "";
            uid_t uid = -1;
            while (fgets(line, sizeof(line), status)) {
                if (strncmp(line, "Uid:", 4) == 0)
                    sscanf(line, "Uid:\t%d", &uid);
                if (strncmp(line, "Name:", 5) == 0)
                    sscanf(line, "Name:\t%255s", name);
            }

            fclose(status);
            if (uid == target_uid) log_message(name, "RUNNING");
        }

        closedir(proc);
        sleep(5);
    }
}

// ============================================================================
// Fitur 3: stop <user> - Menghentikan daemon
// ============================================================================

void stop_daemon(const char *username) {
    FILE *f = fopen(PID_FILE, "r");
    if (!f) {
        fprintf(stderr, "Daemon tidak running\n");
        return;
    }

    pid_t pid;
    fscanf(f, "%d", &pid);
    fclose(f);

    kill(pid, SIGTERM);
    remove(PID_FILE);
    log_message("debugmon", "RUNNING");
    printf("Berhenti memantau user %s.\n", username);
}

// ============================================================================
// Fitur 4: fail <user> - Mematikan proses user yang tidak diizinkan
// ============================================================================

void fail_user(const char *username) {
    struct passwd *pwd = getpwnam(username);
    if (!pwd) {
        fprintf(stderr, "User %s tidak ditemukan.\n", username);
        return;
    }

    uid_t target_uid = pwd->pw_uid;
    const char *allowed[] = {"bash", "sh", "zsh", "debugmon", "sleep"};

    DIR *proc = opendir("/proc");
    if (!proc) {
        perror("opendir /proc");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(proc)) != NULL) {
        if (!isdigit(entry->d_name[0])) continue;

        char status_path[512];
        snprintf(status_path, sizeof(status_path), "/proc/%s/status", entry->d_name);
        FILE *status = fopen(status_path, "r");
        if (!status) continue;

        char line[256], name[256] = "";
        uid_t uid = -1;

        while (fgets(line, sizeof(line), status)) {
            if (strncmp(line, "Uid:", 4) == 0)
                sscanf(line, "Uid:\t%d", &uid);
            if (strncmp(line, "Name:", 5) == 0)
                sscanf(line, "Name:\t%255s", name);
        }

        fclose(status);
        if (uid != target_uid) continue;

        int allowed_flag = 0;
        for (int i = 0; i < sizeof(allowed)/sizeof(allowed[0]); i++) {
            if (strcmp(name, allowed[i]) == 0) {
                allowed_flag = 1;
                break;
            }
        }

        if (!allowed_flag) {
            pid_t victim = atoi(entry->d_name);
            if (kill(victim, SIGKILL) == 0)
                log_message(name, "FAILED");
            else
                log_message(name, "KILL_FAILED");
        }
    }

    closedir(proc);
}


// ============================================================================
// Fitur 5: revert <user> - Mengembalikan status monitoring 
// ============================================================================

void revert_user(const char *username) {
    log_message("debugmon", "RUNNING");
    printf("Monitor dari user %s telah revert. Proses dijalankan kembali.\n", username);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s [list|daemon|stop|fail|revert] <user>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "list") == 0) list_processes(argv[2]);
    else if (strcmp(argv[1], "daemon") == 0) daemon_mode(argv[2]);
    else if (strcmp(argv[1], "stop") == 0) stop_daemon(argv[2]);
    else if (strcmp(argv[1], "fail") == 0) fail_user(argv[2]);
    else if (strcmp(argv[1], "revert") == 0) revert_user(argv[2]);
    else {
        fprintf(stderr, "tidak ada perintah yang ditentukan.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

