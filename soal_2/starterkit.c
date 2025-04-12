#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <time.h>

void stepa(){
    printf("Downloading starterkit.zip...\n");
    // download file https://drive.usercontent.google.com/u/0/uc?id=1_5GxIGfQr3mNKuavJbte_AoRkEQLXSKS&export=download dengan wget
    char *url = "https://drive.usercontent.google.com/u/0/uc?id=1_5GxIGfQr3mNKuavJbte_AoRkEQLXSKS&export=download";
    char *args[] = {"/usr/bin/wget", "-O", "starterkit.zip", url, NULL}; //set argumentnya, kalau di shell harusnya (wget -O starterkit.zip https://drive.usercontent.google.com/u/0/uc?id=1_5GxIGfQr3mNKuavJbte_AoRkEQLXSKS&export=download)
    /*  
        args[0] = "/usr/bin/wget"; // path wget
        args[1] = "-O"; // option -O
        args[2] = "starterkit.zip"; // nama file yang di download
        args[3] = url; // url yang di download
        args[4] = NULL; // akhir dari argument 
    */   
    pid_t pid = fork();
    if (pid == 0) {
        execve("/usr/bin/wget", args, NULL);
        // set error handling kalau wget gagal
        perror("Failed to execute wget");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        wait(NULL); // Wait for the child process to finish
        printf("Download completed.\n");
        sleep(2);

        // mari kita unzip file starterkit.zip dan simpan ke folder starter_kit
        printf("Unzipping starterkit.zip...\n");

        char *unzip_args[] = {"/usr/bin/unzip", "starterkit.zip", "-d", "starter_kit", NULL}; //set argumentnya, kalau di shell harusnya (unzip starterkit.zip -d starter_kit)
        pid_t unzip_pid = fork();
        if (unzip_pid == 0) {
            execve("/usr/bin/unzip", unzip_args, NULL);
            // set error handling kalau unzip gagal
            perror("Failed to execute unzip");
            exit(EXIT_FAILURE);
        } else if (unzip_pid > 0) {
            wait(NULL); // Wait for the unzip process to finish
            printf("Unzip completed.\n");
            sleep(2);
            printf("Deleting starterkit.zip...\n");

            // mari kita hapus file starterkit.zip
            if (remove("starterkit.zip") != 0) {
                perror("Failed to delete starterkit.zip");
                exit(EXIT_FAILURE);
            }

            printf("starterkit.zip deleted.\n");
        }
    } else {
        perror("Failed to fork process");
        exit(EXIT_FAILURE);
    }   
}

