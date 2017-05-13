#ifndef ENDING_UTILS_H
#define ENDING_UTILS_H
#include <stdbool.h>


/*!
 * \file        ending_utils.h
 */


/*!
 * \struct      Struttura che permette di liberare un'area di memoria condivisa
 */
typedef struct {
    int shmid;
    void *shmaddr;
} sig_utils_t;


/*!
 * \struct      Lista che contiene le memorie condivise
 */
typedef struct sig_shmem_list{
	sig_utils_t obj;

	struct sig_shmem_list *next;
} sig_shmem_list_t;


/*!
 * \brief      	Funzione che elimina tutte le memorie condivise del processo
 *
 * \param[in]  	sig   Il segnale
 * \param[in]     	arg   L'array di argomenti
 */
void sig_handler(int sig, void *arg);


/*!
 * \brief      	Assegna il corretto handler ad ogni segnale
 * 
 * \param[in]	list   La testa della lista degli oggetti da rimuovere
 */
void sig_init(sig_shmem_list_t *list);


/*!
 * \brief      	Aggiunge alla lista gli oggetti della memoria condivisa da
 *              eliminare
 *
 * \param[in] 	n       Numero di elementi
 * \param[in]  	args 	Gli oggetti
 */
void sig_add(int n, ...);


/*!
 * \brief		Se \setting è false il programma libera la memoria ed esce
 *              con codice 0
 *
 * \param[in]  	setting 	Se true il programma non termina
 * \param[in]	arg			La head della lista degli oggetti da rimuovere
 */
void sig_free_memory(bool setting, sig_shmem_list_t *arg);


/*!
 * \brief		Funzione per liberare la memoria prima della terminazione
 *
 * \param[in]   code  Codice di exit
 */
void sig_end(int code);

#endif
