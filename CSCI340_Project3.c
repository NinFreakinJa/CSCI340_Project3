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
pthread_mutex_t donelock;
pthread_cond_t c= PTHREAD_COND_INITIALIZER;
int done;

int checkIfDone();
void *consumer(void* thnum);
int isAlpha(char c);
int wordCounter(char* str,int size);

int main(int argc, char const *argv[]){
    // Checking command-line arguments.
    if(argc != 2 || atoi(argv[1]) <= 0){
        printf("Usage:\t%s <consumer_task_count>\n", argv[0]);
        return 1;
    }
    int consumerTaskCount = atoi(argv[1]);
    
    //Initialize all global variables
    Queue_Init(&lineQueue);
    pthread_mutex_init(&linecountlock, NULL);
    pthread_mutex_init(&wordcountlock, NULL);
    pthread_mutex_init(&donelock, NULL);;
    char *line=NULL;
    size_t len=0;
    ssize_t read;
    lineCounter = 0;
    totalWC=0;
    done=0;

    //Create specified number of threads
    int arr[consumerTaskCount];
    for(int i=0;i<consumerTaskCount;i++){
        arr[i]=i+1;
    }
    printf("  Thread  |  Word Count  |  Line\n---------------------------------->\n");
    pthread_t p[consumerTaskCount];
    for(int i=0;i<consumerTaskCount;i++){
        pthread_create(&p[i],NULL,consumer,&arr[i]);
    }

    //Reads the file line by line into the queue simultaneous with the consumers
    int complete=0;
    while((read=getline(&line,&len,stdin))!=-1 || complete==0){
        if(read==-1){
            complete=1;
        }
        char* curr=malloc(len);
        int size=len;
        strcpy(curr,line);
        curr[strcspn(curr, "\n")] = 0;
        Queue_Enqueue(&lineQueue,&curr,&size);
        pthread_mutex_lock(&linecountlock);
        lineCounter++;
        pthread_mutex_unlock(&linecountlock);
        //signals waiting consumer
        pthread_cond_signal(&c);
    }
    //tells consumer that the producer is done with its task
    pthread_mutex_lock(&donelock);
    done=1;
    pthread_mutex_unlock(&donelock);
    //rejoin threads
    for(int i=0;i<consumerTaskCount;i++){
        pthread_join(p[i],NULL);
    }

    printf("Total Word Count: %d\n", totalWC);
   
    return 0;
}

//checks if the consumer thread should continue, wait, or terminate
int checkIfDone(){
    pthread_mutex_lock(&linecountlock);
    pthread_mutex_lock(&donelock);
    //there are still lines in the queue, continue
    if(lineCounter>0&&done==0){
        lineCounter--;
        pthread_mutex_unlock(&donelock);
        pthread_mutex_unlock(&linecountlock);
        return 1;
    }
    //there is nothing in the queue but the producer is not done, wait for signal and retest
    else if(lineCounter<=0 && done==0){
        pthread_mutex_unlock(&donelock);
        pthread_cond_wait(&c,&linecountlock);
        pthread_mutex_unlock(&linecountlock);
        return checkIfDone();
    }
    //the producer is done but the queue still has items, continue
    else if(lineCounter>0 && done==1){
        lineCounter--;
        pthread_mutex_unlock(&donelock);
        pthread_mutex_unlock(&linecountlock);
        return 1;
    }
    //there is nothing in the queue and the producer is done, terminate
    else{
        pthread_mutex_unlock(&donelock);
        pthread_mutex_unlock(&linecountlock);
        return 0;
    }
}

//Determines what the threads do
void *consumer(void* thnum){
    int thId=*((int*)thnum);
    //will loop until queue is empty and producer signals completion
    //dequeues line, counts the words, and outputs information
    while(checkIfDone()==1){
        char* value=NULL;
        int size=0;
        Queue_Dequeue(&lineQueue,&value,&size);
        int count=wordCounter(value,size);
        pthread_mutex_lock(&wordcountlock);
        totalWC+=count;
        pthread_mutex_unlock(&wordcountlock);
        printf("    %.2d    |     %.4d     |  %s\n",thId,count,value);
    }
    return NULL;
}

int wordCounter(char* str,int size){
    int spacePrev = 0;
    int wordCount = 0;
    for(int i = 0; i < size; i++){
        if(str[i] == ' '){
            if(!spacePrev){
                wordCount++;
            }
            spacePrev = 1;
        }else{
            spacePrev = 0;
        }
    }

    if(wordCount > 0){
        return ++wordCount;
    }
    return 0;
}