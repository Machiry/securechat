/* 
 * File:   GtMutex.h
 * Author: machiry
 *
 * Created on September 29, 2011, 10:40 PM
 */

#ifndef GTMUTEX_H
#define	GTMUTEX_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <ucontext.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
    
typedef struct GtMutex_Node
{
   long lockNumber;
} mfiber_mutex_t;

typedef struct MutexQueue
{
    mfiber_mutex_t *mutex;
    struct MutexQueue *next;
    struct WaitingNodeList *nodeList;
} *MutexQueuePtr;

//this function will initialize the mutex functionality of mfiber
//this function also stores the mutex value in the provided mutex variable
//please make sure that you call mfiber_init before calling this function
int  mfiber_mutex_init(mfiber_mutex_t *mutex);

//this function will spinlock the current thread acheives the normal lock functionality
//return value will be in accordance with the MFError (refer mferror.h)
int  mfiber_mutex_lock(mfiber_mutex_t *mutex);

//this function will unblock the current thread on the mutex variable pointed by mutex
//if the called thread doesn't own the mutex then this call will be unsucessfull
//return value will be in accordance with the MFError (refer mferror.h)
int  mfiber_mutex_unlock(mfiber_mutex_t *mutex);


#ifdef	__cplusplus
}
#endif

#endif	/* GTMUTEX_H */

