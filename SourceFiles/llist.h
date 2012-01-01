/* 
 * File:   LinkedList.h
 * Author: machiry
 *
 * Created on December 18, 2011, 6:38 PM
 */

#ifndef LINKEDLIST_H
#define	LINKEDLIST_H

//This is the structure used by all the nodes in the singly linked list
typedef struct SLLNode {
    int data;
    struct SLLNode *next;
} *SLLNodePtr;

//This is the structure of all the nodes in the doubly linked list
typedef struct DLLNode {
    int data;
    struct DLLNode *next;
    struct DLLNode *prev;
} *DLLNodePtr;

//This is the structure used to store the singly linked list
typedef struct SLList {
    //This points to the head of the list
    SLLNodePtr first;
    //This points to the tail of the list
    SLLNodePtr last;
    //this stores the length(no of nodes) of the list
    long length;
} *SLListPtr;

//This is the structure used to store the doubly linked list
typedef struct DLList {
    //This points to the head of the list
    DLLNodePtr first;
    //This points to the tail of the list
    DLLNodePtr last;
    //this stores the length(no of nodes) of the list
    long length;
} *DLListPtr;


//Functions Supporting Singly Linked List

//This function creates a new Singly linked list
//returns: pointer to the newly created singly linked list or null if there is no enough space
SLListPtr GetNewSLList();

//This function creates a new node of Singly linked list
//returns: pointer to the newly created singly linked list node or null if there is no enough space
SLLNodePtr GetNewSLNode();


//This function checks the sanity of the linked list
//Things this checks:
//1.list->first and list->last pointed to teh first and last nodes
//2.length is the correct length of the linked list
//3.list->last correctly points to the last node of the list
//returns: non zero if list is good else 0
int IsSLListGood(SLListPtr list);


//All the below 5 functions return enum ErrorCode (refer dserror.h)

//This function inserts the a new node with the provided data (nodeData) to the head of the provided
//singly linked list
int SL_InsertAtFront(SLListPtr list, int nodeData);

//This function inserts the a new node with the provided data (nodeData) to the tail of the provided
//singly linked list
int SL_InsertAtEnd(SLListPtr list, int nodeData);

//This function removes a node from the head of the provided
//singly linked list
//if data is non null this will store the data provided in the deleted node to the location
//pointed by data
int SL_RemoveAtFront(SLListPtr list, int *data);

//This function removes a node from the end of the provided
//singly linked list
//if data is non null this will store the data provided in the deleted node to the location
//pointed by data
int SL_RemoveAtEnd(SLListPtr list, int *data);

//This function sorts the provided list using Merge Sort in non-decreasing order
int SL_Sort(SLListPtr list);

//This function reverses the provided list
//returs the head of the reversed list or null of the provided list is null
SLListPtr SL_ReverseList(SLListPtr list);



//Functions Supporting Doubly Linked List

//This function creates a new doubly linked list
//returns: pointer to the newly created doubly linked list or null if there is no enough space
DLListPtr GetNewDLList();

//This function creates a new node of doubly linked list
//returns: pointer to the newly created doubly linked list node or null if there is no enough space
DLLNodePtr GetNewDLNode();

//This function checks the sanity of the doubly linked list
//Things this checks:
//1.list->first and list->last pointed to teh first and last nodes
//2.length is the correct length of the linked list
//3.list->last correctly points to the last node of the list
//4.next and prev of each node point to the correct nodes
//returns: non zero if list is good else 0
int IsDLListGood(DLListPtr list);

//This function inserts the a new node with the provided data (nodeData) to the head of the provided
//doubly linked list
int DL_InsertAtFront(DLListPtr list, int nodeData);

//This function inserts the a new node with the provided data (nodeData) to the tail of the provided
//doubly linked list
int DL_InsertAtEnd(DLListPtr list, int nodeData);

//This function removes a node from the head of the provided
//doubly linked list
//if data is non null this will store the data provided in the deleted node to the location
//pointed by data
int DL_RemoveAtFront(DLListPtr list, int *data);

//This function removes a node from the end of the provided
//doubly linked list
//if data is non null this will store the data provided in the deleted node to the location
//pointed by data
int DL_RemoveAtEnd(DLListPtr list, int *data);

//This function reverses the provided list
//returs the head of the reversed list or null of the provided list is null
DLListPtr DL_ReverseList(DLListPtr list);

#endif	/* LINKEDLIST_H */

