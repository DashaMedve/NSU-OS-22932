#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <aio.h>
#include <signal.h>

#define MAX_STR_LENGTH 255
#define BACKLOG 3

void io_handler(int signum, siginfo_t* info, void* context) {
    struct aiocb* current = info->si_value.sival_ptr;
    if (aio_error(current) == 0) {
        size_t n_read = aio_return(current);
        char* buffer = (char*)current->aio_buf;
        if (n_read == 0 || *buffer == '\a') {
            close(current->aio_fildes);
            free(buffer);
            free(current);
        }
        else {
            size_t i;
            for (i = 0; i < n_read; ++i)
                printf("%c", toupper(buffer[i]));
            aio_read(current);
        }
    }
    else {
        perror("An error occurred while executing the request: ");
    }
}

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
    if (bind(fds, (struct sockaddr *) &address, sizeof(address)) == -1) {
        perror("Failed to bind a name to the socket: ");
        close(fds);
        return EXIT_FAILURE;
    }

    if (listen(fds, BACKLOG) == -1) {
        perror("The socket isn't ready to listen incoming messages: ");
        close(fds);
        return EXIT_FAILURE;
    }

    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_sigaction = io_handler;
    act.sa_flags = SA_SIGINFO | SA_RESTART;
    sigaction(SIGIO, &act, NULL);

    while (1) {
        int c_fds;
        if ((c_fds = accept(fds, NULL, NULL)) == -1) {
            perror("Failed to accept a connection on the socket: ");
            close(fds);
            return EXIT_FAILURE;
        }

        struct aiocb* cb = calloc(1, sizeof(struct aiocb));
        cb->aio_fildes = c_fds;
        cb->aio_buf = malloc(MAX_STR_LENGTH);
        cb->aio_nbytes = MAX_STR_LENGTH;
        cb->aio_sigevent.sigev_signo = SIGIO;
        cb->aio_sigevent.sigev_value.sival_ptr = cb;

        if (aio_read(cb) == -1) {
            perror("Failed to queue the read request: ");
            close(fds);
            close(c_fds);
        }
    }
}
