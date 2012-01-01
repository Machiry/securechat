#include "CommonCHeaders.h"
#include "DataStructures.h"

extern TCB_QUEUE readyQueue;
ucontext_t notifierContext;
extern struct itimerval timeQuantum;
extern long timeIntervalInMSec;
TCB_PTR GetCurrentTCB_PTR_Atomically();
extern sigset_t sigProcMask;
int ignoreSignal = 0;

void scheduler(int  sigNum)
{
    //Check if only element on queue is main()
    if(!ignoreSignal){
      sigprocmask(SIG_BLOCK, &sigProcMask, NULL);
    if(GetTCBCount(readyQueue) == 1)
    {
        //only one node nothing to do
        //TODO: if it has completed then Exit
        TCB_PTR currentNode = GetCurrentTCB(readyQueue);
        if(currentNode->hasCompleted)
        {
            DeQueue(readyQueue);
            sigprocmask(SIG_UNBLOCK, &sigProcMask, NULL);
            exit(0);
        }
        //END CONDITION OF USER THREADS
    }
    else if(GetTCBCount(readyQueue) > 0)
    {
        TCB_PTR currentNode= GetCurrentTCB(readyQueue);
        int currentNodeCompleted = 0;
        if(currentNode != null)
        {
            TCB_PTR nextNode;
            if(currentNode->hasCompleted)
            {
                //Remove the Node if it has completed Execution
                currentNodeCompleted = 1;
                DeQueue(readyQueue);
            }
            else
            {
                MoveForward(readyQueue);
            }
            nextNode = GetCurrentTCB(readyQueue);

            while((nextNode!=null) && (nextNode->isBlocked || nextNode->hasCompleted))
            {
                if(nextNode->hasCompleted)
                {
                    DeQueue(readyQueue);
                }
                else
                {
                    //This means the thread is not completed but blocked
                    //we need to move forward
                    MoveForward(readyQueue);
                }
                nextNode = GetCurrentTCB(readyQueue);
            }
            if(nextNode == null)
            {
                //All Threads Completed
                sigprocmask(SIG_UNBLOCK, &sigProcMask, NULL);
                exit(0);
            }
            if(nextNode != currentNode)
            {
                //if After removing all the completed TCBs
                //of there are more then one node remaining
                //then Context Switch
                //Swap the Context between currentNode and nextNode


                //DEBUGGING

                fflush(stdout);
                //ENDDEBUGGING'
                timeQuantum.it_value.tv_usec = timeIntervalInMSec;
                sigprocmask(SIG_UNBLOCK, &sigProcMask, NULL);
                if(currentNodeCompleted)
                {
                    //printf("Seeting Context as the current thread completed  %ld\n",nextNode->tid);
                    setcontext(&(nextNode->context));
                }
                else
                {
                    //printf("Swaping %ld with %ld\n",currentNode->tid,nextNode->tid);
                    swapcontext(&(currentNode->context),&(nextNode->context));
                }
            }

        }
    }
    sigprocmask(SIG_UNBLOCK, &sigProcMask, NULL);
    }
}

void fiberCompletedNotifier()
{
    //Set the hasCompleted Node of the current node as 1
    TCB_PTR currentNode = GetCurrentTCB_PTR_Atomically();
    BlockedThreadList blockedNode = currentNode->blockedThreads;

    //we will first unblock all the other threads waiting for this
    //thread to complete
    while(blockedNode != null)
    {
        blockedNode->node->isBlocked = 0;
        blockedNode = blockedNode->next;
    }

    //DEBUGGING
    printf("Thread Completed:%ld\n",currentNode->tid);
    //ENDDEBUGGING
    

    //then we mark this thread as complete
    currentNode->hasCompleted = 1;

    raise(SIGPROF);

}

ucontext_t getFiberCompleteNotfierContext()
{
    static int hasContextCreated;
    
    if(!hasContextCreated)
    {
        getcontext(&notifierContext);
        notifierContext.uc_link = 0;
        notifierContext.uc_stack.ss_sp = malloc(STACKSIZE);
        notifierContext.uc_stack.ss_size = STACKSIZE;
        notifierContext.uc_stack.ss_flags= 0;
        makecontext( &notifierContext, (void (*) (void))&fiberCompletedNotifier, 0);
        hasContextCreated = 1;
    }
    return notifierContext;
}
