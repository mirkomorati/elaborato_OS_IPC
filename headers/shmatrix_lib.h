#ifndef SHMATRIX_LIB_H
#define SHMATRIX_LIB_H
#include <sys/ipc.h>
/*! \brief Matrix loaded like shared memory type
 * 
 * inserire una descrizione decente.
 */
typedef struct {
    char *path;
    int shmid;
    key_t key;
    int fd;
    long *shmaddr;
}shmatrix_t;


/*!
 * \brief 	function that parse a .csv file and load the matrix in the shmaddr 
 * 			field of the M struct					
 *
 * \param   	M 	shmatrix_t object
 */
void shmatrix_parse(shmatrix_t *M);

/*
 * \brief      Creates an empty shmatrix and parse it in one function.
 *
 * \param      M	shmatrix_t object
 * \param[in]  N	order of the matrix
 *
 * \return     -1 in case of error 0 otherwise
 */
int shmatrix_load(shmatrix_t *M, int N);


/*!
 * \brief      Creates an empty shmatrix.
 *
 * \param      M	shmatrix_t object
 * \param[in]  N	order of the matrix
 *
 * \return     -1 in case of error 0 otherwise
 */
int shmatrix_create(shmatrix_t *M, int N);

#endif