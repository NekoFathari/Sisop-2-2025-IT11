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
![image](https://github.com/user-attachments/assets/52df75c4-f609-4d22-ae6c-386c089451bf)
<sys/stat.h>  fungsi mkdir()
<unistd.h>     execvp(), fork(), access file
<dirent.h>      baca folder
<ctype.h>       cek huruf/angka
<sys/wait.h>  buat tunggu child process (wait)

2.Menjalankan Command terminal
![image](https://github.com/user-attachments/assets/ce482e83-c3ed-434c-92b6-12898bb89f87)

3.Download file zip dan unzip
![image](https://github.com/user-attachments/assets/8fbf2237-d80a-40be-a6b5-71eb0412efe2)
Untuk mendownload file kita memakai command wget -o seperti di terminal, ini bisa dilakukan di c dengan memakai bantuan execvp(), begitu juga untuk unzip dan rm file 

4.Mengecek apakah nama file valid untuk di filter 
![image](https://github.com/user-attachments/assets/bfaa5021-a8ed-4518-b075-9476971b2ea8)
Dari soal yang diberikan kita diminta untuk membuat folder baru bernama filtered yang berisi file yang valid. Hal ini memerlukan pengecekan untuk penamaan file yang berada di dalam folder melalui validNama

5.Urutin string alfabet secara ascending agar urut filenya
![image](https://github.com/user-attachments/assets/7c45c04c-9ce0-41d5-9175-bd815b9c652b)
Supaya file menjadi urut dan mudah untuk menggabungkan di next code
6.Menggabungkan file
![image](https://github.com/user-attachments/assets/22da6447-cb8d-445a-bae0-36e9a27fa0a8)

7.Fungsi ROT13
![image](https://github.com/user-attachments/assets/15bfcff6-fc9b-4487-ac8f-2753d4df7fe3)

8.Decode File
![image](https://github.com/user-attachments/assets/6dc22cd1-5b5d-42ec-be9f-636ecc616897)

9.Int main 
![image](https://github.com/user-attachments/assets/fb8ddc5e-424f-493a-b57e-f3897fa624e1)

Ini untuk menjalankan command mulai dari download unzip, filter, combine dan decode 
Dan berisi error handling seperti yang disarankan di soal


### Soal 2
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




