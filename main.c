/**
 * @file main.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>

#define BUF_SIZE 4096

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


void parse_matrix(long int *addr, int fd) {
    char buf[BUF_SIZE];
    int i = 0;

    while (read(fd, buf, BUF_SIZE) != EOF) {
        char *line;

        for (line = strtok(buf, "\n"); line; line = strtok(NULL, "\n")) {
            char *value;
            
            for (value = strtok(line, ";"); value; value = strtok(NULL, ";")) {
                addr[i] = atol(value);    
            }
        }
    }
}


int load_matrix(long int **addr, const char *path, int N) {
    key_t key;
    int id;
    int size = N * N * sizeof(long int);
    int flag = O_CREAT | 0644;

    if ((key = ftok(path, 'R')) == -1) {
        perror("ftok");
        return -1;
    }

    if ((id = shmget(key, size, flag)) == -1) {
        perror("shmget");
        return -1;
    }

    int fd;
    
    if ((fd = open(path, O_RDONLY, S_IRUSR)) == -1) {
        perror("open");
        return -1;
    }

    if ((*addr = (long int *) shmat(id, NULL, 0)) == -1) {
        perror("shmat");
        return -1;
    }

    parse_matrix(addr, fd);

    return 0;
}


int main(int argc, char **argv) {

    // Leggi A e B
    int opt;

    char *mA, *mB, *mC;
    long int *mA_addr, *mB_addr, *mC_addr;
    int N;
    int P;

    while ((opt = getopt_long(argc, argv, "A:B:C:N:P:h", long_options, NULL)) != -1) {
        if (opt == -1) break;
        switch (opt) {
            case 'A': 
                    mA = optarg;
                break;

            case 'B':
                    mB = optarg;
                break;

            case 'C':
                    mC = optarg;
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
    
    if (load_matrix(&mA_addr, mA, &N) == -1) {
        perror("load_matrix");
        exit(1);
    }

    if (load_matrix(&mB_addr, mB, &N) == -1) {
        perror("load_matrix");
        exit(1);
    }
}
