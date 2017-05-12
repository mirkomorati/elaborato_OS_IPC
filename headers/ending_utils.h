#ifndef ENDING_UTILS_H
#define ENDING_UTILS_H
#include <stdbool.h>
/*!
 * struct that permits to free a shared memory area.
 */
typedef struct {
    int shmid;
    void *shmaddr;
} sig_utils_t;

/*!
 * list that contains the the object of the shared memory
 */
typedef struct sig_shmem_list{
	sig_utils_t obj;

	struct sig_shmem_list *next;
} sig_shmem_list_t;

/*!
 * \brief      	function that free all the shared memory of the program
 *
 * \param[in]  	sig   The signal
 * \param      	arg   The arguments array
 */
void sig_handler(int sig, void *arg);

/*!
 * \brief      	assigns the handler function to the right signal
 * 
 * \param		list	The head of the list of shared object to remove
 */
void sig_init(sig_shmem_list_t *list);

/*!
 * \brief      	add to the list of shared memory object to free the object passed
 *
 * \param[in] 	n		number of elements
 * \param[in]  	args 	list of objects
 */
void sig_add(int n, ...);

/*!
 * \brief		if setting is false the program free the memory
 * 				end exit with exit code 0
 *
 * \param[in]  	setting 	if true the program will not exit.
 * \param		arg			The head of the list of shared object to remove	
 */
void sig_free_memory(bool setting, sig_shmem_list_t *arg);

/*!
 * \brief		function that frees the memory before exit		
 *
 * \param[in]  code  The exit code
 */
void sig_end(int code);

#endif