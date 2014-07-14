#include "CommonHeader.h"
#include "dserror.h"
#include "queue.h"

QUE_PTR GetNewQueue(unsigned long size) {
    QUE_PTR que = (QUE_PTR) malloc(sizeof (struct queue));
    if (que) {
        que->bottom = que->top = 0;
        que->maxSize = size;
        que->elements = (QUE_ELEMENT*) malloc(size * (sizeof (QUE_ELEMENT)));
        que->noOfElements = 0;
    }
    return que;
}

QUE_ELEMENT GetNewQueueElement(char *data) {
    QUE_ELEMENT qelem = (QUE_ELEMENT) malloc(sizeof (struct queueElement));
    if (qelem) {
        unsigned long len = strlen(data);
        qelem->data = (char*) malloc(sizeof (char) *(len + 1));
        strncpy(qelem->data, data, len);
        (qelem->data)[len] = '\0';
    }
    return qelem;
}

int IsQueueFull(QUE_PTR que) {
    return que == null || que->noOfElements >= que->maxSize;
}

int Enqueue(QUE_PTR que, char *data) {
    ErrorCode err = QUEUE_FULL;
    if (!IsQueueFull(que)) {
        err = OPERATION_NOT_SUPPORTED;
        if (data) {
            QUE_ELEMENT qelem = GetNewQueueElement(data);
            unsigned long nextIndex = que->top + 1;
            if (nextIndex >= que->maxSize) {
                nextIndex = 0;
            }
            que->elements[nextIndex] = qelem;
            if (que->noOfElements == 0) {
                que->top = que->bottom = nextIndex;
            }
            que->noOfElements++;
        }
    }
    return err;
}

int IsQueueEmpty(QUE_PTR que) {
    return que == null || que->noOfElements <= 0;
}

char* Dequeue(QUE_PTR que) {
    char *data = null;

    if (!IsQueueEmpty(que)) {
        unsigned long nextIndex = que->bottom + 1;
        if (nextIndex >= que->maxSize) {
            nextIndex = 0;
        }
        data = que->elements[que->bottom]->data;
        free(que->elements[que->bottom]);
        que->elements[que->bottom] = null;
        if (que->noOfElements == 1) {
            que->bottom = que->top = nextIndex;
        }
        que->noOfElements--;

    }
    return data;
}

char* Peepq(QUE_PTR que) {
    char *data = null;
    if (!IsQueueEmpty(que)) {
        data = que->elements[que->bottom]->data;
    }
	return data;
}




