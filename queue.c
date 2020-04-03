//This queue is retrofitted from the one from our notes to fit our purposes
//char * are now used for values


#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct __node_t {
    
    char * value;
    struct __node_t * next;
    //length of string
    int size;
    
} node_t;

typedef struct {
    
    node_t *        head;
    node_t *        tail;
    pthread_mutex_t head_lock;
    pthread_mutex_t tail_lock;

} queue_t;

void Queue_Init(queue_t *q) {
    
    node_t *tmp = malloc(sizeof(node_t));
    tmp->next = NULL;
    q->head = q->tail = tmp;
    
    pthread_mutex_init(&q->head_lock, NULL);
    pthread_mutex_init(&q->tail_lock, NULL);
}

//adds new node to tail
void Queue_Enqueue(queue_t * q,
                   char**       value, int* size) {
    
    node_t *tmp = malloc(sizeof(node_t));
    assert(tmp != NULL);
    tmp->value = *value;
    tmp->next  = NULL;
    tmp->size=*size;

    pthread_mutex_lock(&q->tail_lock);
    q->tail->next = tmp;
    q->tail = tmp;
    pthread_mutex_unlock(&q->tail_lock);
}

//removes node from head, setting the values of value and size for use elsewhere
int Queue_Dequeue(queue_t * q,
                  char **     value,int* size) {

    // Failure is always an option
    int rc = -1;
    
    pthread_mutex_lock(&q->head_lock);
    
    node_t *tmp = q->head;
    node_t *new_head = tmp->next;
    if (new_head != NULL) {
        *value = tmp->value;
        *size=tmp->size;
        q->head = new_head;
        free(tmp);
        rc = 0;
    }
    
    pthread_mutex_unlock(&q->head_lock);

    return rc;
}