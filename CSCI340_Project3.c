// CSCI340_Project3.c
// Reid Foster and Ethan Guthrie
// 04/02/2020
// Implements a single-produce/multiple-consumer queue.

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.c"

queue_t lineQueue;
int lineCounter;
int totalWC;
pthread_mutex_t linecountlock;
pthread_mutex_t wordcountlock;


int wordCounter(char* str,int size){
    return -1;
}

int checkIfDone(){
    pthread_mutex_lock(&linecountlock);
    if(lineCounter>0){
        lineCounter--;
        pthread_mutex_unlock(&linecountlock);
        return 1;
    }
    else{
        pthread_mutex_unlock(&linecountlock);
        return 0;
    }
}

void *consumer(void* thnum){
    int thId=*((int*)thnum);
    while(checkIfDone()==1){
        char* value=NULL;
        int size=0;
        Queue_Dequeue(&lineQueue,&value,&size);
        int count=wordCounter(value,size);
        pthread_mutex_lock(&wordcountlock);
        totalWC+=count;
        pthread_mutex_unlock(&wordcountlock);
        printf("Thread %d:  %s  : Word Count=%d\n",thId,value,count);
    }
    return NULL;
}


int main(int argc, char const *argv[]){
    // Checking command-line arguments.
    if(argc != 2 || atoi(argv[1]) <= 0){
        printf("Usage:\t%s <consumer_task_count>\n", argv[0]);
        return 1;
    }
    int consumerTaskCount = atoi(argv[1]);
    
    Queue_Init(&lineQueue);
    pthread_mutex_init(&linecountlock, NULL);
    pthread_mutex_init(&wordcountlock, NULL);
    char *line=NULL;
    size_t len=0;
    ssize_t read;
    lineCounter = 0;
    totalWC=0;
    while((read=getline(&line,&len,stdin))!=-1){
        char* curr=malloc(len);
        int size=len;
        strcpy(curr,line);
        Queue_Enqueue(&lineQueue,&curr,&size);
        lineCounter++;
    }

    pthread_t p[consumerTaskCount];
    for(int i=0;i<consumerTaskCount;i++){
        int pNum=i+1;
        pthread_create(&p[i],NULL,consumer,&pNum);
    }
    for(int i=0;i<consumerTaskCount;i++){
        pthread_join(p[i],NULL);
    }

    printf("Total Word Count: %d\n", totalWC);
   
    return 0;
}
