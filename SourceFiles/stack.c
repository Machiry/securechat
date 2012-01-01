#include "CommonHeader.h"
#include "dserror.h"
#include "stack.h"

STK_PTR GetNewStack(unsigned long size) {
    STK_PTR stack = (STK_PTR) malloc(sizeof (struct stack));
    if (stack != null) {
        stack->maxSize = size;
        stack->top = 0;
        stack->elements = (STK_ELEMENT*) malloc(size * sizeof (STK_ELEMENT));
    }
    return stack;
}

STK_ELEMENT GetNewStackElement(char *data) {
    STK_ELEMENT elem = (STK_ELEMENT) malloc(sizeof (struct stackElement));
    if (elem != null) {
        unsigned long len = strlen(data);
        elem->data = (char*) malloc(sizeof (char) *(len + 1));
        strncpy(elem->data, data, len);
        (elem->data)[len] = '\0';
    }
    return elem;
}

int IsStackFull(STK_PTR stk) {
    return stk == null || (stk->maxSize <= 0) || (stk->top >= (stk->maxSize - 1));
}

int Push(STK_PTR stk, char *data) {
    ErrorCode err = OPERATION_NOT_SUPPORTED;
    if (stk != null) {
        if (IsStackFull(stk)) {
            err = STACK_FULL;
        } else {
            err = OPERATION_NOT_SUPPORTED;
            if(data){
            STK_ELEMENT elem = GetNewStackElement(data);
            if (!elem) {
                err = INSUFFICIENTSPACE;
            } else {
                stk->top++;
                stk->elements[stk->top] = elem;
                err = SUCESS;
            }
            }
        }
    }
    return err;
}

int IsStackEmpty(STK_PTR stk) {
    return stk == null || stk->top <= 0;
}

char* Pop(STK_PTR stk) {
    char *data = null;

    if (stk != null && !IsStackEmpty(stk)) {
        data = stk->elements[stk->top]->data;
        free(stk->elements[stk->top]);
        stk->elements[stk->top] = null;
        stk->top--;
    }
    return data;

}

char* Peep(STK_PTR stk) {
    char *data = null;

    if (stk != null && !IsStackEmpty(stk)) {
        data = stk->elements[stk->top]->data;
    }
    return data;
}

