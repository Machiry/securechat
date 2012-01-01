#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stack.h>

void NormalizeExpression(char *exp) {
    int i = 0, j = 0, len;
    len = strlen(exp);
    for (i = 0; i < len; i++) {
        if (exp[i] == ' ' || exp[i] == '\t' || exp[i] == '\n') {

        } else {
            exp[j++] = exp[i];
        }
    }
    exp[j] = '\0';
}

int IsNumber(char c) {
    return c >= '0' && c <= '9';
}

int IsExpressionValid(char *exp) {
    int isValid = 1;
    int i = 0;
    int state = 0;
    int openParen = 0;
    for (i = 0; i < strlen(exp); i++) {
        if (IsNumber(exp[i])) {
            if (state == 0 || state == 2 || state == 3 || state == 1) {
                state = 1;
            } else {
                isValid = 0;
            }
        } else {
            switch (exp[i]) {
                case '+':
                case '*':
                case '-':
                case '/':if (state == 1 || state == 4) {
                        state = 2;
                    } else {
                        isValid = 0;
                    }
                    break;
                case '(':if (state == 0 || state == 2 || state == 3) {
                        state = 3;
                        openParen++;
                    } else {
                        isValid = 0;
                    }
                    break;
                case ')':if ((state == 1 || state == 4 || state == 3) && openParen > 0) {
                        state = 4;
                        openParen--;
                    } else {
                        isValid = 0;
                    }
                    break;
                default: isValid = 0;
                    break;
            }
        }
        if (!isValid) {
            break;
        }
    }

    if (state != 4 && state != 1) {
        isValid = 0;
    }
    return isValid;
}

int IsOperator(char c) {

    int ret = 0;
    switch (c) {
        case '+':
        case '*':
        case '-':
        case '/': ret = 1;
            break;
    }
    return ret;
}

int IsGreater(char op1, char op2) {
    int prio1 = 0, prio2 = 0;
    if (op1 == '(') {
        return 1;
    }
    if (op2 == '(') {
        return 1;
    }
    if (op1 == '+' || op1 == '-') {
        prio1 = 1;
    }
    if (op1 == '*' || op1 == '/') {
        prio1 = 2;
    }
    if (op2 == '+' || op2 == '-') {
        prio2 = 1;
    }
    if (op2 == '*' || op2 == '/') {
        prio2 = 2;
    }
    return prio1 > prio2;

}

void ExpressionTest() {
    char originalExpr[1024];
    int lengthOfString = 0, i = 0;
    char *popExp1;
    char convertedExpression[1024];
    char curr[2], j = 0;
    char c;
    printf("\nEnter the infix Expression:");
    scanf("%1023s", originalExpr);
    lengthOfString = strlen(originalExpr);
    originalExpr[lengthOfString] = '\0';
    NormalizeExpression(originalExpr);
    if (!IsExpressionValid(originalExpr)) {
        printf("\nExpression is InValid\n");
    } else {
        printf("\nExpression Is Valid\n");
        printf("Proceeding With Conversion...\n");
        lengthOfString = strlen(originalExpr);
        STK_PTR stack = GetNewStack(1024);
        for (i = 0; i < lengthOfString; i++) {
            c = originalExpr[i];
            curr[0] = c;
            curr[1] = '\0';
            if (IsNumber(c)) {
                convertedExpression[j++] = c;
            } else {
                if (!IsStackEmpty(stack)) {
                    if (c != ')') {
                        popExp1 = Peep(stack);
                        while (popExp1!=NULL && !IsGreater(c, popExp1[0])) {
                            popExp1 = Pop(stack);
                            convertedExpression[j++] = popExp1[0];
                            free(popExp1);
                            popExp1 = Peep(stack);
                        }
                        Push(stack, curr);
                    } else {
                        popExp1 = Peep(stack);
                        while (popExp1!=NULL && popExp1[0] != '(') {
                            popExp1 = Pop(stack);
                            convertedExpression[j++] = popExp1[0];
                            free(popExp1);
                            popExp1 = Peep(stack);
                        }
                        popExp1 = Pop(stack);
                        if(popExp1){
                        free(popExp1);
                        }
                    }
                } else {
                    Push(stack, curr);
                }
            }

        }
        popExp1 = Pop(stack);
        while(popExp1){
            convertedExpression[j++] = popExp1[0];
            free(popExp1);
            popExp1 = Pop(stack);
        }
        convertedExpression[j]='\0';

        printf("\nConverted Expression:%s\n", convertedExpression);

    }
}

int main(int argc, char** argv) {

    ExpressionTest();

}