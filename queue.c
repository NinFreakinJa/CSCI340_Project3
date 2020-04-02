// This shows how the head and tail of the list could
// be separately be locked allowing pushing and poping
// to occue in parallel.

// Note: All pthread functions should have their return codes
//       checked. The checking has been omitted to clarity in
//       this example.


#include <assert.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct __node_t {
    
    char * value;
    struct __node_t * next;
    
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

void Queue_Enqueue(queue_t * q,
                   char*       value) {
    
    node_t *tmp = malloc(sizeof(node_t));
    assert(tmp != NULL);
    tmp->value = value;
    tmp->next  = NULL;

    pthread_mutex_lock(&q->tail_lock);
    q->tail->next = tmp;
    q->tail = tmp;
    pthread_mutex_unlock(&q->tail_lock);
    
}

int Queue_Dequeue(queue_t * q,
                  char *     value) {

    // Failure is always an option
    int rc = -1;
    
    pthread_mutex_lock(&q->head_lock);
    
    node_t *tmp = q->head;
    node_t *new_head = tmp->next;
    if (new_head != NULL) {
        *value = new_head->value;
        q->head = new_head;
        free(tmp);
        rc = 0;
    }
    
    pthread_mutex_unlock(&q->head_lock);
    
    return rc;
    
}
