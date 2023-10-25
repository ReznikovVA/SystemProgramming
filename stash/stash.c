#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// XOR key (must be such that "." character is not present in resulting string)
// another solution was to hide encrypted extension inside filename and add .hidden, but
// looked like less elegant solution at first :D

#define XOR_KEY 0x30

// Encrypt func
void encryptFileType(char *filename) {
    char *fileType = strrchr(filename, '.'); // Finding last "." to take file extension
    if (fileType != NULL) {
        fileType++;
        while (*fileType) {
            *fileType ^= XOR_KEY;
            fileType++;
        }
    }
}

// Decrypt func
void decryptFileType(char *filename) {
    char *fileType = strrchr(filename, '.'); // Finding last "." to take file extension
    if (fileType != NULL) {
        fileType++;
        while (*fileType) {
            *fileType ^= XOR_KEY;
            fileType++;
        }
    }
}

// hide func
void hideFileType(const char *filename) {
    char encrypted_filename[256];
    strncpy(encrypted_filename, filename, sizeof(encrypted_filename));

    encryptFileType(encrypted_filename);

    if (rename(filename, encrypted_filename) == 0) {
        printf("File type of %s has been hidden.\n", filename);
    } else {
        printf("Failed to hide the file type of %s.\n", filename);
    }
}

// restore func
void restoreFileType(const char *filename) {
    char decrypted_filename[256];
    strncpy(decrypted_filename, filename, sizeof(decrypted_filename));

    decryptFileType(decrypted_filename);

    if (rename(filename, decrypted_filename) == 0) {
        printf("File type of %s has been restored.\n", filename);
    } else {
        printf("Failed to restore the file type of %s.\n", filename);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <mode> <filename>\n", argv[0]);
        return 1;
    }

    char *mode = argv[1];
    char *filename = argv[2];

    if (strcmp(mode, "hide") == 0) {
        hideFileType(filename);
    } else if (strcmp(mode, "restore") == 0) {
        restoreFileType(filename);
    } else {
        printf("Invalid mode. Use 'hide' or 'restore'.\n");
        return 1;
    }

    return 0;
}
