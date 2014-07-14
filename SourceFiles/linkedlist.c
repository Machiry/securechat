#include "CommonHeader.h"
#include "llist.h"
#include "dserror.h"

SLListPtr GetNewSLList() {
    SLListPtr newList = (SLListPtr) malloc(sizeof (struct SLList));
    if (newList) {
        newList->length = 0;
        newList->first = newList->last = null;
    }
    return newList;
}

SLLNodePtr GetNewSLNode() {
    SLLNodePtr newNode = (SLLNodePtr) malloc(sizeof (struct SLLNode));
    if (newNode) {
        newNode->data = -1;
        newNode->next = null;
    }
    return newNode;
}

int SL_InsertAtFront(SLListPtr list, int nodeData) {
    ErrorCode err = UNSUCCESSFUL;
    if (list) {
        SLLNodePtr newNode = GetNewSLNode();
        newNode->data = nodeData;
        newNode->next = list->first;
        list->first = newNode;
        if (!list->last) {
            list->last = list->first;
        }
        list->length++;
        err = SUCESS;
    }
    return err;
}

int SL_InsertAtEnd(SLListPtr list, int nodeData) {
    ErrorCode err = UNSUCCESSFUL;
    if (list) {
        SLLNodePtr newNode = GetNewSLNode();
        newNode->data = nodeData;
        newNode->next = null;
        if (list->last) {
            list->last->next = newNode;
        }
        list->last = newNode;
        if (!list->first) {
            list->first = list->last;
        }
        list->length++;
        err = SUCESS;
    }
    return err;
}

int SL_RemoveAtFront(SLListPtr list, int *data) {
    ErrorCode err = UNSUCCESSFUL;
    if (list && list->first) {
        SLLNodePtr targetNode = list->first;
        if (list->first == list->last && list->length == 1) {
            list->last = null;
        }
        list->first = list->first->next;
        list->length--;
        if (data) {
            *data = targetNode->data;
        }
        free(targetNode);
        err = SUCESS;
    }
    return err;
}

int SL_RemoveAtEnd(SLListPtr list, int *data) {
    ErrorCode err = UNSUCCESSFUL;
    if (list && list->last && list->first) {
        int maxNoOfNodes = list->length;
        SLLNodePtr penultNode = null;
        SLLNodePtr currentNode = list->first;
        while (currentNode && currentNode != list->last && maxNoOfNodes > 0) {
            penultNode = currentNode;
            currentNode = currentNode->next;
            maxNoOfNodes--;
        }
        if (currentNode == list->last) {
            if (penultNode) {
                penultNode->next = null;
                list->last = penultNode;
                free(currentNode);
                list->length--;
            } else if (list->length == 1 && list->first == list->last) {
                list->first = list->last = null;
                free(currentNode);
                list->length--;
            }
        }

    }
    return err;
}

int IsSLListGood(SLListPtr list) {
    ErrorCode err = BAD_LIST;
    if (list) {
        if (list->first == null || list->last == null || list->length <= 0) {
            if (list->first == null && list->last == null && list->length == 0) {
                err = SUCESS;
            }
        } else {
            SLLNodePtr tempPtr = list->first;
            long noOfNodes = 1;
            while (tempPtr->next && noOfNodes <= list->length) {
                tempPtr = tempPtr->next;
                noOfNodes++;
            }
            if (noOfNodes == list->length && tempPtr == list->last) {
                err = SUCESS;
            }
        }
    }
    return !err;
}

void DelSLList(SLListPtr list) {
    SLLNodePtr curr,temp;
    if(list) {
        curr = list->first;
        while(curr) {
            temp = curr->next;
            free(curr);
            curr=temp;
        }
        free(list);
    }
}
static SLLNodePtr ReverseSLList(SLLNodePtr first, SLLNodePtr *last) {
    SLLNodePtr firstNode = null;
    if (first == null) {
        return null;
    }
    if ((firstNode = ReverseSLList(first->next, last))!=null) {
        (*last)->next = first;
        first->next = null;
        *last = first;
        return firstNode;
    }
    *last = first;
    first->next = null;
    return first;

}

