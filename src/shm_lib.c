#include "../headers/std_lib.h"
#include "../headers/shm_lib.h"

#define BUF_SIZE 4096

int shm_create(shm_t *M) {
	key_t key;
    int size = M->N * M->N * sizeof(long);
    int flag = O_CREAT | 0644;
    sig_shmem_t to_delete;

#ifdef DEBUG
    printf("---SHM OBJ\n");
    printf("PATH: %s\n", M->path);
#endif

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
    printf("path: %s\nkey: %x\nid: %i\nfd: %i\naddr: %li\n",
            M->path, key, M->shmid, M->fd, *M->shmaddr);
#endif
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


int shm_load(shm_t *M, bool parse) {
	if(shm_create(M) == -1) {
        perror("shm_create");
        return -1;
    }

    if (parse)
        shmatrix_parse(M);
	return 0;
}