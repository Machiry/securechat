/* 
 * File:   CommonHeaders.h
 * Author: machiry
 *
 * Created on September 28, 2011, 9:33 PM
 */

#ifndef COMMONHEADERS_H
#define	COMMONHEADERS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <ucontext.h>
#include "DataStructures.h"
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "mfiber.h"
    
void scheduler(int  sigNum);
ucontext_t getFiberCompleteNotfierContext();


#ifdef	__cplusplus
}
#endif

#endif	/* COMMONHEADERS_H */

