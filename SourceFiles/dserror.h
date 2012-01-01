/* 
 * File:   DSLibErrorCodes.h
 * Author: machiry
 *
 * Created on December 29, 2011, 9:14 PM
 */

#ifndef DSLIBERRORCODES_H
#define	DSLIBERRORCODES_H

#ifdef	__cplusplus
extern "C" {
#endif

    //These are the error codes returned by various functions operating on several datastructures
    typedef enum ErrorCode {
        SUCESS = 0,
        UNSUCCESSFUL = 1,
        INSUFFICIENTSPACE = -1,
        OPERATION_NOT_SUPPORTED = -2,
        BAD_LIST = -3,

        //Below 2 are only returned by functions operating on the stack
        STACK_FULL = -4,
        STACK_EMPTY = -5,

        //Below 2 are only returned by functions operating on the queue
        QUEUE_FULL = -6,
        QUEUE_EMPTY = -7
    } ErrorCode;


#ifdef	__cplusplus
}
#endif

#endif	/* DSLIBERRORCODES_H */

