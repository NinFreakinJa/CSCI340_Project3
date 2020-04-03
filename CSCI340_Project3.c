// CSCI340_Project3.c
// Reid Foster and Ethan Guthrie
// 04/02/2020
// Implements a single-produce/multiple-consumer queue.

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>

#include "queue.c"

queue_t lineQueue;
int lineCounter;
int totalWC;
pthread_mutex_t linecountlock;
pthread_mutex_t wordcountlock;
pthread_mutex_t donelock;
pthread_cond_t c= PTHREAD_COND_INITIALIZER;
sem_t s;
int done;


int wordCounter(char* str,int size){
    return -1;
}

int checkIfDone(){
    pthread_mutex_lock(&linecountlock);
    pthread_mutex_lock(&donelock);
    if(lineCounter>0&&done==0){
        lineCounter--;
        pthread_mutex_unlock(&donelock);
        pthread_mutex_unlock(&linecountlock);
        return 1;
    }
    else if(lineCounter<=0 && done==0){
        pthread_mutex_unlock(&donelock);
        pthread_cond_wait(&c,&linecountlock);
        pthread_mutex_unlock(&linecountlock);
        checkIfDone();
    }
    else if(lineCounter>0 && done==1){
        lineCounter--;
        pthread_mutex_unlock(&donelock);
        pthread_mutex_unlock(&linecountlock);
        return 1;
    }
    else{
        pthread_mutex_unlock(&donelock);
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

    // Reading from stdin into file for easier handling.
    
    Queue_Init(&lineQueue);
    pthread_mutex_init(&linecountlock, NULL);
    pthread_mutex_init(&wordcountlock, NULL);
    pthread_mutex_init(&donelock, NULL);
    sem_init(&s,0,0);
    //FILE *fp;
    char *line=NULL;
    size_t len=0;
    ssize_t read;
    //char buffer[100];
    lineCounter = 0;
    totalWC=0;
    done=0;
    //fp = fopen("./temp.txt", "w");
    //while(fgets(buffer, 100, stdin)){
    

    pthread_t p[consumerTaskCount];
    for(int i=0;i<consumerTaskCount;i++){
        int pNum=i+1;
        pthread_create(&p[i],NULL,consumer,&pNum);
    }
    int complete=0;
    while((read=getline(&line,&len,stdin))!=-1 || complete==0){
        //fprintf(fp, "%s", buffer);
        if(read==-1){
            complete=1;
        }
        char* curr=malloc(len);
        int size=len;
        strcpy(curr,line);
        Queue_Enqueue(&lineQueue,&curr,&size);
        pthread_mutex_lock(&linecountlock);
        lineCounter++;
        pthread_mutex_unlock(&linecountlock);
        pthread_cond_signal(&c);
    }
    pthread_mutex_lock(&donelock);
    done=1;
    pthread_mutex_unlock(&donelock);
    //sem_post(&s);
    for(int i=0;i<consumerTaskCount;i++){
        pthread_join(p[i],NULL);
    }

    printf("Total Word Count: %d\n", totalWC);

    //fclose(fp);

    // Creating multidimensional array to store lines (while stripping newline characters).
   /* char lines[lineCounter][100];
    // Reading lines from file into 'lines' array.
    int counter = 0;
    char *pos;
    fp = fopen("./temp.txt", "r");
    while(fgets(buffer, 100, fp)){
        // Stripping newline character.
        if ((pos = strchr(buffer, '\n')) != NULL){
            *pos = '\0';
        }
        // Copying formatted string to array.
        strcpy(lines[counter], buffer);
        counter++;
    }
    fclose(fp);
    remove("./temp.txt");

    // Enqueueing items.
    //queue_t *q;
    //Queue_Init(&q);
    */
    /*for(int i = 0; i < lineCounter; i++){
        char* value=NULL;
        Queue_Dequeue(&lineQueue,&value);
        printf("%d:\t%s\n", i + 1, value);
        //Queue_Enqueue(&q, lines[i]);
    }*/
    
   
    return 0;
}
