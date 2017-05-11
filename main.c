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

typedef struct {
    char *path;
    int shmid;
    key_t key;
    int fd;
    long *shmaddr;
}Matrix_t;

typedef struct {
    int shmid;
    void *shmaddr;
} shm_utils_t;

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


void parse_matrix(Matrix_t *M) {
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


int load_matrix(Matrix_t *M, int N) {
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

void signal_handler(int sig, void *arg) {
    static shm_utils_t *curr = NULL;
    int i = 0;

    if (curr == NULL) curr = arg; 
    if (sig != -1) // se non sono in fase di inizializzazione
    {
        while(curr->shmid != 0 && curr->shmaddr != NULL){
            if (shmdt(curr->shmaddr) == -1) {
                perror("shmdt");
                exit(1);
            }
            if (shmctl(curr->shmid, IPC_RMID, NULL) == -1){
                perror("shmctl");
                exit(2);
            }
            curr++;
            i++;
        }
       
        free(curr - i);
        exit(sig);
    }
}

int main(int argc, char **argv) {

    // Leggi A e B
    int opt;
    Matrix_t A, B, C;

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

    if (load_matrix(&A, N) == -1) {
        perror("load_matrix");
        exit(1);
    }

    if (load_matrix(&B, N) == -1) {
        perror("load_matrix");
        exit(1);
    }

    // signal initializer
    {
        /* array di 4 puntatori alla zona di memoria condivisa da 
        liberare in caso di segnali, più un campo che dovrà essere
        NULL per evitare di dover passare la dimensione dell'array
        all'handler */
        shm_utils_t *tmp = malloc(sizeof(shm_utils_t) * 5);
        printf("tmp = %p\n", tmp);

        // inizializzo tmp con i suoi valori
        tmp[4].shmid = 0;
        tmp[4].shmaddr = NULL;
        tmp[0].shmid = A.shmid;
        tmp[0].shmaddr = A.shmaddr;
        tmp[1].shmid = B.shmid;
        tmp[1].shmaddr = B.shmaddr;
        tmp[2].shmid = 0;
        tmp[2].shmaddr = NULL;
        tmp[3].shmid = 0;
        tmp[3].shmaddr = NULL;

        // inizializzo l'handler per il sigint
        signal(SIGINT, (void (*)(int))signal_handler);
        signal(SIGHUP, (void (*)(int))signal_handler);
        signal(SIGKILL, (void (*)(int))signal_handler);
        signal(SIGTERM, (void (*)(int))signal_handler);
        signal_handler(-1,(void *)tmp);
    }

    printf("attendo ctrl-c..\n");
    while (1){
      usleep(10000);
    }

}
