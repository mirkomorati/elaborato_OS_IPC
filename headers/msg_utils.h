/*
 * file che conterrà tutte le funzioni ed i dati di
 * utilità per i messaggi scambiati tra padre e figli 
 * o viceversa attraverso code di messaggi e pipe
 */

#ifndef MSG_UTILS_H
#define MSG_UTILS_H
#include <stdbool.h>

typedef enum{
	MULTIPLY,
	SUM
}role_t;

typedef struct{
	long type;
	pid_t sender_pid;
	bool success;
	role_t role;
}msg_t;

#endif