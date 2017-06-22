/**
 * @file io_lib.h
 * Funzioni utili per l'I/O che utilizzano syscall
 */

#ifndef IO_LIB_H
#define IO_LIB_H
#include "std_lib.h"
#include "shm_lib.h"

#define BUF_SIZE 128


/**
 * @brief      Funzione che simula il comportamento della printf usando
 *             la syscall write
 *
 * @param[in]  fd         File descriptor su cui scrivere
 * @param      str        Stringa di formattazione
 * @param[in]  ...        Argomenti da sostituire
 */
void sys_print(int fd, char *str, ...);

/**
 * @brief      Funzione che simula il comportamento della perror
 *
 * @param      str   La stringa da stampare come errore
 */
void sys_err(char *str);

#endif