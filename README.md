# Sisop-2-2025-IT11

# Member

    1. Muhammad Ardiansyah Tri Wibowo - 5027241091
    2. Erlinda Annisa Zahra Kusuma - 5027241108
    3. Fika Arka Nuriyah - 5027241071

# Reporting

### Soal 1
Penjelasan Soal No. 1

![image](https://github.com/user-attachments/assets/3b3601f4-009d-4bda-b3d4-740ff979e9ff)

Soal ini memberikan chalenge untuk mencari password dari sebuah data berupa Clues.zip untuk masuk kedalam website, dari penjelasan soal kita perlu membuat sebuah file action.c dimana isi dari file tersebut berupa program untuk

A.Mendownload Clues.zip dan unzip
B.Filtering the files
C.Combine teh file content
D.Decode the file
E.Password check

Penjelasan code action.c :

1. Library yang dipakai 
```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/wait.h>
```
<sys/stat.h>  fungsi mkdir()
<unistd.h>     execvp(), fork(), access file
<dirent.h>      baca folder
<ctype.h>       cek huruf/angka
<sys/wait.h>  buat tunggu child process (wait)

2.Menjalankan Command terminal
```  
void run(char *args[]) {
    if (fork() == 0) {
        execvp(args[0], args);
        exit(1);
    } else {
        wait(NULL);
    }
}
```

3.Download file zip dan unzip
``` 
void downloadDanUnzip() {
    struct stat st;

    // Cek apakah folder Clues sudah ada
    if (stat("Clues", &st) == 0 && S_ISDIR(st.st_mode)) {
        printf("Folder Clues sudah ada, skip download dan unzip.\n");
        return;
    }

    char *dl[] = {
        "wget", "-O", "Clues.zip",
        "https://drive.usercontent.google.com/u/0/uc?id=1xFn1OBJUuSdnApDseEczKhtNzyGekauK&export=download",
        NULL
    };
    char *uz[] = {"unzip", "Clues.zip", NULL};
    char *rmz[] = {"rm", "Clues.zip", NULL};

    run(dl);
    run(uz);
    run(rmz);
}
```
Untuk mendownload file kita memakai command wget -o seperti di terminal, ini bisa dilakukan di c dengan memakai bantuan execvp(), begitu juga untuk unzip dan rm file 

4.Mengecek apakah nama file valid untuk di filter 
```
int validNama(const char *nama) {
    return strlen(nama) == 5 && isalnum(nama[0]) && strcmp(nama+1, ".txt") == 0;
}
```
```
void filterFile() {
    mkdir("Filtered", 0755);

    const char *clue[] = {"Clues/ClueA", "Clues/ClueB", "Clues/ClueC", "Clues/ClueD"};
    for (int i = 0; i < 4; i++) {
        DIR *dir = opendir(clue[i]);
        struct dirent *ent;

        while ((ent = readdir(dir)) != NULL) {
            char asal[512], tujuan[512];
            snprintf(asal, sizeof(asal), "%s/%s", clue[i], ent->d_name);

            if (validNama(ent->d_name)) {
                snprintf(tujuan, sizeof(tujuan), "Filtered/%s", ent->d_name);
                rename(asal, tujuan);
            } else {
                remove(asal);
            }
        }

        closedir(dir);
    }
}

```
Dari soal yang diberikan kita diminta untuk membuat folder baru bernama filtered yang berisi file yang valid. Hal ini memerlukan pengecekan untuk penamaan file yang berada di dalam folder melalui validNama

5.Urutin string alfabet secara ascending agar urut filenya
```
int banding(const void *a, const void *b) {
    return strcmp(*(char **)a, *(char **)b);
}

```

Supaya file menjadi urut dan mudah untuk menggabungkan di next code
6.Menggabungkan file
```
DIR *dir = opendir("Filtered");
    char *angka[100], *huruf[100];
    int ca = 0, ch = 0;

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        if (strlen(ent->d_name) == 5) {
            if (isdigit(ent->d_name[0])) angka[ca++] = strdup(ent->d_name);
            else if (isalpha(ent->d_name[0])) huruf[ch++] = strdup(ent->d_name);
        }
    }
    closedir(dir);

    qsort(angka, ca, sizeof(char*), banding);
    qsort(huruf, ch, sizeof(char*), banding);

    FILE *out = fopen("Combined.txt", "w");

    int i = 0;
    while (i < ca || i < ch) {
        if (i < ca) {
            char path[512];
            snprintf(path, sizeof(path), "Filtered/%s", angka[i]);
            FILE *f = fopen(path, "r");
            char c = fgetc(f);
            if (c != EOF) fputc(c, out);
            fclose(f);
            remove(path);
        }
        if (i < ch) {
            char path[512];
            snprintf(path, sizeof(path), "Filtered/%s", huruf[i]);
            FILE *f = fopen(path, "r");
            char c = fgetc(f);
            if (c != EOF) fputc(c, out);
            fclose(f);
            remove(path);
        }
        i++;
    }

    fclose(out);
}

```


7.Fungsi ROT13
```
 if (c >= 'a' && c <= 'z') return ((c - 'a' + 13) % 26) + 'a';
    if (c >= 'A' && c <= 'Z') return ((c - 'A' + 13) % 26) + 'A';
    return c;
}
```

8.Decode File
```
void decodeFile() {
    FILE *in = fopen("Combined.txt", "r");
    FILE *out = fopen("Decoded.txt", "w");

    char c;
    while ((c = fgetc(in)) != EOF) {
        fputc(rot13(c), out);
    }

    fclose(in);
    fclose(out);
}
```

9.Int main
ini untuk menjalankan command mulai dari download unzip, filter, combine dan decode 
Dan berisi error handling seperti yang disarankan di soal


### Soal 2
Membuat suatu fitur yang dapat membantu Kenade. Berikut beberapa fitur yang harus didapatkan di starterkit.c

```
A. Mendownload dan unzip dan melakukan penghapusan file asli zip
B. Mendecrypt file Base64
C. Memindahkan file
D. Mematikan file starterkit dengan aman
E. Menambahkan Error handling
F. Mencatat Aktivitas starterkit
```

### A. Mendownload dan unzip dan melakukan penghapusan file asli zip
Kita melakukan pendownloadan terlebih dahulu sesuai yang diminta.

    char *url = "https://drive.usercontent.google.com/u/0/uc?id=1_5GxIGfQr3mNKuavJbte_AoRkEQLXSKS&export=download";
    char *args[] = {"/usr/bin/wget", "-O", "starterkit.zip", url, NULL};

    ....

    execve("/usr/bin/wget", args, NULL);

    ....

Selanjutnya kita melakukan unzip

    char *unzip_args[] = {"/usr/bin/unzip", "starterkit.zip", "-d", "starter_kit", NULL}; //set argumentnya, kalau di shell harusnya (unzip starterkit.zip -d starter_kit)
    ....

    execve("/usr/bin/unzip", unzip_args, NULL);
    ....

Habis itu kita menghapus zipnya
        
    if (remove("starterkit.zip") != 0) {
        perror("Failed to delete starterkit.zip");
        exit(EXIT_FAILURE);
    }

### B. Mendecrypt file Base64
Disini kita membuat decrypt dengan daemon

```
 pid_t pid, sid; // Variabel untuk menyimpan PID

    pid = fork(); // Menyimpan PID dari Child Process

    /* Keluar saat fork gagal
     * (nilai variabel pid < 0) */
    if (pid < 0) {
        perror("Gagal membuat proses fork");
        exit(EXIT_FAILURE);
    }

    /* Keluar saat fork berhasil
     * (nilai variabel pid adalah PID dari child process) */
    if (pid > 0) {
        printf("Proses daemon untuk enkripsi telah dimulai.\n");
        exit(EXIT_SUCCESS);
    }

    umask(0);

    sid = setsid();
    if (sid < 0) {
        perror("Gagal membuat session baru");
        exit(EXIT_FAILURE);
    }

    ....

    while (1) {
        pid_t exec_pid = fork();
        if (exec_pid == 0) {
            char *args[] = {"/bin/sh", "-c",
                "find quarantine -type f -printf '%P\\n' | "
                "xargs -I {} bash -c 'echo \"{}\" | base64 -d 2>/dev/null | iconv -f ASCII -t UTF-8 2>/dev/null' | "
                "awk '{ remaining = $0; while (match(remaining, /([^.]+\\.(vbs|js|dll|bin|exe|py|jar|sys|bat))/)) { print \"quarantine/\" substr(remaining, RSTART, RLENGTH); remaining = substr(remaining, RSTART + RLENGTH); } if (remaining != \"\") { print \"quarantine/\" remaining; } }' | "
                "xargs -n 1 touch && "
                "find quarantine -type f -printf '%P\\n' | "
                "awk '{if ($0 ~ /^[A-Za-z0-9+\\/]+=*$/) { print \"quarantine/\" $0 }}' | "
                "xargs -d '\n' rm -v",
                NULL};
            execve("/bin/sh", args, NULL);
        .....
        sleep(30);
    }
```
### C. Memindahkan file
Kita melakukan perpindahan file dari quarantine ke starter_kit baik sebaliknya 
```

execl("/bin/sh", "sh", "-c", "find starter_kit -type f -exec sh -c 'mv \"$1\" quarantine/", NULL);

....

execl("/bin/sh", "sh", "-c", "find quarantine -type f -exec sh -c 'mv \"$1\" starter_kit/", NULL);

```

### D. Mematikan file starterkit dengan aman
Kita melakukan shutdown proses dengan kill PIDnya
```
execl("/bin/sh", "sh", "-c", "ps aux | grep './starterkit --decrypt' | grep -v grep | awk '{print $2}' | while read pid; do kill -9 $pid; done", NULL);
```

### E. Menambahkan Error handling

```
} else {
        printf("Penggunaan: ./starterkit --<flag>\n");
        printf("Pilihan flag:\n");
        printf("  --decrypt    : Mendekripsi file di folder quarantine\n");
        printf("  --return     : Memindahkan file dari folder quarantine ke starter_kit\n");
        printf("  --quarantine : Memindahkan file dari folder starter_kit ke quarantine\n");
        printf("  --eradicate  : Menghapus folder quarantine\n");
        printf("  --shutdown   : Mematikan proses dekripsi\n");
        printf(" ");
        printf("Jika tidak ada flag yang diberikan, maka akan melakukan download starterkit.zip\n");
        stepa();
}
```
### F. Mencatat Aktivitas starterkit
```
....
"echo \"[$(date '+%%d-%%m-%%Y')][$(date '+%%H:%%M:%%S')] - Successfully started decryption process with PID %d.\" >> activity.log",
...
...
execl("/bin/sh", "sh", "-c", "find starter_kit -type f -exec sh -c 'mv \"$1\" quarantine/ && echo \"[$(date '+%d-%m-%Y')][$(date '+%H:%M:%S')] - $(basename \"$1\") - Successfully moved to quarantine directory.\" >> activity.log' _ {} \\;", NULL);
...
...
execl("/bin/sh", "sh", "-c", "find quarantine -type f -exec sh -c 'mv \"$1\" starter_kit/ && echo \"[$(date '+%d-%m-%Y')][$(date '+%H:%M:%S')] - $(basename \"$1\") - Successfully moved to starter_kit directory.\" >> activity.log' _ {} \\;", NULL);
...
...
execl("/bin/sh", "sh", "-c", "find quarantine -type f -exec sh -c 'echo \"[$(date '+%d-%m-%Y')][$(date '+%H:%M:%S')] - $(basename \"$1\") - Successfully deleted.\" >> activity.log' _ {} \\;", NULL);
...
...
execl("/bin/sh", "sh", "-c", "ps aux | grep './starterkit --decrypt' | grep -v grep | awk '{print $2}' | while read pid; do echo \"[$(date '+%d-%m-%Y')][$(date '+%H:%M:%S')] - Successfully shut off decryption process with PID $pid.\" >> activity.log; kill -9 $pid; done", NULL);
...
```

### G. Hapus file quarantine

```
execl("/bin/sh", "sh", "-c", "rm -rf quarantine/*", NULL);

```

### Revisi No 2
```
        system("find quarantine -type f -printf '%P\\n' | "
               "xargs -I {} bash -c 'echo \"{}\" | base64 -d 2>/dev/null | iconv -f ASCII -t UTF-8 2>/dev/null' | "
               "awk '{ remaining = $0; while (match(remaining, /([^.]+\\.(vbs|js|dll|bin|exe|py|jar|sys|bat))/)) { print \"quarantine/\" substr(remaining, RSTART, RLENGTH); remaining = substr(remaining, RSTART + RLENGTH); } if (remaining != \"\") { print \"quarantine/\" remaining; } }' | "
               "xargs -n 1 touch && "
               "find quarantine -type f -printf '%P\\n' | "
               "awk '{if ($0 ~ /^[A-Za-z0-9+\\/]+=*$/) { print \"quarantine/\" $0 }}' | "
               "xargs -d '\n' rm -v");
        sleep(1);

    // change to 

    char *args[] = {"/bin/sh", "-c",
                "find quarantine -type f -printf '%P\\n' | "
                "xargs -I {} bash -c 'echo \"{}\" | base64 -d 2>/dev/null | iconv -f ASCII -t UTF-8 2>/dev/null' | "
                "awk '{ remaining = $0; while (match(remaining, /([^.]+\\.(vbs|js|dll|bin|exe|py|jar|sys|bat))/)) { print \"quarantine/\" substr(remaining, RSTART, RLENGTH); remaining = substr(remaining, RSTART + RLENGTH); } if (remaining != \"\") { print \"quarantine/\" remaining; } }' | "
                "xargs -n 1 touch && "
                "find quarantine -type f -printf '%P\\n' | "
                "awk '{if ($0 ~ /^[A-Za-z0-9+\\/]+=*$/) { print \"quarantine/\" $0 }}' | "
                "xargs -d '\n' rm -v",
                NULL};
    execve("/bin/sh", args, NULL);
```

### Soal 3
### Soal 4
Membuat Sebuah Fitur yang bisa memantau semua aktivitas di komputer. Berikut adalah fitur fitur yang dimiliki oleh debugmon

    A. Mengetahui semua aktivitas user (./debugmon list user)
    B. Memasang mata mata dalam mode daemon (./debugmon daemon user)
    C. Menghentikan pengawasan (./debugmon stop user)
    D. Menggagalkan semua proses user yang sedang berjalan (./debugmon fail user)
    E. Mengizinkan user untuk kembali menjalankan proses (./debugmon revert user)
    
Semua fitur yang telah diperintahkan akan dicatat ke dalam file log (/tmp/debugmon.log) lalu ada format waktu seperti 


    [dd:mm:yyyy]-[hh:mm:ss]_nama-process_STATUS(RUNNING/FAILED)

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




