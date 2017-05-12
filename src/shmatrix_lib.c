#include "../headers/shmatrix_lib.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/types.h>

#include <stdio.h>

#define BUF_SIZE 4096

int shmatrix_create(shmatrix_t *M, int N){
	key_t key;
    int size = N * N * sizeof(long);
    int flag = O_CREAT | 0644;

    if ((key = ftok(M->path, 'A')) == -1) {
        perror("ftok");
        return -1;
    }

#ifdef DEBUG
    printf("---INFO\n");
    printf("key: %x\tsize: %i\tflag: %i\n", key, size, flag);
#endif

    if ((M->shmid = shmget(key, size, flag)) == -1) {
        perror("shmget");
        return -1;
    }
    
    if ((M->fd = open(M->path, O_RDONLY, S_IRUSR)) == -1) {
        perror("open");
        return -1;
    }

    if (*(M->shmaddr = (long *) shmat(M->shmid, NULL, 0)) == -1) {
        perror("shmat");
        return -1;
    }

#ifdef DEBUG   
    printf("---INFO2\n");
    printf("path: %s\nkey: %x\nid: %i\nfd: %i\naddr: %li\n", M->path, key, M->shmid, M->fd, *M->shmaddr);
#endif

    return 0;
}


void shmatrix_parse(shmatrix_t *M){
	char buf[BUF_SIZE];
    int i = 0;
    char *line, *value, *brkt, *brkb;

#ifdef DEBUG
    printf("---PARSING %s\n", M->path);
#endif

    while (read(M->fd, buf, BUF_SIZE) != 0) {

        for (line = strtok_r(buf, "\n", &brkt); line; line = strtok_r(NULL, "\n", &brkt)) {

            for (value = strtok_r(line, ";", &brkb); value; value = strtok_r(NULL, ";", &brkb)) {
                M->shmaddr[i] = atol(value);
#ifdef DEBUG
                printf("%i: %li\t", i, M->shmaddr[i]);
#endif
                i++;
            }
#ifdef DEBUG
            printf("\n");
#endif
        }
    }
}


int shmatrix_load(shmatrix_t *M, int N){
	if(shmatrix_create(M, N) == -1) return -1;

	shmatrix_parse(M);
	return 0;
}
