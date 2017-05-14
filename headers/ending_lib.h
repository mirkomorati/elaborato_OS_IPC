/*!
 * \file        ending_lib.h
 */

#ifndef ENDING_LIB_H
#define ENDING_LIB_H
#include <stdbool.h>

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
 * \struct 		Permette di memorizzare i dati necessari all'eliminazione di 
 * 				un set di semafori.
 */
typedef struct{
	int semid;
	int semnum;
} sig_sem_t;

/**
 * \struct 		Lista che verrà utilizzata per tenere traccia di tutti
 *              i semafori presenti da liberare alla terminazione
 *              del programma.
 */
typedef struct sig_sem_list{
	sig_sem_t obj;

	struct sig_sem_list *next;
} sig_sem_list_t;


/*!
 * \brief      	Funzione che elimina tutte le memorie condivise del processo
 *
 * \param[in]  	sig   Il segnale
 * \param[in]   arg   L'array di argomenti
 */
void sig_handler(int sig, void *arg);


/*!
 * \brief      	Assegna il corretto handler ad ogni segnale
 * 
 * \param[in]	shm_list   	La testa della lista della memoria condivisa da 
 * 							rimuovere.
 * \param[in]	sem_list	La testa della lista dei semafori da rimuovere.
 */
void sig_init(sig_shmem_list_t *shm_list, sig_sem_list_t *sem_list);


/*!
 * \brief      	Aggiunge gli oggetti alla lista della memoria condivisa da
 *              eliminare.
 *
 * \param[in] 	n       Numero di elementi
 * \param[in]  	args 	Gli oggetti
 */
void sig_add_shmem(int n, ...);

/*!
 * \brief      	Aggiunge alla lista dei semafori da eliminare gli oggetti
 * 				che gli vengono passati come parametri.
 *
 * \param[in] 	n       Numero di elementi
 * \param[in]  	args 	Gli oggetti
 */
void sig_add_sem(int n, ...);

/*!
 * \brief		Se \setting è false il programma libera la memoria condivisa
 * 				presente nella lista.
 *
 * \param[in]  	setting 	Se true il valore di arg viene valutato e 
 * 							inizializzato.
 * 							
 * \param[in]	arg			La head della lista degli oggetti da rimuovere
 */
void sig_free_memory(bool setting, sig_shmem_list_t *arg);

/*!
 * \brief		Se \setting è false il programma elimina i semafori presenti 
 * 				nella lista dei semafori.
 *
 * \param[in]  	setting 	Se true il valore di arg viene valutato e 
 * 							inizializzato.
 * 							
 * \param[in]	arg			La head della lista dei semafori da rimuovere
 */
void sig_free_sem(bool setting, sig_sem_list_t *arg);

/*!
 * \brief		Funzione per liberare la memoria prima della terminazione
 *
 * \param[in]   code  Codice di exit
 */
void sig_end(int code);

#endif
