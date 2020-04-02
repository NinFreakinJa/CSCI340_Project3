// CSCI340_Project3.c
// Reid Foster and Ethan Guthrie
// 04/02/2020
// Implements a single-produce/multiple-consumer queue.

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.c"

int main(int argc, char const *argv[]){
    // Checking command-line arguments.
    if(argc != 2 || atoi(argv[1]) <= 0){
        printf("Usage:\t%s <consumer_task_count>\n", argv[0]);
        return 1;
    }
    int consumerTaskCount = atoi(argv[1]);

    // Reading from stdin into file for easier handling.
    queue_t lineQueue;
    Queue_Init(&lineQueue);
    FILE *fp;
    char buffer[100];
    int lineCounter = 0;
    fp = fopen("./temp.txt", "w");
    while(fgets(buffer, 100, stdin)){
        fprintf(fp, "%s", buffer);
        lineCounter++;
    }
    fclose(fp);

    // Creating multidimensional array to store lines (while stripping newline characters).
    char lines[lineCounter][100];
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
    for(int i = 0; i < lineCounter; i++){
        printf("%d:\t%s\n", i + 1, lines[i]);
        //Queue_Enqueue(&q, lines[i]);
    }
    
    return 0;
}
