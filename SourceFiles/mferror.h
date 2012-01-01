/* 
 * File:   GTError.h
 * Author: machiry
 *
 * Created on October 31, 2011, 6:34 PM
 */

#ifndef GTERROR_H
#define	GTERROR_H

#ifdef	__cplusplus
extern "C" {
#endif

//These are the possible error codes returned
//from the functions returning integer
enum MFError
{
    //this will be returned if the requested function is sucessfull
    SUCESS =0,
    //this will be returned if any mfiber functions will be called
    //without calling mfiber_init
    READY_QUEUE_NOT_INITIALIZED = -2,
    //this will be returned if there is no enough memeory available
    //to perform the requested function
    NOT_ENOUGH_MEMORY = -3,
    //this will be returned if the requested operation
    //cannot be performed with the required parameters
    INVALID_OPERATION = -1,
    //this will be returned if the provided
    //mfiber_t is invalid or not initialized
    INVALID_MFIBER_T = -4
};


#ifdef	__cplusplus
}
#endif

#endif	/* GTERROR_H */

