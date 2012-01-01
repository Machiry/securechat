#include "CommonCHeaders.h"
#include "DataStructures.h"
#include "mferror.h"

//This will remove the TCB which is pointed by head of the given queue
int DeQueue(TCB_QUEUE queue)
{
    int failure = 0;
    TCB_PTR next_node,prev_node,head_node;
    if(queue == null)
    {
        failure = INVALID_OPERATION;
    }
    else
    {
        head_node = queue->head;
        prev_node = queue->head_parent;
        if(head_node != null)
        {

            next_node = head_node->next;
            if(queue->count == 1)
            {
                //There is only one node in the queue
                //clear the pointers
                queue->head = queue->head_parent = null;
            }
            else
            {
                //More Than one node in queue
                //Make the parent node point to next_node
                queue->head = next_node;
                prev_node->next = queue->head;
            }
            FreeNode(head_node);
            queue->count--;
        }
        else
        {
            //Queue Is Empty
            failure = INVALID_OPERATION;
        }
    }
    return failure;
}

//This will add teh given TCB to the last of the queue
int EnQueue(TCB_QUEUE queue,TCB_PTR node)
{
    int failure = 0;
    if(queue ==null || node== null)
    {
        failure = INVALID_OPERATION;
    }
    else
    {
        if(queue->head == null)
        {
            //first node
            //make next of it point to itself as we are implementing circular list
            node->next = node;
            queue->head_parent = node;
            queue->head = node;
        }
        else
        {
            //Add the node before head so that it will be at the end of the queue
            node->next= queue->head;
            queue->head_parent->next = node;
            queue->head_parent = node;
        }
        queue->count++;
    }

    return failure;
}

//Enqueue To The Dead Node List
int EnqueueToCompletedNodeQueue(COMPLETED_QUEUE queue,DEADNODE_PTR node)
{
    int failure = INVALID_OPERATION;
    if(queue != null && node != null)
    {
        node->next=queue->node;
        queue->node=node;
        failure=0;
    }
    return failure;
}

DEADNODE_PTR GetDeadNodeByThreadID(COMPLETED_QUEUE queue,mfiber_t tid,int deleteNodeAfterFetching)
{
    if(queue != null && tid != INVALID_MFIBER_T)
    {
        DEADNODE_PTR node = queue->node;
        DEADNODE_PTR prevNode = null;
        while((node != null) && (node->tid != tid))
        {
            prevNode = node;
            node = node->next;
        }
        if(deleteNodeAfterFetching && node!=null)
        {
            //we will delete this node
            if(prevNode == null)
            {
                queue->node = node->next;
            }
            else
            {
                prevNode->next = node->next;
            }
        }
        return node;
    }
    return null;
}
TCB_PTR GetCurrentTCB(TCB_QUEUE queue)
{
    if(queue == null)
    {
        return null;
    }
    return queue->head;
}

TCB_PTR GetNextTCB(TCB_QUEUE queue)
{
    if(queue != null && queue->head != null)
    {
        return queue->head->next;
    }
    return null;
}

int MoveForward(TCB_QUEUE queue)
{
    int failure = INVALID_OPERATION;
    if(queue != null)
    {
        TCB_PTR current_head = queue->head;
        if(current_head != null)
        {
            queue->head_parent = current_head;
            queue->head = current_head->next;
            failure = 0;
        }
    }
    return failure;
}

int GetTCBCount(TCB_QUEUE queue)
{
    if(queue == null)
    {
        return 0;
    }
    return queue->count;
}

TCB_PTR GetTCBByThreadID(mfiber_t threadid,TCB_QUEUE queue)
{
    TCB_PTR firstNode = GetCurrentTCB(queue);
    if(firstNode!=null && firstNode->tid == threadid)
    {
        return firstNode;
    }
    TCB_PTR iteratorNode = firstNode!=null? firstNode->next:null;
    while(firstNode!=iteratorNode)
    {
        if(iteratorNode->tid == threadid)
        {
            return iteratorNode;
        }
        iteratorNode = iteratorNode->next;
    }
    return null;
}

