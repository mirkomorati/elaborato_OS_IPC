#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 32

int main(int argc, char **argv) {
    char buf[BUF_SIZE] = {0};
    int i = 0;
    char *line, *value, *brkt, *brkb;

#ifdef DEBUG
    sys_print(STDOUT, "---PARSING %s\n", M->path);
#endif

    int fd = open(argv[1], O_RDWR, S_IRUSR | S_IWUSR);

    while (read(fd, buf, BUF_SIZE) != 0) {
        printf("BUFFER:\n%s\n", buf);
        buf[BUF_SIZE - 1] = '\0';
        for (line = strtok_r(buf, "\n", &brkt); line; line = strtok_r(NULL, "\n", &brkt)) {

            for (value = strtok_r(line, ",", &brkb); value; value = strtok_r(NULL, ",", &brkb)) {
                printf("%li\t", atol(value));
            }
            printf("\n");
        }
    }
}
