#include "headers/std_lib.h"
#include "headers/shm_lib.h"
#include "headers/ending_utils.h"

#define STDOUT 1

/*! 
 * \struct Struttura da passare a getopt() per l'alias delle opzioni
 */
static struct option long_opt[] = {
    {"matrixA",   required_argument, 0, 'A'},
    {"matrixB",   required_argument, 0, 'B'},
    {"matrixC",   required_argument, 0, 'C'},
    {"order",     required_argument, 0, 'N'},
    {"processes", required_argument, 0, 'P'},
    {"help",      no_argument,       0, 'h'},
    {0,           0,                 0,  0 }
};


int main(int argc, char **argv) {
    int opt;
    char *short_opt = "A:B:C:N:P:h";

    shmatrix_t A, B, C;
    shsum_t S;
    int N;
    int P;

    if (argc < 10) {
        char *buf = "Error: too few arguments.\n";
        write(STDOUT, buf, sizeof(char) * strlen(buf));
        exit(1);
    }

    while ((opt = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1) {
        if (opt == -1) break;
        switch (opt) {
            case 'A': 
                A.path = optarg;
                break;

            case 'B':
                B.path = optarg;
                break;

            case 'C':{
                // controllo se esiste il file altrimenti lo creo.
                struct stat st;
                if(stat(optarg, &st) == -1 && errno == ENOENT)
                    if(creat(optarg, S_IRUSR) == -1)
                        perror("Error creating matrixC file: ");
                C.path = optarg;
                break;
            }
            case 'N':
                N = atoi(optarg);
                break;
            
            case 'P':
                P = atoi(optarg);
                break;

            case 'h': {
                char *buf = "Usage: \n";
                write(STDOUT, buf, sizeof(buf));
                break;
            }
            default:
                printf("Wrong arguments\n");
                printf("Usage: \n");
                exit(1);
        }
    }

// todo da qui in poi secondo le specifiche dobbiamo spostare tutto 
// nella funzione gestita dal padre. 

#ifdef DEBUG   
    printf("Loading matrix\n");
#endif

    if (shmatrix_load(&A, N) == -1) {
        perror("shmatrix_load A");
        exit(1);
    }

    {
    	sig_utils_t tmp;
    	tmp.shmid = A.shmid;
    	tmp.shmaddr = A.shmaddr;
    	sig_add_shmem(1, &tmp);
    }

    if (shmatrix_load(&B, N) == -1) {
        perror("shmatrix_load B");
        exit(1);
    }
    
    {
    	sig_utils_t tmp;
    	tmp.shmid = B.shmid;
    	tmp.shmaddr = B.shmaddr;
    	sig_add_shmem(1, &tmp);
    }

    if (shmatrix_create(&C, N) == -1) {
        perror("shmatrix_create C");
        exit(1);
    }

    {
        sig_utils_t tmp;
        tmp.shmid = C.shmid;
        tmp.shmaddr = C.shmaddr;
        sig_add_shmem(1, &tmp);
    }

#ifdef DEBUG
    printf("Creating shm for sum\n");
#endif
    S.path = "/dev/urandom";
    if (shsum_create(&S) == -1) {
        perror("shsum_create S");
        exit(1);
    }

    {
        sig_utils_t tmp;
        tmp.shmid = S.shmid;
        tmp.shmaddr = S.shmaddr;
        sig_add_shmem(1, &tmp);
    }

#ifdef DEBUG
    printf("attendo ctrl-c..\n");
    for (int i = 0; i < 10; i++){
      usleep(1e6);
    }
#endif
    sig_end(0);

}