int AddToTCBBlockingList(TCB_PTR targetNode,TCB_PTR nodeToBeAdded)
{
    BlockedThreadList newNode = GetNewBlockedThreadNode();
    if(newNode != null)
    {
        newNode->node = nodeToBeAdded;
        newNode->next = targetNode->blockedThreads;
        targetNode->blockedThreads = newNode;
        nodeToBeAdded->isBlocked = 1;
        return 0;
    }
    return INVALID_OPERATION;
}

//HELPER FUNCTIONS

int GetNextThreadId()
{
    static long currentThreadID=0;
    return ++currentThreadID;
}


//MEMORY MANAGEMENT FUNCTIONS

void FreeNode(TCB_PTR node)
{
    //if we had alloceted stack space to this node
    //first free it before freeing the node
    if(node != null)
    {
        BlockedThreadList blockedThreadNode = node->blockedThreads;
        if(!(node->hasNoStackSpaceAllocated))
        {
            free(node->context.uc_stack.ss_sp);
        }
        if(blockedThreadNode != null)
        {
            BlockedThreadList nextNode = blockedThreadNode->next;
            free(blockedThreadNode);
            blockedThreadNode = nextNode;
        }
        free(node);
    }
}

TCB_PTR GetNewTCBWithOutStack()
{
    TCB_PTR newTCB = (TCB_PTR)malloc(sizeof(TCB));
    if(newTCB == null)
    {
        //Memory Exhaustion
        return null;
    }
    newTCB->context.uc_link = 0;
    newTCB->context.uc_stack.ss_flags = 0;
    newTCB->next = null;
    newTCB->blockedThreads = null;
    newTCB->isBlocked = 0;
    newTCB->tid = GetNextThreadId();
    newTCB->hasNoStackSpaceAllocated = 1;
    newTCB->hasCompleted = 0;
    return newTCB;
}

BlockedThreadList GetNewBlockedThreadNode()
{
    BlockedThreadList newNode = (BlockedThreadList)malloc(sizeof(struct WaitingNodeList));
    if(newNode != null)
    {
        newNode->node = null;
        newNode->next = null;
    }
    return newNode;
}

TCB_PTR GetNewTCB()
{
    TCB_PTR newTCB = (TCB_PTR)malloc(sizeof(TCB));
    if(newTCB == null)
    {
        //Memory Exhaustion
        return null;
    }
    newTCB->next = null;
    newTCB->context.uc_stack.ss_sp = malloc(STACKSIZE);
    if(newTCB->context.uc_stack.ss_sp == null)
    {
        //Memory Exhaustion
        free(newTCB);
        return null;
    }
    newTCB->context.uc_link = 0;
    newTCB->hasNoStackSpaceAllocated = 0;
    newTCB->hasCompleted = 0;
    newTCB->blockedThreads = null;
    newTCB->context.uc_stack.ss_size = STACKSIZE;
    newTCB->context.uc_stack.ss_flags = 0;
    newTCB->isBlocked = 0;
    //assign ThreadID only if everthing goes fine
    newTCB->tid = GetNextThreadId();

    return newTCB;
}

TCB_QUEUE GetTCBQueue()
{
    TCB_QUEUE newQueue = (TCB_QUEUE)malloc(sizeof(struct TCB_Queue));
    if(newQueue == null)
    {
        //Memory Exhaustion
        return null;
    }
    newQueue->count = 0;
    newQueue->head = newQueue->head_parent = null;
    return newQueue;
}

DEADNODE_PTR GetCompletedNode()
{
    DEADNODE_PTR newNode = (DEADNODE_PTR)malloc(sizeof(struct DeadNode));
    if(newNode == null)
    {
        //Memory Exhaustion
        return null;
    }
    newNode->returnStatus = (void**)malloc(sizeof(void*));
    if(newNode->returnStatus == null)
    {
        //Memory Exhaustion
        free(newNode);
        return null;
    }
    newNode->tid=INVALID_MFIBER_T;
    *(newNode->returnStatus) = null;
    newNode->next = null;
    return newNode;
}

COMPLETED_QUEUE GetCompletedQueue()
{
    COMPLETED_QUEUE newQueue = (COMPLETED_QUEUE)malloc(sizeof(struct Completed_TCB_Queue));
    if(newQueue == null)
    {
        //Memory Exhaustion
        return null;
    }
    newQueue->count = 0;
    newQueue->node = null;
    return newQueue;
}
