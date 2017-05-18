/*! 
 * \file 		msg_lib.h
 * Raccolta di funzioni e strutture dati per lo scambio di messaggi
 * 
 * Conterrà tutte le funzioni ed i dati di utilità per i messaggi scambiati tra padre e figli attraverso code di messaggi e pipe
 */

#ifndef MSG_LIB_H
#define MSG_LIB_H
#include <stdbool.h>
#include <stdint.h>

/*! 
 * \brief 		Rappresenta il ruolo di un figlio
 * 
 * Il ruolo è variabile ed è parte di un comando che può essere inviato da un
 * padre ad un figlio
 * Può essere:
 * 	- MULTIPLY: il figlio deve effettuare una moltiplicazione
 * 	- SUM: 		il figlio deve effettuare una somma
 */
typedef enum{
	MULTIPLY, 
	SUM,
	END
}role_t;

/*! 
 * \brief 		Comando inviato dal padre ai figli
 * 
 * Rappresenta un comando inviato dal padre ad uno dei suoi figli
 */
typedef struct{
	role_t role; //!< Ruolo che il figlio deve svolgere
	union data_u {
		struct point_u {
			int i;	
			int j;
		} c;
		int row;
	} data; 	//!< \brief Dati del comando 
	     		//! In caso di somma va tenuto conto del valore di row,
	     		//! altrimenti va tenuto conto del valore di c
}cmd_t;

/*! 
 * \brief Messaggio dei figli al padre 
 * 
 * Rappresenta un messaggio per la coda di messaggi utilizzata dai 
 * figli per comunicare con il padre
 */
typedef struct{
	long type;		//!< Tipo del messaggio
	bool success;	//!< True se il comando è stato eseguito correttamente
	cmd_t cmd;		//!< Comando che doveva essere eseguito
	pid_t pid;		//!< Il pid del processo che ha eseguito il comando.
}msg_t;

/*!
 * \brief 	Crea un dizionario che mette in relazione il pid di un figlio 
 * 			con il descrittore della pipe su cui scrivere
 */
typedef struct{
	pid_t pid;
	int pipe_fd;
}pid_to_pipe_t;

/*!
 * \brief      	Mette il messaggio msg nella coda di messaggi che devono
 * 				essere letti dal padre L'operazione è gestita da semafori
 *
 * \param[in]  	msg   	Il messaggio da inviare
 * \param[in]	id		ID della coda di messaggi
 *
 * \return     	0 in caso di successo, -1 altrimenti
 */
int send_msg(const msg_t * restrict msg, const int id, const int sem_id);


/*!
 * \brief      	Preleva un messaggio dalla coda di messaggi diretti al padre
 * 				L'operazione è gestita da semafori
 *
 * \param[out]  msg   	Puntatore all'oggetto in cui salvare il messaggio
 * \param[in]	id 		ID della coda di messaggi
 *
 * \return     	0 in caso di successo, -1 altrimenti
 */
int rcv_msg(msg_t * restrict msg, const int id);

/*!
 * \brief      	Scrive un comando nella pipe che collega il padre con i figli
 *				L'operazione è gestita da semafori
 *				
 * \param[in]  	cmd		Il comando da inviare
 * \param[in] 	fd    	File descriptor della pipe
 *
 * \return    	0 in caso di successo, -1 altrimenti
 */
int send_cmd(const cmd_t * restrict cmd, const int fd);

/*!
 * \brief      	Legge un comando scritto dal padre nella pipe che lo collega
 * 				ai figli
 * 
 * \param[out]  cmd		Puntatore all'oggetto in cui salvare il comando
 * \param[in]  	fd		File descriptor della pipe
 *
 * \return    	0 in caso di successo, -1 altrimenti
 */
int rcv_cmd(cmd_t * restrict cmd, const int fd);

/*!
 * \brief      compara due comandi e ritorna true se sono uguali false altrimenti
 *
 * \param      a   	primo comando
 * \param      b	secondo comando
 *
 * \return     true se i comandi sono uguali
 */
bool cmd_equals (const cmd_t *a,const cmd_t *b);
#endif
