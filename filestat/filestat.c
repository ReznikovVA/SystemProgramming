#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

int main()
{
    DIR *directory;
    struct dirent *entry;
    int count = 0;

    directory = opendir(".");

    if (directory == NULL) {
        perror("opendir");
        return 1;
    }

    while ((entry = readdir(directory))) {
        struct stat statbuf;

        if (stat(entry->d_name, &statbuf) == 0) {
            if (S_ISREG(statbuf.st_mode)) {
                count++;
            }
        }
    }

    closedir(directory);
    printf("Number of files is: %d\n", count);

    return 0;
}
