#ifndef SHMATRIX_LIB_H
#define SHMATRIX_LIB_H
#include <sys/ipc.h>

/*! 
 * \struct      Struttura per rappresentare una matrice in memoria condivisa
 */
typedef struct {
    char *path;     //!< Path del file contenente la matrice
    int fd;         //!< File Descriptor
    int shmid;      //!< ID della memoria condivisa
    key_t key;      //!< Chiave generata dal sistema
    long *shmaddr;  //!< Indirizzo di attacco della memoria condivisa
}shmatrix_t;


/*!
 * \brief       Funzione per parsare il file contenente la matrice e caricarla
 *              in memoria condivisa
 *
 * \param       M   puntatore a un oggetto shmatrix_t
 */
void shmatrix_parse(shmatrix_t *M);


/*!
 * \brief       Chiama le funzioni utili a creare un nuovo oggetto vuoto
 *              shmatrix, parsarlo e caricarlo in memoria condivisa
 *
 * \param       M	puntatore a un oggetto shmatrix_t
 * \param[in]   N	grado della matrice
 *
 * \return      -1 in caso di errore, 0 altrimenti
 */
int shmatrix_load(shmatrix_t *M, int N);


/*!
 * \brief       Crea un oggetto vuoto shmatrix
 *
 * \param       M	puntatore a un oggetto shmatrix_t
 * \param[in]   N	grado della matrice
 *
 * \return      -1 in caso di errore, 0 altrimenti
 */
int shmatrix_create(shmatrix_t *M, int N);

#endif
