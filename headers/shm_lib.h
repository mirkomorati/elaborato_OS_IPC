/*!
 * \file        shm_lib.h
 */

#ifndef SHM_LIB_H
#define SHM_LIB_H
#include "ending_lib.h"

/*! 
 * \brief   Struttura per rappresentare una memoria condivisa
 * 
 */
typedef struct {
    char *path;     //!< Path del file contenente la matrice
    int N;			//!< Ordine della matrice. 1 nel caso che sia il campo somma.
    int fd;         //!< File Descriptor
    int shmid;      //!< ID della memoria condivisa
    long *shmaddr;  //!< Indirizzo di attacco della memoria condivisa
} shm_t;

/*!
 * \brief       Chiama le funzioni utili a creare un nuovo oggetto vuoto
 *              shm_t, parsarlo e caricarlo in memoria condivisa
 *
 * \param[out]  M	Puntatore a un oggetto shm_t
 * \param[in]  	parse   Booleano per capire se la matrice dev'essere
 *                      parsata o solo creata
 * \return      -1 in caso di errore, 0 altrimenti
 */
int shm_load(shm_t *M, bool parse);

/*!
 * \brief       Crea un oggetto vuoto shm_t
 *
 * \param[out]  M	Puntatore a un oggetto shm_t
 *
 * \return      -1 in caso di errore, 0 altrimenti
 */
int shm_create(shm_t *M);

/*!
 * \brief      	Funzione per parsare il file contenente la matrice e caricarla
 *             	in memoria condivisa
 *
 * \param[in]  	M   Puntatore a un oggetto shm_t
 */
void shmatrix_from_csv(shm_t *M);

/**
 * @brief      Funzione per convertire la matrice caricata in memoria condivisa
 *             e salvarla su file in formato csv
 *
 * @param[in]  M     La matrice da convertire
 * 
 */     
void shmatrix_to_csv(shm_t *M);
#endif