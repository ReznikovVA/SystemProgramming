#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

//Hide func
void hideFile(const char *filename) {
    const char *dark_directory = ".dark_directory";

    mkdir(dark_directory, 0700);

    char cmd[256];
    snprintf(cmd, sizeof (cmd), "mv %s %s/%s", filename, dark_directory, filename);

    if (system(cmd) == 0) {
        printf("File %s has been hidden.\n", filename);
    }
    else {
        printf("Failed to hide file");
    }
}


//Restore func
void restoreFile(const char *filename) {
    const char *dark_directory = ".dark_directory";

    //char dark_path[256];
    //snprintf(dark_path, sizeof(dark_path), "%s/%s", dark_directory, filename);

    char cmd[256];
    snprintf (cmd, sizeof(cmd), "mv %s/%s %s", dark_directory, filename, filename);

    if (system(cmd) == 0) {
        printf("File %s has been restored.\n", filename);
    }
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Use: %s <mode> <filename>\n", argv[0]);
        return 1;
    }

    char *mode = argv[1];
    char *filename = argv[2];

    if (strcmp (mode, "hide") == 0) {
        hideFile(filename);
    }
    else if (strcmp (mode, "restore") == 0) {
        restoreFile(filename);
    }
    else {
        printf("Invalide mode, use 'hide' or 'restore'.\n");
        return 1;
    }

    return 0;
}

