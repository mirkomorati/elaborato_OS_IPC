#include "../headers/thread_lib.h"

int use_thread(char *A_path, char *B_path, char *C_path, int N) {
    thread_arg_t args;

    int A_fd, B_fd, C_fd;

    if ((A_fd = open(A_path, O_RDWR, S_IRUSR | S_IWUSR)) == -1) {
        sys_err("ERROR use_thread - open");
        return -1;
    }

    if ((B_fd = open(B_path, O_RDWR, S_IRUSR | S_IWUSR)) == -1) {
        sys_err("ERROR use_thread - open");
        return -1;
    }

    if ((C_fd = open(C_path, O_RDWR, S_IRUSR | S_IWUSR)) == -1) {
        sys_err("ERROR use_thread - open");
        return -1;
    }

    args.matrixA = (long *) malloc(sizeof(long) * N * N);
    args.matrixB = (long *) malloc(sizeof(long) * N * N);
    args.matrixC = (long *) malloc(sizeof(long) * N * N);

    matrix_from_csv(A_fd, args.matrixA, N);
    matrix_from_csv(B_fd, args.matrixB, N);

#ifdef DEBUG
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            sys_print(STDOUT, "%i\t", args.matrixA[i * N + j]);
        }
        sys_print(STDOUT, "\n");
    }
    sys_print(STDOUT, "\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            sys_print(STDOUT, "%i\t", args.matrixB[i * N + j]);
        }
        sys_print(STDOUT, "\n");
    }
#endif

    free(args.matrixA);
    free(args.matrixB);
    free(args.matrixC);
    return 0;
}

void matrix_to_csv(int fd, long *matrix, int N) {
    char buf[BUF_SIZE];
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int bytes = sprintf(buf, (j + 1 == N ? "%li\n" : "%li,"), matrix[i * N + j]);
            if (write(fd, buf, bytes) != bytes) {
                sys_err("ERROR matrix_to_csv - write");
            }
        }
    }    
}

void matrix_from_csv(int fd, long *matrix, int N) {
    char *buf;
    int i = 0;
    char *line, *value, *brkt, *brkb;

    int curr = lseek(fd, 0, SEEK_CUR);
    int buf_size = lseek(fd, 0, SEEK_END);
    lseek(fd, curr, SEEK_SET);
    buf = (char *) malloc(sizeof(char) * buf_size + 1);

    if (read(fd, buf, buf_size) != 0) {
        buf[buf_size] = '\0';
        for (line = strtok_r(buf, "\n", &brkt); line; line = strtok_r(NULL, "\n", &brkt)) {
            for (value = strtok_r(line, ",", &brkb); value; value = strtok_r(NULL, ",", &brkb)) {
                matrix[i] = atol(value);
                i++;
            }
        }
    }

    free(buf);
}