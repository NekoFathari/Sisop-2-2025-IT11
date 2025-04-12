#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/wait.h>

void run(char *args[]) {
    if (fork() == 0) {
        execvp(args[0], args);
        exit(1);
    } else {
        wait(NULL);
    }
}

void downloadDanUnzip() {
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

int validNama(const char *nama) {
    return strlen(nama) == 5 && isalnum(nama[0]) && strcmp(nama+1, ".txt") == 0;
}

void filterFile() {
    mkdir("Filtered", 0755);

    const char *clue[] = {"Clues/ClueA", "Clues/ClueB", "Clues/ClueC", "Clues/ClueD"};
    for (int i = 0; i < 4; i++) {
        DIR *dir = opendir(clue[i]);
        struct dirent *ent;

        while ((ent = readdir(dir)) != NULL) {
            char asal[512], tujuan[512];
            snprintf(asal, sizeof(asal), "%s/%s", clue[i], ent->d_name);

            // langsung cek nama file aja, tanpa cek tipe file
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

int banding(const void *a, const void *b) {
    return strcmp(*(char **)a, *(char **)b);
}

void gabungFile() {
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

char rot13(char c) {
    if (c >= 'a' && c <= 'z') return ((c - 'a' + 13) % 26) + 'a';
    if (c >= 'A' && c <= 'Z') return ((c - 'A' + 13) % 26) + 'A';
    return c;
}

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

int main(int argc, char *argv[]) {
    if (argc == 1) {
        downloadDanUnzip();
    } else if (argc == 3 && strcmp(argv[1], "-m") == 0) {
        if (strcmp(argv[2], "Filter") == 0) {
            filterFile();
        } else if (strcmp(argv[2], "Combine") == 0) {
            gabungFile();
        } else if (strcmp(argv[2], "Decode") == 0) {
            decodeFile();
        } else {
            printf("Gunakan: -m Filter | Combine | Decode\n");
        }
    } else {
        printf("Cara pakai:\n");
        printf("  ./action\n");
        printf("  ./action -m Filter\n");
        printf("  ./action -m Combine\n");
        printf("  ./action -m Decode\n");
    }

    return 0;
}
