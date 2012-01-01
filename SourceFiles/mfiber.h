/* 
 * File:   GtThread.h
 * Author: machiry
 *
 * Created on September 29, 2011, 2:30 AM
 */

#ifndef GTTHREAD_H
#define	GTTHREAD_H
#include "mferror.h"
#ifdef	__cplusplus
extern "C" {
#endif

typedef long mfiber_t;
#define InValidmfiber_t -999;

//this function will initialize the mfiber library
//this function also initializes ready queue which will be used to schedule
//this function has to be called before we use any other function in the library
void mfiber_init(long period);

//this function will create a new fiber which is used to execute the
//function pointed by start_routine with arg as its parameter
//if thread is not null then the id of the created fiber will be stored in it
//return value will be in accordance with MFError (refer mferror.h for more details)
int mfiber_create(mfiber_t *thread,void *(*start_routine)(void *),void *arg);

//this function will be blocked till the thread referred by thread is complete
//if status is not null then the return value of thread will be stored here
//return value will be in accordance with MFError (refer mferror.h for more details)
int  mfiber_join(mfiber_t thread, void **status);

//returns non zero if threads pointed by t1 and t2 are same
//else 0
int  mfiber_equal(mfiber_t t1, mfiber_t t2);

//this function will return the id of the thread
//on which the called function is executed
//if mfiber is not initialized then this will return INVALID_MFIBER_T
mfiber_t mfiber_self(void);

//this function will terminate the thread provided forcefully
//return value will be in accordance with MFError (refer mferror.h for more details)
int  mfiber_cancel(mfiber_t thread);

//this function will return the time quantun assigned to it forcefully
void mfiber_yield(void);

//this function will forcefully close the called thread
//the return value of this thread will be stored as retval
void mfiber_exit(void *retval);


#ifdef	__cplusplus
}
#endif

#endif	/* GTTHREAD_H */

