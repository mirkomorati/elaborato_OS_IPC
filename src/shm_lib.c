#include "../headers/std_lib.h"
#include "../headers/shm_lib.h"

#define BUF_SIZE 4096

int shm_create(shm_t *M) {
    int size = M->N * M->N * sizeof(long);
    int flag = O_CREAT | 0666;
    sig_shmem_t to_delete;

    if ((M->shmid = shmget(IPC_PRIVATE, size, flag)) == -1) {
        perror("ERROR shm_create - shmget");
        return -1;
    }


    if ((M->fd = open(M->path, O_RDWR, S_IRUSR | S_IWUSR)) == -1) {
        perror("ERROR shm_create - open");
        return -1;
    }

    if (*(M->shmaddr = (long *) shmat(M->shmid, NULL, 0)) == -1) {
        perror("ERROR shm_create - shmat");
        return -1;
    }

    to_delete.shmid = M->shmid;
    to_delete.shmaddr = M->shmaddr;

    sig_add_shmem(1, &to_delete);


    return 0;
}


void shmatrix_parse(shm_t *M) {
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
//                printf("%i: %li\t", i, M->shmaddr[i]);
#endif
                i++;
            }
#ifdef DEBUG
//            printf("\n");
#endif
        }
    }
}


int shm_load(shm_t *M, bool parse) {
	if(shm_create(M) == -1) {
        perror("shm_create");
        return -1;
    }

    if (parse)
        shmatrix_parse(M);
	return 0;
}