#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_STR_LENGTH 255
#define BACKLOG 3

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

    unlink(address.sun_path);
    if (bind(fds, (struct sockaddr*)&address, sizeof(address)) == -1) {
        perror("Failed to bind a name to the socket: ");
        close(fds);
        return EXIT_FAILURE;
    }

    if (listen(fds, BACKLOG) == -1) {
        perror("The socket isn't ready to listen incoming messages: ");
        close(fds);
        return EXIT_FAILURE;
    }

    fd_set set;
    fd_set tmp_set;
    FD_ZERO(&set);
    FD_SET(fds, &set);
    FD_SET(0, &set);
    int max_fd = fds;
    while (1) {
        tmp_set = set;

        if (select(max_fd + 1, &tmp_set, NULL, NULL, NULL) == -1) {
            perror("Failed to select a client: ");
            close(fds);
            return EXIT_FAILURE;
        }

        if (FD_ISSET(fds, &tmp_set)) {
            int c_fds;
            struct sockaddr c_address;
            socklen_t c_address_len = sizeof(c_address);
            if ((c_fds = accept(fds, &c_address, &c_address_len)) == -1) {
                perror("Failed to accept a connection on the socket: ");
                close(fds);
                return EXIT_FAILURE;
            }
            FD_SET(c_fds, &set);
            max_fd = c_fds > max_fd ? c_fds : max_fd;
        }

        if (FD_ISSET(0, &tmp_set))
            break;

        char buffer[MAX_STR_LENGTH];
        ssize_t n;
        for (int i = 0; i <= max_fd; ++i) {
            if (FD_ISSET(i, &tmp_set)) {
                if (i == fds)
                    continue;
                if ((n = read(i, buffer, MAX_STR_LENGTH)) != -1) {
                    if (*buffer == '\a') {
                        FD_CLR(i, &set);
                        continue;
                    }
                    for (int j = 0; j < n; ++j)
                        printf("%c", toupper(buffer[j]));
                }
                else {
                    perror("Failed to read an incoming message: ");
                    close(fds);
                    return EXIT_FAILURE;
                }
            }
        }
    }

    close(fds);
    return EXIT_SUCCESS;
}
