/* 
 * File:   queue.h
 * Author: machiry
 *
 * Created on December 30, 2011, 3:56 AM
 */

#ifndef QUEUE_H
#define	QUEUE_H

#ifdef	__cplusplus
extern "C" {
#endif
    //This is the structure of each element of the queue
    typedef struct queueElement {
        char *data;
    } *QUE_ELEMENT;

    //This is the structure used to maintain queue
    typedef struct queue {
        //This points to the index where bottom element of the queue is present
        //This is also the index from which the elements will be fetched from the queue
        unsigned long bottom;

        //This points to the index where top element of the queue if present
        unsigned long top;

        //This is the capacity of the queue
        unsigned long maxSize;

        //This is the array that contains place holders for various elements of the queue
        QUE_ELEMENT *elements;

        //This conatins the tatal number of elements present in the queue
        unsigned long noOfElements;
    } *QUE_PTR;

    //This function will create a new queue of capacity size
    //returns: pointer to the newly created queue or null if the is insufficient space
    QUE_PTR GetNewQueue(unsigned long size);

    //This will create a new element that can be stored in the queue
    //returns: pointer to the newly created element or null if the is insufficient space
    QUE_ELEMENT GetNewQueueElement(char *data);

    //This function will insert a new element with contents contained in data to the queue
    //return value is inaccordance with enum ErrorCode (refer dserror.h)
    int Enqueue(QUE_PTR que, char *data);

    //This function removes a element present at the bottom of the queue
    //returns: pointer to the data pointed by the queue element or null if the queue is empty
    //NOTE: the data pointed by the return pointer is placed in the heap of the program
    //NOTE: free the returned pointer after using it to avoid space problems
    char* Dequeue(QUE_PTR que);

    //Same as above operation, but doesn't remove the element from the queue.
    char* Peepq(QUE_PTR que);

    //This functions returns non zero if the provided queue is empty
    //else 0
    int IsQueueEmpty(QUE_PTR que);

    //This functions returns non zero if the provided queue is full
    //else 0
    int IsQueueFull(QUE_PTR que);



#ifdef	__cplusplus
}
#endif

#endif	/* QUEUE_H */

