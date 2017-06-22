#include "../headers/std_lib.h"
#include "../headers/shm_lib.h"
#include "../headers/io_lib.h"


int shm_create(shm_t *M) {
    int size = M->N * M->N * sizeof(long);
    int flag = O_CREAT | 0666;
    sig_shmem_t to_delete;

    if ((M->shmid = shmget(IPC_PRIVATE, size, flag)) == -1) {
        sys_err("ERROR shm_create - shmget");
        return -1;
    }


    if ((M->fd = open(M->path, O_RDWR, S_IRUSR | S_IWUSR)) == -1) {
        sys_err("SYS_ERROR shm_create - open");
        return -1;
    }

    if (*(M->shmaddr = (long *) shmat(M->shmid, NULL, 0)) == -1) {
        sys_err("ERROR shm_create - shmat");
        return -1;
    }

    to_delete.shmid = M->shmid;
    to_delete.shmaddr = M->shmaddr;

    sig_add_shmem(1, &to_delete);

    return 0;
}


int shm_load(shm_t *M, bool parse) {
	if(shm_create(M) == -1) {
        sys_err("shm_create");
        return -1;
    }

    if (parse)
        shmatrix_from_csv(M);
	return 0;
}


void shmatrix_to_csv(shm_t *M) {
    char buf[BUF_SIZE];
    for (int i = 0; i < M->N; i++) {
        for (int j = 0; j < M->N; j++) {
            int bytes = sprintf(buf, (j + 1 == M->N ? "%li\n" : "%li,"), M->shmaddr[i * M->N + j]);
            if (write(M->fd, buf, bytes) != bytes) {
                sys_err("ERROR shmatrix_to_csv - write");
            }
        }
    }    
}

void shmatrix_from_csv(shm_t *M) {
    char buf[5093] = {'\0'};
    int i = 0, sum = 0;
    char *line, *value, *brkt, *brkb;
    int read_size;

    printf("\nmatrice\n");
    while ((read_size = read(M->fd, buf, 5093)) != 0) {
        buf[read_size] = '\0'; // strtok wants a null terminated string.
        for (line = strtok_r(buf, "\n", &brkt); line; line = strtok_r(NULL, "\n", &brkt)) {
            for (value = strtok_r(line, ",", &brkb); value; value = strtok_r(NULL, ",", &brkb)) {
                M->shmaddr[i] = atol(value);
                sum++;
                printf("%li ", atol(value));
                i++;
            }
            printf("\n");
        }
    }
    printf("\n%i\n\n", sum);
}