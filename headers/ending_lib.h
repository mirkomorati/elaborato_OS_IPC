/*!
 * \file        ending_lib.h
 */

#ifndef ENDING_LIB_H
#define ENDING_LIB_H
#include <stdbool.h>
#include "io_lib.h"

/*!
 * \brief	Struttura che permette di liberare un'area di memoria condivisa
 * 
 */
typedef struct {
    int shmid;
    void *shmaddr;
} sig_shmem_t;


/*!
 * \brief  	Lista che contiene le memorie condivise
 * 
 */
typedef struct sig_shmem_list {
	sig_shmem_t obj;
	struct sig_shmem_list *next;
} sig_shmem_list_t;

/*!
 * \brief	permette di memorizzare i dati necessari all'eliminazione di 
 * 			un set di semafori.
 * 			
 */
typedef struct {
	int semid;
	int semnum;
} sig_sem_t;

/*!
 * \brief 	Lista che verrà utilizzata per tenere traccia di tutti
 *          i semafori presenti da liberare alla terminazione
 *          del programma
 *          
 */
typedef struct sig_sem_list {
	sig_sem_t obj;
	struct sig_sem_list *next;
} sig_sem_list_t;

/*!
 * \brief 	Rappresenta una lista di interi, che verrà utilizzata per 
 * 			memorizzare tutti gli id di tutte le code di messaggi che 
 * 			verranno create dal programma, così che possano essere 
 * 			liberate prima della terminazione.
 * 				
 */
typedef struct sig_queue_list {
	int obj;
	struct sig_queue_list *next;
} sig_queue_list_t;

/*!
 * \brief      	Funzione che elimina tutte le memorie condivise del processo
 *
 * \param[in]  	sig  	Il segnale
 * \param[in]   pid   	Il pid del padre
 */
void sig_handler(int sig, int pid);

/*!
 * @brief      	Assegna il corretto handler ad ogni segnale
 *
 * @param[in]  	shm_list   	La testa della lista della memoria condivisa da
 *                         	rimuovere.
 * @param[in]  	sem_list    La testa della lista dei semafori da rimuovere.
 * @param[in]  	queue_list  La testa della lista delle code da rimuovere.
 */
void sig_init(sig_shmem_list_t *shm_list, sig_sem_list_t *sem_list, sig_queue_list_t *queue_list);

/*!
 * @brief      	Aggiunge gli oggetti alla lista della memoria condivisa da
 *             	eliminare.
 *
 * @param[in]  	n       Numero di elementi
 * @param[in]  	args  	Gli oggetti
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
 * \brief      	Aggiunge un alemento alla lista delle code di messaggi da
 * 				eliminare.
 *
 * \param[in] 	n       Numero di elementi
 * \param[in]  	args 	Elementi.
 */
void sig_add_queue(int n, ...);

/*!
 * \brief		Se setting è false il programma libera la memoria condivisa
 * 				presente nella lista.
 *
 * \param[in]  	setting 	Se true il valore di arg viene valutato e 
 * 							inizializzato.							
 * \param[in]	arg			La head della lista degli oggetti da rimuovere
 */
void sig_free_memory(bool setting, sig_shmem_list_t *arg);

/*!
 * \brief      	Se setting è false il programma effettua il detach 
 * 				della memoria condivisa presente nella lista.
 * 				
 * \param[in]  	setting  	Se true il valore di arg viene valutato e 
 * 							inizializzato.
 * \param      	arg      	La head della lista degli oggetti di memoria 
 * 							condivisa 
 */
void sig_shmdt(bool setting, sig_shmem_list_t *arg);

/*!
 * \brief		Se setting è false il programma elimina i semafori presenti 
 * 				nella lista dei semafori.
 *
 * \param[in]  	setting 	Se true il valore di arg viene valutato e 
 * 							inizializzato.
 * 							
 * \param[in]	arg			La head della lista dei semafori da rimuovere
 */
void sig_free_sem(bool setting, sig_sem_list_t *arg);

/*!
 * \brief		Se setting è false il vengono eliminate tutte le code 
 * 				di messaggi presenti nel programma.
 *
 * \param[in]  	setting 	Se true il valore di arg viene valutato e 
 * 							inizializzato.
 * 							
 * \param[in]	arg			La head della lista delle code da rimuovere
 */
void sig_free_queue(bool setting, sig_queue_list_t *arg);

/*!
 * \brief		Funzione per liberare la memoria prima della terminazione
 *
 * \param[in]   code  Codice di exit
 */
void sig_end(int code);

#endif
