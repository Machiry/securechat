/* 
 * File:   main.c
 * Author: machiry
 *
 * Created on September 27, 2011, 10:12 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <mfiber.h>
#include <mfmutex.h>

/*
 * 
 */


void *Worker(void *threadid)
{
    int i=5;
    int *returnCode = (int*)malloc(sizeof(int));
    printf("In Worker:%d\n",*(int*)threadid);
    while(i > 0)
    {
        printf("In Worker:%d\n",i--);
        fflush(stdout);
    }
    *returnCode = 783;
    return returnCode;
}

mfiber_t threadInFunc;
int threadCreated = 0;
mfiber_mutex_t mutex;
int arrived = 0;
void *Worker1(void *threadid)
{
    int i=5;
    int *jjd = (int*)malloc(sizeof(int));
    printf("In Worker1:%d\n",*(int*)threadid);
    while(i > 0)
    {
        printf("In Worker1:%d\n",i--);
        fflush(stdout);
    }
    arrived = 1;
    printf("In Worker Before Unlock\n");
    while(mfiber_mutex_lock(&mutex));
    printf("In Worker After Unlock\n");
    mfiber_create(&threadInFunc,Worker,(void*)&threadInFunc);
    threadCreated = 1;
    *jjd = 234;
    return (void*)jjd;
}

int main(int argc, char** argv) {

    int i=5;
    mfiber_t t1,t2,t3;
    int hello=5;
    void *status;


  mfiber_init(500);
  mfiber_mutex_init(&mutex);

  while(mfiber_mutex_lock(&mutex));
  
   mfiber_create(&t1,Worker1,(void*)&t1);

 

     while(i > 0)
    {
        printf("In Main:%d\n",i);
        fflush(stdout);
        i--;
    }

   //getchar();
   printf("Before Unlocking\n");
   //getchar();
   while(!arrived)
   {

   }
mfiber_mutex_unlock(&mutex);
printf("After Unlocking\n");

   while(!threadCreated)
   {
       
   }

    printf("Join Returned:%d",mfiber_join(threadInFunc,&status));
    int returnCode = *((int*)status);
    printf("\nReturn Code:%d",returnCode);
    printf("\nExiting Main\n");

}

