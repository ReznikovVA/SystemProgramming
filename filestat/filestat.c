#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

int main() {
    DIR *directory;
    struct dirent *entry;
    struct stat statbuf;
    const char *file_types[] = {"Regular File", "Directory", "Character Device", "Block Device", "FIFO", "Symbolic Link", "Socket"};
    int type_counts[7] = {0};

    directory = opendir(".");

    if (directory == NULL) {
        perror("opendir");
        return 1;
    }

    while ((entry = readdir(directory)) != NULL) {
        if (stat(entry->d_name, &statbuf) == 0) {
            mode_t mode = statbuf.st_mode;

            switch (mode & S_IFMT) {
                case S_IFREG:
                    type_counts[0]++;
                    break;
                case S_IFDIR:
                    type_counts[1]++;
                    break;
                case S_IFCHR:
                    type_counts[2]++;
                    break;
                case S_IFBLK:
                    type_counts[3]++;
                    break;
                case S_IFIFO:
                    type_counts[4]++;
                    break;
                case S_IFLNK:
                    type_counts[5]++;
                    break;
                case S_IFSOCK:
                    type_counts[6]++;
                    break;
                default:
                    break;
            }
        }
    }

    closedir(directory);

    for (int i = 0; i < 7; i++) {
        printf("Number of %s: %d\n", file_types[i], type_counts[i]);
    }

    return 0;
}
