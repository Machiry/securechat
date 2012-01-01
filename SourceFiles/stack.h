/* 
 * File:   stack.h
 * Author: machiry
 *
 * Created on December 30, 2011, 3:56 AM
 */

#ifndef STACK_H
#define	STACK_H

#ifdef	__cplusplus
extern "C" {
#endif
    //This is the structure of each element of the stack
    typedef struct stackElement{
        char *data;
    } *STK_ELEMENT;

    //This is the structure used to maintain stack
    typedef struct stack{
        //This points to the top of the stack
        unsigned long top;
        //This is the capacity of the stack
        unsigned long maxSize;
        //This is the place holder of various elements of the stack
        STK_ELEMENT *elements;
    } *STK_PTR;

    //This function will create a new stack of capacity size
    //returns: pointer to the newly created stack or null if the is insufficient space
    STK_PTR GetNewStack(unsigned long size);

    //This will create a new element that can be stored in the stack
    //returns: pointer to the newly created element or null if the is insufficient space
    STK_ELEMENT GetNewStackElement(char *data);

    //This function will push a new element with contents contained in data to the stack
    //return value is inaccordance with enum ErrorCode (refer dserror.h)
    int Push(STK_PTR stk,char *data);

    //This function removes a element present at the top of the stack
    //returns: pointer to the data pointed by the stack element or null if the stack is empty
    //NOTE: the data pointed by the return pointer is placed in the heap of the program
    //NOTE: free the returned pointer after using it to avoid space problems
    char* Pop(STK_PTR stk);

    //Same as above operation, but doesn't remove the element from the stack.
    char* Peep(STK_PTR stk);

     //This functions returns non zero if the provided stack is empty
    //else 0
    int IsStackEmpty(STK_PTR stk);

     //This functions returns non zero if the provided stack is full
    //else 0
    int IsStackFull(STK_PTR stk);

#ifdef	__cplusplus
}
#endif

#endif	/* STACK_H */