SLListPtr SL_ReverseList(SLListPtr list) {
    if (list!=null && IsSLListGood(list)) {
        list->first = ReverseSLList(list->first, &(list->last));
        return list;
    }
    return null;
}

static SLLNodePtr MergeSort(SLLNodePtr head,int length){
    SLLNodePtr secondListStart,temp1,lnode=null;
    SLLNodePtr flistIterator,slistIterator,retHead=null;
    int flen,slen,counter=0,counterS,counterF;
    if(head == null || length<=1){
        return head;
    }
    slen=flen = length/2;
    slen = slen + (length&1);
    counter = 0;
    secondListStart = head;
    while(counter < flen){
        secondListStart = secondListStart->next;
        counter++;
    }

    head = MergeSort(head,flen);
    secondListStart=MergeSort(secondListStart,slen);

    flistIterator = head;
    slistIterator = secondListStart;
    counterS = 0;
    counterF=0;
    while(counterS < slen && counterF < flen){
        if(flistIterator->data > slistIterator->data){
            if(retHead == null){
                retHead = slistIterator;
            }
            if(lnode != null){
                lnode->next = slistIterator;
                lnode=slistIterator;
            } else if(lnode ==null){
                lnode = slistIterator;
            }
            temp1 = slistIterator->next;
            slistIterator->next=flistIterator;
            slistIterator = temp1;
            counterS++;

        }else{
            if(retHead == null){
                retHead = flistIterator;
            }
            if(lnode != null){
                lnode->next = flistIterator;
                lnode=flistIterator;
            } else if(lnode==null){
                lnode = flistIterator;
            }
            temp1 = flistIterator->next;
            flistIterator->next = slistIterator;
            flistIterator = temp1;
            counterF++;
        }
    }
    if(lnode && lnode->next){
        lnode = lnode->next;
        if(counterF != flen)
        lnode->next=slistIterator;
    }
    return retHead;
}

int SL_Sort(SLListPtr list){
    ErrorCode err = UNSUCCESSFUL;
    int count = 0;
    SLLNodePtr nodePtr=null;
    if(list != null && list->length > 0 && list->first != null && list->last != null){
        list->first = MergeSort(list->first,list->length);
        count=1;
        nodePtr=list->first;
        while (count<list->length) {
            nodePtr= nodePtr->next;
            count++;
        }
        list->last=nodePtr;
    }
    return err;
}

DLListPtr GetNewDLList() {
    DLListPtr newList = (DLListPtr) malloc(sizeof (struct DLList));
    if (newList) {
        newList->length = 0;
        newList->first = newList->last = null;
    }
    return newList;
}

DLLNodePtr GetNewDLNode() {
    DLLNodePtr newNode = (DLLNodePtr) malloc(sizeof (struct DLLNode));
    if (newNode) {
        newNode->next = newNode->prev = null;
        newNode->data = -1;
    }
    return newNode;
}

int DL_InsertAtFront(DLListPtr list, int nodeData) {
    ErrorCode err = UNSUCCESSFUL;
    if (list) {
        if (list->first == null || list->last == null || list->length <= 0) {
            if (list->first == null && list->last == null && list->length == 0) {
                //First Node of the list
                DLLNodePtr newNode = GetNewDLNode();
                err = INSUFFICIENTSPACE;
                if (newNode) {
                    newNode->data = nodeData;
                    newNode->next = newNode->prev = null;
                    list->first = list->last = newNode;
                    list->length++;
                    err = SUCESS;
                }
            } else {
                //Something teriblly bad happned here
                err = OPERATION_NOT_SUPPORTED;
            }
        } else {
            DLLNodePtr newNode = GetNewDLNode();
            err = INSUFFICIENTSPACE;
            if (newNode) {
                newNode->data = nodeData;
                newNode->prev = null;
                newNode->next = list->first;
                list->first->prev = newNode;
                list->first = newNode;
                list->length++;
                err = SUCESS;
            }
        }
    }
    return err;
}

