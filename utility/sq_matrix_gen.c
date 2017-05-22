#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define MAX_RAND 100
#define OFFSET -50

int main(int argc, char **argv) {
    int fd;

    srand(time(NULL));

    if ((fd = open(argv[1], O_WRONLY | O_CREAT | O_EXCL, 0644)) <= 0) {
        if ((fd = open(argv[1], O_WRONLY | O_EXCL | O_TRUNC)) == -1) {
            perror("open");
            exit(1);
        }
    }

    printf("generating %s (fd: %d)\n", argv[1], fd);

    char buffer[20];
    int N = atoi(argv[2]); 
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int bytes = sprintf(buffer, (j + 1 == N ? "%d\n" : "%d;"), rand() % MAX_RAND + OFFSET);
            write(fd, buffer, bytes);
        }
    }
    
    return 0;
}
