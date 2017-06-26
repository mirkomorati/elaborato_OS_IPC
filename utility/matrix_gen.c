#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#define MAX_RAND 10
#define OFFSET -5

// argv: pathname minN maxN

int main(int argc, char **argv) {
    int fd;

    srand(time(NULL));
    if (argc < 4)
        printf("Error\n");

    char buffer[20];
    int N = atoi(argv[2]);
    int maxN = atoi(argv[3]);

    for (int l = N; l <= maxN; l++) {
        char path[255];
        sprintf(path, "%s%d", argv[1], l);
        if ((fd = open(path, O_WRONLY | O_CREAT | O_EXCL, 0644)) <= 0) {
            if ((fd = open(path, O_WRONLY | O_EXCL | O_TRUNC)) == -1) {
                perror("open");
                exit(1);
            }
        }

        printf("generating %s, order %i\n", path, l);

        for (int i = 0; i < l; i++) {
            for (int j = 0; j < l; j++) {
                int bytes = sprintf(buffer, (j + 1 == l ? "%d\n" : "%d,"), rand() % MAX_RAND + OFFSET);
                write(fd, buffer, bytes);
            }
        }
        close(fd);
    }

    return 0;
}
