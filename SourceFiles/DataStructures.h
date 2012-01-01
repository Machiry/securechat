/* 
 * File:   DataStrcutures.h
 * Author: machiry
 *
 * Created on September 28, 2011, 12:26 AM
 */
#include "CommonCHeaders.h"

#define STACKSIZE 1024*8

#ifndef DATASTRCUTURES_H
#define	DATASTRCUTURES_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef long mfiber_t;
#define InValidgtthread_t -999;


typedef struct Node
{
    mfiber_t tid;
    ucontext_t context;
    struct Node *next;
    struct WaitingNodeList *blockedThreads;
    //This is required becasue we don't
    //allocate stack space to this node
    //we should not free stack space if it is such a node
    int hasNoStackSpaceAllocated;
    int hasCompleted;
    int isBlocked;

} TCB,*TCB_PTR;

typedef struct WaitingNodeList
{
    TCB_PTR node;
    struct WaitingNodeList *next;
} *BlockedThreadList;

typedef struct TCB_Queue
{
    TCB_PTR head;
    TCB_PTR head_parent;
    long count;
} *TCB_QUEUE;

typedef struct DeadNode
{
    mfiber_t tid;
    void **returnStatus;
    struct DeadNode *next;
} *DEADNODE_PTR;

typedef struct Completed_TCB_Queue
{
    struct DeadNode *node;
    long count;
} *COMPLETED_QUEUE;

TCB_PTR GetCurrentTCB(TCB_QUEUE queue);
TCB_PTR GetNextTCB(TCB_QUEUE queue);
int MoveForward(TCB_QUEUE queue);
int GetTCBCount(TCB_QUEUE queue);
int GetNextThreadId();
void FreeNode(TCB_PTR node);
TCB_PTR GetNewTCBWithOutStack();
TCB_PTR GetNewTCB();
TCB_QUEUE GetTCBQueue();
int DeQueue(TCB_QUEUE queue);
int EnQueue(TCB_QUEUE queue,TCB_PTR node);
int AddToTCBBlockingList(TCB_PTR targetNode,TCB_PTR nodeToBeAdded);
BlockedThreadList GetNewBlockedThreadNode();
TCB_PTR GetTCBByThreadID(mfiber_t threadid,TCB_QUEUE queue);
DEADNODE_PTR GetDeadNodeByThreadID(COMPLETED_QUEUE queue,mfiber_t tid,int deleteNodeAfterFetching);
int EnqueueToCompletedNodeQueue(COMPLETED_QUEUE queue,DEADNODE_PTR node);
COMPLETED_QUEUE GetCompletedQueue();
DEADNODE_PTR GetCompletedNode();

#ifdef	__cplusplus
}
#endif

#endif	/* DATASTRCUTURES_H */

