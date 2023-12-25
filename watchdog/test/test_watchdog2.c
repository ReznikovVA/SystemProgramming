#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE_FILE "/dev/watchdog"

void sig_handler(int signo) {
    if (signo == SIGTERM) {
        printf("Received SIGTERM signal. Stopping the process...\n");
        exit(0);
    }
}

int main() {
    int fd;
    char buf[2] = {0};
    int count = 1;

    if (signal(SIGTERM, sig_handler) == SIG_ERR) {
        perror("Signal handler registration failed");
        return EXIT_FAILURE;
    }

    fd = open(DEVICE_FILE, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open the device");
        return EXIT_FAILURE;
    }

    while (1) {
        printf("%d\n", count++);

        if (count > 300) {
            count = 1;
        }

        sleep(1);

        // Read the watchdog device every 30 seconds
        if (count % 30 == 0) {
            if (read(fd, buf, sizeof(buf)) < 0) {
                perror("Failed to read the device");
                close(fd);
                return EXIT_FAILURE;
            }
        }
    }

    close(fd);
    return EXIT_SUCCESS;
}
