/*!
 * \file        shm_lib.h
 */

#ifndef SHM_LIB_H
#define SHM_LIB_H

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
 * \typedef     L'oggetto che rappresenterà l'area di memoria condivisa per la
 *              somma degli elementi è lo stesso delle matrici
 */
typedef shmatrix_t shsum_t;

/*!
 * \brief       Funzione per parsare il file contenente la matrice e caricarla
 *              in memoria condivisa
 *
 * \param       M   Puntatore a un oggetto shmatrix_t
 */
void shmatrix_parse(shmatrix_t * M);

/*!
 * \brief       Chiama le funzioni utili a creare un nuovo oggetto vuoto
 *              shmatrix_t, parsarlo e caricarlo in memoria condivisa
 *
 * \param[out]  M	Puntatore a un oggetto shmatrix_t
 * \param[in]   N	Grado della matrice
 *
 * \return      -1 in caso di errore, 0 altrimenti
 */
int shmatrix_load(shmatrix_t * M, int N);

/*!
 * \brief       Crea un oggetto vuoto shmatrix_t
 *
 * \param[out]  M	Puntatore a un oggetto shmatrix_t
 * \param[in]   N	Grado della matrice
 *
 * \return      -1 in caso di errore, 0 altrimenti
 */
int shmatrix_create(shmatrix_t * M, int N);

/*!
 * \brief       Crea un oggetto vuoto shsum_t
 * 
 * \param[out]  S   Puntatore a un oggetto shsum_t
 */
int shsum_create(shsum_t * S);

#endif