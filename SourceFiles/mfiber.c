#include "CommonHeaders.h"
#include "DataStructures.h"

TCB_QUEUE readyQueue = null;
COMPLETED_QUEUE deadQueue = null;
struct sigaction schedulerHandle;
extern ucontext_t notifierContext;
struct itimerval timeQuantum;
long timeIntervalInMSec;
sigset_t sigProcMask;
void *wrapperFunction(void *(*start_routine)(void *), void *arg);
TCB_PTR GetCurrentTCB_PTR_Atomically();
extern int ignoreSignal;

void mfiber_init(long period) {
    //Initialize Ready Queue
    if (readyQueue == null && deadQueue == null) {
        sigemptyset(&sigProcMask);
        sigaddset(&sigProcMask, SIGPROF);
        ignoreSignal = 0;
        deadQueue = GetCompletedQueue();
        if (deadQueue == null) {
            //This Can happen in case of memory exhaustion
            return;
        }
        readyQueue = GetTCBQueue();
        //This Can happen in case of memory exhaustion
        if (readyQueue != null) {
            //Get TCB for main
            timeIntervalInMSec = period;

            TCB_PTR tcbOfMain = GetNewTCBWithOutStack();
            getcontext(&(tcbOfMain->context));
            //TODO: Set the completed link of TCB to the dummy function
            getFiberCompleteNotfierContext();
            tcbOfMain->context.uc_link = &notifierContext;

            //Put the TCB in ready Queue
            EnQueue(readyQueue, tcbOfMain);

            //Set Signal Handler to Call Scheduler
            memset(&schedulerHandle, 0, sizeof (schedulerHandle));
            schedulerHandle.sa_handler = &scheduler;
            sigaction(SIGPROF, &schedulerHandle, NULL);

            printf("\nmfiber library initialized\n");
            timeQuantum.it_value.tv_sec = 0;
            timeQuantum.it_value.tv_usec = timeIntervalInMSec;
            timeQuantum.it_interval.tv_sec = 0;
            timeQuantum.it_interval.tv_usec = timeIntervalInMSec;
            setitimer(ITIMER_PROF, &timeQuantum, NULL);
        }
    }
}

int mfiber_create(mfiber_t *thread, void *(*start_routine)(void *), void *arg) {

    if (readyQueue != null) {
        sigprocmask(SIG_BLOCK, &sigProcMask, NULL);
        //Create a New TCB with stack
        TCB_PTR newTCB = GetNewTCBWithOutStack();
        getcontext(&(newTCB->context));

        newTCB->context.uc_stack.ss_sp = malloc(STACKSIZE);
        if (newTCB->context.uc_stack.ss_sp == null) {
            //Memory Exhaustion
            FreeNode(newTCB);
            sigprocmask(SIG_UNBLOCK, &sigProcMask, NULL);
            return NOT_ENOUGH_MEMORY;
        }
        newTCB->context.uc_stack.ss_size = STACKSIZE;
        newTCB->context.uc_stack.ss_flags = 0;
        newTCB->hasNoStackSpaceAllocated = 0;

        //Set the ulink to CompleteHandlerContext
        newTCB->context.uc_link = &notifierContext;

        //Make Context by calling the wrapper function
        //called with the given function pointer and arguments
        makecontext(&(newTCB->context), wrapperFunction, 2, start_routine, arg);



        //Copy the value of threadid to *thread
        *thread = newTCB->tid;

        //DEBUGGING
        printf("Created Thread:%ld\n", *thread);
        //ENDDEBUGGING

        //Enqueue the item to the ready queue
        EnQueue(readyQueue, newTCB);
        sigprocmask(SIG_UNBLOCK, &sigProcMask, NULL);
        return 0;
    }

    return READY_QUEUE_NOT_INITIALIZED;
}

int mfiber_join(mfiber_t thread, void **status) {
    //Get the TCB corresponding to thread
    //Wait till
    sigprocmask(SIG_BLOCK, &sigProcMask, NULL);
    TCB_PTR currentNode = GetCurrentTCB(readyQueue);
    TCB_PTR targetNode = GetTCBByThreadID(thread, readyQueue);
    if (currentNode == targetNode || currentNode == null) {
        //if it is blocking on itself or some non existing thread
        //return
        sigprocmask(SIG_UNBLOCK, &sigProcMask, NULL);
        return INVALID_OPERATION;
    }

    //if node not present in ready queue
    //check completed queue
    if (targetNode == null) {
        DEADNODE_PTR deadNode = GetDeadNodeByThreadID(deadQueue, thread, 1);
        sigprocmask(SIG_UNBLOCK, &sigProcMask, NULL);
        if (deadNode) {
            if (status) {
                *status = *(deadNode->returnStatus);
            }
            free(deadNode);
            return 0;
        } else {
            return INVALID_OPERATION;
        }
    }

    if (targetNode->blockedThreads == null) {
        //This is to allow only one thread to be blocked on any given thread
        //by removing the above if we can allow MultiThread block feature
        AddToTCBBlockingList(targetNode, currentNode);
        int isBlocked = currentNode->isBlocked;
        sigprocmask(SIG_UNBLOCK, &sigProcMask, NULL);
        while (isBlocked) {
            //This is to avoid compiler optimization
            //and allow compiler to get the value every time
            isBlocked = currentNode->isBlocked;
        }
        sigprocmask(SIG_BLOCK, &sigProcMask, NULL);
        DEADNODE_PTR completedNode = GetDeadNodeByThreadID(deadQueue, thread, 1);
        sigprocmask(SIG_UNBLOCK, &sigProcMask, NULL);
        if (completedNode != null && status != null) {
            if (status) {
                *status = *(completedNode->returnStatus);
            }
            free(completedNode);
        }
        return 0;
    } else {
        //Already some thread blocked on this
        sigprocmask(SIG_UNBLOCK, &sigProcMask, NULL);
        return INVALID_OPERATION;
    }
}

