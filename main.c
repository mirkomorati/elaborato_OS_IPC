/**
 * @file main.c
 */

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
#include <sys/ipc.h>

#define BUF_SIZE 4096

struct Matrix {
    char *path;
    int shmid;
    key_t key;
    int fd;
    long *shmaddr;
};

static struct option long_options[] = {
    /* These options don’t set a flag.
     We distinguish them by their indices. */
    {"matrixA", required_argument, 0, 'A'},
    {"matrixB", required_argument, 0, 'B'},
    {"matrixC", required_argument, 0, 'C'},
    {"order", required_argument, 0, 'N'},
    {"processes", required_argument, 0, 'P'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
};


void parse_matrix(struct Matrix *M) {
    char buf[BUF_SIZE];
    int i = 0;
    char *line, *value, *brkt, *brkb;
    printf("---PARSING %s\n", M->path);
    while (read(M->fd, buf, BUF_SIZE) != 0) {

        for (line = strtok_r(buf, "\n", &brkt); line; line = strtok_r(NULL, "\n", &brkt)) {

            for (value = strtok_r(line, ";", &brkb); value; value = strtok_r(NULL, ";", &brkb)) {
                M->shmaddr[i] = atol(value);    
                printf("%i: %li\t", i, M->shmaddr[i]);
                i++;
            }
            
            printf("\n");
        }
    }
}


int load_matrix(struct Matrix *M, int N) {
    key_t key;
    int size = N * N * sizeof(long);
    int flag = O_CREAT | 0644;

    if ((key = ftok(M->path, 'A')) == -1) {
        perror("ftok");
        return -1;
    }
    
    printf("---INFO\n");
    printf("key: %x\tsize: %i\tflag: %i\n", key, size, flag);
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
    
    printf("---INFO2\n");
    printf("path: %s\nkey: %x\nid: %i\nfd: %i\naddr: %li\n", M->path, key, M->shmid, M->fd, *M->shmaddr);

    parse_matrix(M);

    return 0;
}

void sigint_handler(int sig, long **addr, ...) {
    va_list ptr;
    long **curr;
    va_start(ptr, addr);
    for (curr = va_arg(ptr, long **); curr != NULL; curr = va_arg(ptr, long**)) {
        if (shmdt(curr) == -1) {
            perror("shmdt");
            exit(1);
        }
    }
    exit(sig);
}




int main(int argc, char **argv) {

    // Leggi A e B
    int opt;
    
    struct Matrix A, B, C;
    int N;
    int P;

    while ((opt = getopt_long(argc, argv, "A:B:C:N:P:h", long_options, NULL)) != -1) {
        if (opt == -1) break;
        switch (opt) {
            case 'A': 
                    A.path = optarg;
                break;

            case 'B':
                    B.path = optarg;
                break;

            case 'C':
                    C.path = optarg;
                break;

            case 'N':
                    N = atoi(optarg);
                break;
            
            case 'P':
                    P = atoi(optarg);
                break;

            case 'h':
                    printf("Usage: \n");
                break;
            
            default:
                printf("Wrong arguments\n");
                printf("Usage: \n");
                exit(1);
        }
    }
   
    printf("Loading matrix\n");

    if (load_matrix(&A, N) == -1) {
        perror("load_matrix");
        exit(1);
    }

    if (load_matrix(&B, N) == -1) {
        perror("load_matrix");
        exit(1);
    }

/* l'idea è di gestire un eventuale CTRL-C per eliminare anche le shm
 * come ?

    signal(SIGINT, (void (*)(int))sigint_handler);
    sigint_handler(SIGINT, &mA_addr, &mB_addr, NULL);

*/
}
