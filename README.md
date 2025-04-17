# Sisop-2-2025-IT11

# Member

    1. Muhammad Ardiansyah Tri Wibowo - 5027241091
    2. Erlinda Annisa Zahra Kusuma - 5027241108
    3. Fika Arka Nuriyah - 5027241071

# Reporting

### Soal 1
### Soal 2
### Soal 3
### Soal 4
Membuat Sebuah Fitur yang bisa memantau semua aktivitas di komputer. Berikut adalah fitur fitur yang dimiliki oleh debugmon

    A. Mengetahui semua aktivitas user (./debugmon list user)
    B. Memasang mata mata dalam mode daemon (./debugmon daemon user)
    C. Menghentikan pengawasan (./debugmon stop user)
    D. Menggagalkan semua proses user yang sedang berjalan (./debugmon fail user)
    E. Mengizinkan user untuk kembali menjalankan proses (./debugmon revert user)
    
Semua fitur yang telah diperintahkan akan dicatat ke dalam file log (/tmp/debugmon.log) lalu ada format waktu seperti [dd:mm:yyyy]-[hh:mm:ss]_nama-process_STATUS(RUNNING/FAILED)

## A. Mengetahui semua aktivitas user (./debugmon list user)
Disini akan menampilkan daftar semua proses yang sedang berjalan pada user tersebut beserta PID, command, CPU usage dan memory usage 


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

# Output
![image](https://github.com/user-attachments/assets/585ad1b4-7f58-4c31-bba3-ce3683953d83)


## B. Memasang mata mata dalam mode daemon (./debugmon daemon user)
Disini debugmon akan terus memantau user secara otomatis


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

# Output
![image](https://github.com/user-attachments/assets/ea6f4b1d-7e6a-44c7-8904-593adf0ef3e3)


## C. Menghentikan pengawasan (./debugmon stop user)
Disini untuk menghentikan commandnya yang sedang berjalan di user tersebut.

     
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

# Output
![image](https://github.com/user-attachments/assets/fb2e7dcb-1f30-4a39-a889-efd2dd9de37c)


## D. Menggagalkan semua proses user yang sedang berjalan (./debugmon fail user)
Debugmon langsung menggagalkan semua proses yang sedang berjalan dan menulis status proses.


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



# Output 
![image](https://github.com/user-attachments/assets/329e711e-5f82-4147-aea4-08afd8681a13)

lalu user juga tidak dapat menjalankan proses lain dalam mode ini
![failed](https://github.com/user-attachments/assets/eb255c27-bcfc-4a18-949c-27ca906a3f37)

## E. Mengizinkan user untuk kembali menjalankan proses (./debugmon revert user)
dengan ini debugmon kembali ke mode normal dan bisa menjalankan proses lain seperti biasa


        void revert_user(const char *username) {
    log_message("debugmon", "RUNNING");
    printf("Monitor dari user %s telah revert. Proses dijalankan kembali.\n", username);
    }        

# Output
![image](https://github.com/user-attachments/assets/ed5700f4-d22f-4a3b-9910-0d8765cce3bb)




