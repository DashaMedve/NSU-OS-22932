#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_STR_LENGTH 255

int main() {
    int fds;
    if ((fds = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Failed to create a socket: ");
        return EXIT_FAILURE;
    }

    struct sockaddr_un address;
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, "socket", sizeof(address.sun_path) - 1);

    if (connect(fds, (struct sockaddr*)&address, sizeof(address)) == -1) {
        perror("Failed to connect on the socket: ");
        close(fds);
        return EXIT_FAILURE;
    }

    char buffer[MAX_STR_LENGTH];
    unsigned long n;
    while (1) {
        printf("Put some text here: ");
        if ((n = strlen(fgets(buffer, MAX_STR_LENGTH, stdin))) != -1) {
            send(fds, buffer, n, 0);
            if (buffer[0] == '\a')
                break;
        }
        else {
            perror("Failed to read a message: ");
            close(fds);
            return EXIT_FAILURE;
        }
    }

    close(fds);
    return EXIT_FAILURE;
}
