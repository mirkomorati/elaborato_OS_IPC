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

#include "headers/shmatrix_lib.h"
#include "headers/sig_utils.h"

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

int main(int argc, char **argv) {

    // Leggi A e B
    int opt;
    shmatrix_t A, B, C;

    long * sum = NULL; // indirizzo alla zona di memoria condivisa che serve per contenere la somma
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

    if (shmatrix_load(&A, N) == -1) {
        perror("load_shmatrix");
        exit(1);
    }

    {
    	sig_utils_t tmp;
    	tmp.shmid = A.shmid;
    	tmp.shmaddr = A.shmaddr;
    	sig_add(1, &tmp);
    }

    if (shmatrix_load(&B, N) == -1) {
        perror("load_shmatrix");
        exit(1);
    }
    
    {
    	sig_utils_t tmp;
    	tmp.shmid = B.shmid;
    	tmp.shmaddr = B.shmaddr;
    	sig_add(1, &tmp);
    }

    printf("attendo ctrl-c..\n");
    while (1){
      usleep(10000);
    }

}