int DL_InsertAtEnd(DLListPtr list, int nodeData) {
    ErrorCode err = UNSUCCESSFUL;
    if (list) {
        if (list->first == null || list->last == null || list->length <= 0) {
            if (list->first == null && list->last == null && list->length == 0) {
                //First Node of the list
                DLLNodePtr newNode = GetNewDLNode();
                err = INSUFFICIENTSPACE;
                if (newNode) {
                    newNode->data = nodeData;
                    newNode->next = newNode->prev = null;
                    list->first = list->last = newNode;
                    list->length++;
                    err = SUCESS;
                }
            } else {
                //Something teriblly bad happned here
                err = BAD_LIST;
            }
        } else {
            DLLNodePtr newNode = GetNewDLNode();
            err = INSUFFICIENTSPACE;
            if (newNode) {
                newNode->data = nodeData;
                newNode->next = null;
                newNode->prev = list->last;
                list->last->next = newNode;
                list->last = newNode;
                list->length++;
                err = SUCESS;
            }
        }
    }
    return err;
}

int DL_RemoveAtFront(DLListPtr list, int *data) {
    ErrorCode err = UNSUCCESSFUL;
    DLLNodePtr nodeToDel = null;
    if (list) {
        if (list->first == null || list->last == null || list->length <= 0) {
            if (list->first == null && list->last == null && list->length == 0) {
                err = OPERATION_NOT_SUPPORTED;
            } else {
                //Something teriblly bad happned here
                err = BAD_LIST;
            }
        } else {
            nodeToDel = list->first;
            if (data) {
                *data = nodeToDel->data;
            }
            list->first = list->first->next;
            if (list->first) {
                list->first->prev = null;
            }
            if (list->last == nodeToDel) {
                list->last = null;
            }
            free(nodeToDel);
            list->length--;
            err = SUCESS;
        }
    }
    return err;
}

int DL_RemoveAtEnd(DLListPtr list, int *data) {
    ErrorCode err = UNSUCCESSFUL;
    DLLNodePtr nodeToDel = null;
    if (list) {
        if (list->first == null || list->last == null || list->length <= 0) {
            if (list->first == null && list->last == null && list->length == 0) {
                err = OPERATION_NOT_SUPPORTED;
            } else {
                //Something teriblly bad happned here
                err = BAD_LIST;
            }
        } else {
            nodeToDel = list->last;
            if (data) {
                *data = nodeToDel->data;
            }
            list->last = list->last->prev;
            if (list->last) {
                list->last->next = null;
            }
            if (list->first == nodeToDel) {
                list->first = null;
            }
            free(nodeToDel);
            list->length--;
            err = SUCESS;
        }
    }
    return err;
}

static DLLNodePtr ReverseDLList(DLLNodePtr first,DLLNodePtr *last){
    DLLNodePtr firstRevNode = null;
    if(first == null){
        return first;
    }
    if((firstRevNode = ReverseDLList(first->next,last))!=null){
        (*last)->next=first;
        first->prev=*last;
        first->next=null;
        *last = first;
        return firstRevNode;
    }
    *last=first;
    first->next=first->prev=null;
    return first;
}

int IsDLListGood(DLListPtr list) {
    ErrorCode err = BAD_LIST;
    if (list!=null) {
        if (list->first == null || list->last == null || list->length <= 0) {
            if (list->first == null && list->last == null && list->length == 0) {
                err = SUCESS;
            }
        } else {
            DLLNodePtr tempPtr = list->first;
            long noOfNodes = 1;
            while (tempPtr->next && noOfNodes <= list->length && tempPtr->next->prev == tempPtr) {
                tempPtr = tempPtr->next;
                noOfNodes++;
            }
            if (noOfNodes == list->length && tempPtr == list->last) {
                err = SUCESS;
            }
        }
    }
    return !err;
}

DLListPtr DL_ReverseList(DLListPtr list) {
    if(list!=null && IsDLListGood(list)){
        list->first = ReverseDLList(list->first,&(list->last));
        return list;
    }
    return null;
}



