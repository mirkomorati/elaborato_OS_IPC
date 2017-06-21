#include <stdio.h>
#include <stdio.h>      // per perror.
#include <stdlib.h> 
#include <unistd.h>     // malloc etc.
#include <fcntl.h> 
#include <string.h>

int main(int argc, char const *argv[])
{
	int fd1, fd2, dimension, i = 0, exit_flag = 0;
	long *matrix1, *matrix2;
	char buf[4096];
	char *line, *value, *brkt, *brkb;

	if (argc < 4){
		printf("usage: %s csv_matrix1 csv_matrix2 dimension\n", argv[0]);
		return -1;
	}

	if((fd1 = open(argv[1], O_RDWR, S_IRUSR | S_IWUSR)) == -1){
		printf("cannot open %s\n", argv[1]);
	}
	if((fd2 = open(argv[2], O_RDWR, S_IRUSR | S_IWUSR)) == -1){
		printf("cannot open %s\n", argv[2]);
	}
	dimension = atoi(argv[3]);

	matrix1 = (long *) malloc(sizeof(long) * dimension * dimension);
	matrix2 = (long *) malloc(sizeof(long) * dimension * dimension);

	while (read(fd1, buf, 4096) != 0) {

        for (line = strtok_r(buf, "\n", &brkt); line; line = strtok_r(NULL, "\n", &brkt)) {

            for (value = strtok_r(line, ",", &brkb); value; value = strtok_r(NULL, ",", &brkb)) {
                matrix1[i] = atol(value);
                i++;
            }
        }
    }

    i = 0;

    while (read(fd2, buf, 4096) != 0) {

        for (line = strtok_r(buf, "\n", &brkt); line; line = strtok_r(NULL, "\n", &brkt)) {

            for (value = strtok_r(line, ",", &brkb); value; value = strtok_r(NULL, ",", &brkb)) {
                matrix2[i] = atol(value);
                i++;
            }
        }
    }

    for (i = 0; i < dimension * dimension; ++i)
    {
    	if (matrix1[i] != matrix2[i]){
    		printf("error: index = %i\tfirst = %li\tsecond = %li\n", i, matrix1[i], matrix2[i]);
            exit_flag = 1;
    	}
    }

    free(matrix1);
    free(matrix2);

	return exit_flag == 1 ? -1 : 0;
}