void stepb() {

    //lakukan pengecekan apakah folder quarantine ada, jika tidak ada maka buat folder quarantine
    if (access("quarantine", F_OK) == -1) {
      if (mkdir("quarantine", 0700) != 0) {
        perror("Failed to create quarantine folder");
        exit(EXIT_FAILURE);

      }
    }

    // kita dapatkan nama file yang ada di quarantine
    pid_t log_pid = fork();
    if (log_pid == 0) {
        char log_command[256];
        snprintf(log_command, sizeof(log_command),
                 "echo \"[$(date '+%%d-%%m-%%Y')][$(date '+%%H:%%M:%%S')] - Successfully started decryption process with PID %d.\" >> activity.log",
                 getpid());
        char *args[] = {"/bin/sh", "-c", log_command, NULL};
        execve("/bin/sh", args, NULL);
        perror("Failed to log activity");
        exit(EXIT_FAILURE);
    } else if (log_pid > 0) {
        wait(NULL); // Wait for the logging process to finish
    } else {
        perror("Failed to fork process for logging");
        exit(EXIT_FAILURE);
    }
    // ubah nama file-file yang ada di quarantine yang di enkripsi dengan base64 
    // mari kita encrypt file-file yang ada di quarantine
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

    if ((chdir("/")) < 0) {
        perror("Gagal mengubah direktori kerja");
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    while (1) {
        system("find quarantine -type f -printf '%P\\n' | "
               "xargs -I {} bash -c 'echo \"{}\" | base64 -d 2>/dev/null | iconv -f ASCII -t UTF-8 2>/dev/null' | "
               "awk '{ remaining = $0; while (match(remaining, /([^.]+\\.(vbs|js|dll|bin|exe|py|jar|sys|bat))/)) { print \"quarantine/\" substr(remaining, RSTART, RLENGTH); remaining = substr(remaining, RSTART + RLENGTH); } if (remaining != \"\") { print \"quarantine/\" remaining; } }' | "
               "xargs -n 1 touch && "
               "find quarantine -type f -printf '%P\\n' | "
               "awk '{if ($0 ~ /^[A-Za-z0-9+\\/]+=*$/) { print \"quarantine/\" $0 }}' | "
               "xargs -d '\n' rm -v");

        sleep(30);
    }
  }

void stepc1(){
  // pindahkan isi dari starter_kit ke quarantine
  pid_t mv_pid = fork();
  if (mv_pid == 0) {
    execl("/bin/sh", "sh", "-c", "find starter_kit -type f -exec sh -c 'mv \"$1\" quarantine/ && echo \"[$(date '+%d-%m-%Y')][$(date '+%H:%M:%S')] - $(basename \"$1\") - Successfully moved to quarantine directory.\" >> activity.log' _ {} \\;", NULL);
      // set error handling kalau find gagal
      perror("Failed to execute find and move");
      exit(EXIT_FAILURE);
  } else if (mv_pid > 0) {
      wait(NULL);
    printf("Pemindahan file dari starter_kit ke quarantine selesai.\n");
      sleep(2);
  } else {
      perror("Gagal membuat proses fork");
      exit(EXIT_FAILURE);
  }
}

void stepc2(){
  // pindahkan isi dari quarantine ke starter_kit
  pid_t mv_pid = fork();
  if (mv_pid == 0) {
    execl("/bin/sh", "sh", "-c", "find quarantine -type f -exec sh -c 'mv \"$1\" starter_kit/ && echo \"[$(date '+%d-%m-%Y')][$(date '+%H:%M:%S')] - $(basename \"$1\") - Successfully moved to starter_kit directory.\" >> activity.log' _ {} \\;", NULL);
      // set error handling kalau find gagal
      perror("Failed to execute find and move");
      exit(EXIT_FAILURE);
  } else if (mv_pid > 0) {
      wait(NULL);
    printf("Pemindahan file dari quarantine ke starter_kit selesai.\n");
      sleep(2);
  } else {
    perror("Gagal membuat proses fork");
      exit(EXIT_FAILURE);
  }
}

void stepd(){
    // mari kita hapus folder quarantine
    pid_t log_pid = fork();
    if (log_pid == 0) {
        execl("/bin/sh", "sh", "-c", "find quarantine -type f -exec sh -c 'echo \"[$(date '+%d-%m-%Y')][$(date '+%H:%M:%S')] - $(basename \"$1\") - Successfully deleted.\" >> activity.log' _ {} \\;", NULL);
        perror("Failed to log deletion activity");
        exit(EXIT_FAILURE);
    } else if (log_pid > 0) {
        wait(NULL); // Wait for the logging process to finish
    } else {
        perror("Failed to fork process for logging");
        exit(EXIT_FAILURE);
    }

    // Remove all files and subdirectories inside "quarantine"
    pid_t rm_pid = fork();
    if (rm_pid == 0) {
        execl("/bin/sh", "sh", "-c", "rm -rf quarantine/*", NULL);
        perror("Failed to remove contents of quarantine folder");
        exit(EXIT_FAILURE);
    } else if (rm_pid > 0) {
        wait(NULL); // Wait for the removal process to finish
    } else {
        perror("Failed to fork process for removing contents");
        exit(EXIT_FAILURE);
    }

    // Now remove the empty "quarantine" folder
    if (rmdir("quarantine") != 0) {
        perror("Failed to delete quarantine folder");
        exit(EXIT_FAILURE);
    }
    printf("quarantine folder deleted.\n");
}

void stepf(){
    // Find and kill the process with the query "./starterkit --decrypt"
    pid_t pid = fork();
    if (pid == 0) {
        execl("/bin/sh", "sh", "-c", "ps aux | grep './starterkit --decrypt' | grep -v grep | awk '{print $2}' | while read pid; do echo \"[$(date '+%d-%m-%Y')][$(date '+%H:%M:%S')] - Successfully shut off decryption process with PID $pid.\" >> activity.log; kill -9 $pid; done", NULL);
        perror("Failed to execute kill command");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        wait(NULL); // Wait for the kill process to finish
        printf("Process daemon untuk deenkripsi telah selesai.\n");
    } else {
        perror("Failed to fork process");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {

    if (argc > 1 && strcmp(argv[1], "--decrypt") == 0) {
        printf("Decrypting...\n");
        stepb();
    } else if (argc > 1 && strcmp(argv[1], "--return") == 0) {
        printf("Moving files...\n");
        stepc2();
    } else if (argc > 1 && strcmp(argv[1], "--quarantine") == 0) {
        printf("Moving files...\n");
        stepc1();
    } else if (argc > 1 && strcmp(argv[1], "--eradicate") == 0) {
        printf("Deleting quarantine...\n");
        stepd();
    } else if (argc > 1 && strcmp(argv[1], "--shutdown") == 0) {
        printf("Shutting down...\n");
        stepf();
    } else {
        printf("Usage: ./starterkit --decrypt\n");
        stepa();
    }
    return 0;
}