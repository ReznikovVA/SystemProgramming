#include <stdio.h>
#include <stdint.h>
#include <string.h>

// XOR key
#define XOR_KEY 0x30

// encrypt
void xor_encrypt(uint8_t *buffer, size_t size) {
    for (size_t i = 0; i < size; i++) {
        buffer[i] ^= XOR_KEY;
    }
}

// decrypt
void xor_decrypt(uint8_t *buffer, size_t size) {
    xor_encrypt(buffer, size);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <mode> <file_path>\n", argv[0]);
        return 1;
    }

    const char *mode = argv[1];
    const char *file_path = argv[2];

    FILE *file = fopen(file_path, "rb+");
    if (file == NULL) {
        perror("fopen");
        return 1;
    }

    // Read first 8 bytes (magic number)
    uint8_t magic_number[8];
    if (fread(magic_number, 1, sizeof(magic_number), file) != sizeof(magic_number)) {
        perror("fread");
        fclose(file);
        return 1;
    }

    if (strcmp(mode, "hide") == 0) {
        xor_encrypt(magic_number, sizeof(magic_number));

        // move to the beginning of the file to write the encrypted number
        if (fseek(file, 0, SEEK_SET) != 0 || fwrite(magic_number, 1, sizeof(magic_number), file) != sizeof(magic_number)) {
            perror("fseek/fwrite");
        }

        fclose(file);
        printf("Magic number hidden in %s.\n", file_path);
    } 
	else if (strcmp(mode, "restore") == 0) {

        xor_decrypt(magic_number, sizeof(magic_number));

        // move to the beginning of the file to write the dencrypted number
        if (fseek(file, 0, SEEK_SET) != 0 || fwrite(magic_number, 1, sizeof(magic_number), file) != sizeof(magic_number)) {
            perror("fseek/fwrite");
        }

        fclose(file);
        printf("Decrypted Magic Number restored for: %s.\n", file_path);
    } 
	else {
        printf("Invalid usage. Use 'hide' or 'restore'.\n");
        fclose(file);
        return 1;
    }

    return 0;
}
