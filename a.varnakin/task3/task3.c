#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main (int argc, char *argv[]) {
    FILE * file;
    fprintf(stdout, "ACTUAL USER ID = %d\n EFFECTIVE USER ID = %d\n", getuid(), geteuid());

    if (argc == 1) {
        fprintf(stderr, "No arguments\n");
        exit(1);
    }
    file = fopen(argv[1], "r");
    if (!file) {
        perror("");
    }
    else
        fclose(file);

    setuid(getuid());

    fprintf(stdout, "ACTUAL USER ID = %d\n EFFECTIVE USER ID = %d\n", getuid(), geteuid());

    file = fopen(argv[1], "r");
    if (!file)
        perror("");
    else
        fclose(file);

    return 0;
}