int mfiber_equal(mfiber_t t1, mfiber_t t2) {
    return AreMfibersEqual(t1, t2);
}

mfiber_t mfiber_self(void) {
    //Get the ThreadID of the head node of the ready queue
    sigprocmask(SIG_BLOCK, &sigProcMask, NULL);
    TCB_PTR currentNode = GetCurrentTCB(readyQueue);
    if (currentNode == null) {
        sigprocmask(SIG_UNBLOCK, &sigProcMask, NULL);
        return INVALID_MFIBER_T;
    }
    sigprocmask(SIG_UNBLOCK, &sigProcMask, NULL);
    return currentNode->tid;
}

static void clearBlockedThreads(TCB_PTR targetNode) {
    BlockedThreadList blockedNode = targetNode->blockedThreads;

    //we will first unblock all the other threads waiting for this
    //thread to complete
    while (blockedNode != null) {
        blockedNode->node->isBlocked = 0;
        blockedNode = blockedNode->next;
    }

    //DEBUGGING
    printf("Thread Cancelled:%ld\n", targetNode->tid);
    //ENDDEBUGGING


    //then we mark this thread as complete
    targetNode->hasCompleted = 1;
}

int mfiber_cancel(mfiber_t thread) {
    sigprocmask(SIG_BLOCK, &sigProcMask, NULL);
    TCB_PTR currentNode = GetCurrentTCB(readyQueue);
    if ((currentNode != null) && (currentNode->tid != thread)) {
        TCB_PTR targetNode = GetTCBByThreadID(thread, readyQueue);
        if (targetNode != null) {
            clearBlockedThreads(targetNode);
            sigprocmask(SIG_UNBLOCK, &sigProcMask, NULL);
            return 0;
        }
        //Cannot Call Cancel on some non Existing thread
        sigprocmask(SIG_UNBLOCK, &sigProcMask, NULL);
        return INVALID_OPERATION;
    }
    //Cannot Call Cancel on Yourself or if gtinit is not called
    sigprocmask(SIG_UNBLOCK, &sigProcMask, NULL);
    return INVALID_OPERATION;
}

void mfiber_exit(void *retval) {
    sigprocmask(SIG_BLOCK, &sigProcMask, NULL);
    TCB_PTR currentTCB = GetCurrentTCB(readyQueue);
    DEADNODE_PTR completedNode = GetCompletedNode();
    if (completedNode != null && currentTCB != null) {
        *(completedNode->returnStatus) = retval;
        completedNode->tid = currentTCB->tid;
        EnqueueToCompletedNodeQueue(deadQueue, completedNode);
    }
    ignoreSignal = 1;
    fiberCompletedNotifier();
    sigprocmask(SIG_UNBLOCK, &sigProcMask, NULL);
    ignoreSignal = 0;
    raise(SIGPROF);

}

void mfiber_yield(void) {
    raise(SIGPROF);
}

void *wrapperFunction(void *(*start_routine)(void *), void *arg) {
    void *returnValue;
    TCB_PTR currentTCB = GetCurrentTCB_PTR_Atomically();
    returnValue = (*start_routine)(arg);
    sigprocmask(SIG_BLOCK, &sigProcMask, NULL);
    DEADNODE_PTR completedNode = GetCompletedNode();
    if (completedNode != null) {
        *(completedNode->returnStatus) = returnValue;
        completedNode->tid = currentTCB->tid;
        EnqueueToCompletedNodeQueue(deadQueue, completedNode);
    }
    sigprocmask(SIG_UNBLOCK, &sigProcMask, NULL);
    return returnValue;
}

TCB_PTR GetCurrentTCB_PTR_Atomically() {
    TCB_PTR currentTCB = null;
    sigprocmask(SIG_BLOCK, &sigProcMask, NULL);
    currentTCB = GetCurrentTCB(readyQueue);
    sigprocmask(SIG_UNBLOCK, &sigProcMask, NULL);
    return currentTCB;
}

int AreMfibersEqual(mfiber_t t1, mfiber_t t2) {
    return t1 == t2;
}