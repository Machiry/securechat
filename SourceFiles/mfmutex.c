#include "mfmutex.h"
#include "CommonCHeaders.h"
#include "mferror.h"
#include "DataStructures.h"
static MutexQueuePtr queue;
extern TCB_QUEUE readyQueue;
static sigset_t sigProfMask;
typedef struct WaitingNodeList WaitingNodeList;


//Mutex Queue Management
int IsTCBPresentinMutexQueue(MutexQueuePtr tcbQueue,TCB_PTR targetTCB)
{
    if(tcbQueue != null)
    {
        WaitingNodeList *waitingNode = tcbQueue->nodeList;
        while((waitingNode!=null) && (waitingNode->node != targetTCB))
        {
            waitingNode = waitingNode->next;
        }
        return waitingNode != null;
    }
    return 0;
}

MutexQueuePtr GetRequiredMutexQueue(mfiber_mutex_t *mutex)
{
    if(queue == null|| mutex == null)
    {
        return null;
    }
    MutexQueuePtr queuePtr = queue;
    while((queuePtr!= null)  && (queuePtr->mutex->lockNumber != mutex->lockNumber))
    {
        queuePtr = queuePtr->next;
    }
    return queuePtr;
}

void AddMutexToTheQueue(MutexQueuePtr newMutex)
{
    if(newMutex != null)
    {
        newMutex->next = queue;
        queue = newMutex;
    }
}

mfiber_mutex_t* GetNextMutexValue()
{
    static long currentMutexValue = 0;
    mfiber_mutex_t *newMutexValue = (mfiber_mutex_t*)malloc(sizeof(mfiber_mutex_t));
    newMutexValue->lockNumber = ++currentMutexValue;
    return newMutexValue;
}
int IsMutexPresentInQueue(mfiber_mutex_t *mutex)
{
    if(queue == null || mutex==null)
    {
        return 0;
    }
    MutexQueuePtr queuePtr = queue;
    while((queuePtr!= null)  && (queuePtr->mutex->lockNumber != mutex->lockNumber))
    {
        queuePtr = queuePtr->next;
    }
    return queuePtr != null;
}



//Memory Allocation

WaitingNodeList* GetNewWaitingListNode()
{
    WaitingNodeList *nodePtr = (WaitingNodeList *)malloc(sizeof(WaitingNodeList));
    if(nodePtr)
    {
        nodePtr->next = null;
        nodePtr->node = null;
    }
    return nodePtr;
}

MutexQueuePtr GetNewMutexNode()
{
    MutexQueuePtr queuePtr = (MutexQueuePtr)malloc(sizeof(struct MutexQueue));
    if(queuePtr)
    {
        queuePtr->mutex = (mfiber_mutex_t*)malloc(sizeof(mfiber_mutex_t));
        queuePtr->next = null;
        queuePtr->nodeList = null;
    }
    return queuePtr;
}


//Main Thread Functionality
int  mfiber_mutex_init(mfiber_mutex_t *mutex)
{
    sigemptyset(&sigProfMask);
    sigaddset(&sigProfMask, SIGPROF);
    sigprocmask(SIG_BLOCK, &sigProfMask, NULL);
    if(mutex != null)
    {
        if(!IsMutexPresentInQueue(mutex))
        {
            MutexQueuePtr newMutexQueue = GetNewMutexNode();
            newMutexQueue->mutex = GetNextMutexValue();
            mutex->lockNumber = newMutexQueue->mutex->lockNumber;
            AddMutexToTheQueue(newMutexQueue);
            sigprocmask(SIG_UNBLOCK, &sigProfMask, NULL);
            return 0;
        }
    }
    sigprocmask(SIG_UNBLOCK, &sigProfMask, NULL);
    return INVALID_OPERATION;
}

int  mfiber_mutex_lock(mfiber_mutex_t *mutex)
{
    sigprocmask(SIG_BLOCK, &sigProfMask, NULL);
    MutexQueuePtr mutexQueue = GetRequiredMutexQueue(mutex);
    TCB_PTR currentTCB = GetCurrentTCB(readyQueue);
    sigprocmask(SIG_UNBLOCK, &sigProfMask, NULL);
    if(!mutexQueue)
    {        
        return INVALID_OPERATION;
    }

    if((currentTCB != null))
    {
         while((mutexQueue->nodeList));
        sigprocmask(SIG_BLOCK, &sigProfMask, NULL);
        WaitingNodeList *waitingTCBNode = GetNewWaitingListNode();
        waitingTCBNode->node = currentTCB;
        waitingTCBNode->next = mutexQueue->nodeList;
        mutexQueue->nodeList = waitingTCBNode;
        sigprocmask(SIG_UNBLOCK, &sigProfMask, NULL);
        return 0;
    }
    sigprocmask(SIG_UNBLOCK, &sigProfMask, NULL);
    return INVALID_OPERATION;
}

int  mfiber_mutex_unlock(mfiber_mutex_t *mutex)
{
    sigprocmask(SIG_BLOCK, &sigProfMask, NULL);
    MutexQueuePtr mutexQueue = GetRequiredMutexQueue(mutex);
    if(!mutexQueue)
    {
        sigprocmask(SIG_UNBLOCK, &sigProfMask, NULL);
        return INVALID_OPERATION;
    }
    if(mutexQueue->nodeList == null)
    {
        sigprocmask(SIG_UNBLOCK, &sigProfMask, NULL);
        return INVALID_OPERATION;
    }
    TCB_PTR currentTCB = GetCurrentTCB(readyQueue);
    
    WaitingNodeList *waitingNode = mutexQueue->nodeList;
    if(currentTCB == waitingNode->node)
    {
        mutexQueue->nodeList = null;
        free(waitingNode);
        sigprocmask(SIG_UNBLOCK, &sigProfMask, NULL);
        return 0;
    }
    sigprocmask(SIG_UNBLOCK, &sigProfMask, NULL);
    return INVALID_OPERATION;
}




