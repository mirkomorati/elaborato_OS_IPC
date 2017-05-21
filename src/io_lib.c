#include "../headers/io_lib.h"

#define BUF_SIZE 4096

void shmatrix_to_csv(shm_t *M) {
    char buf[BUF_SIZE];
    printf("FD: %i\n", M->fd);
    for (int i = 0; i < M->N; i++) {
        for (int j = 0; j < M->N; j++) {
            int bytes = sprintf(buf, (j + 1 == M->N ? "%li\n" : "%li;"), M->shmaddr[i * M->N + j]);
            if (write(M->fd, buf, bytes) != bytes) {
                perror("ERROR shmatrix_to_csv - write");
            }
        }
    }    
}