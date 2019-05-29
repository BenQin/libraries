#include "queue.h"


#define QUEUE_FULL(queuep)       (((queuep)->tail+1)%(sizeof((queuep)->data)/sizeof((queuep)->data[0])) == (queuep)->head)
#define QUEUE_EMPTY(queuep)      ((queuep)->tail == (queuep)->head)


#define QUEUE_INIT_GEN(type)  int32_t queue_init_##type(queue_##type##_t *q)\
{                                 \
     if (NULL == q)               \
         return QUEUE_ERR_NULL;   \
                                  \
     q->head = q->tail = 0;       \
                                  \
     return QUEUE_SUCCESS;        \
}


#define ENQUEUE_GEN(type)  int32_t enqueue_##type(queue_##type##_t *q, type *u)\
{                                                         \
    if (NULL == q || NULL == u)                           \
        return QUEUE_ERR_NULL;                            \
                                                          \
    if (QUEUE_FULL(q))                                    \
        return QUEUE_ERR_FULL;                            \
                                                          \
    q->data[q->tail++] = *u;                              \
    if (q->tail >= sizeof(q->data)/sizeof(q->data[0]))    \
        q->tail = 0;                                      \
                                                          \
    return QUEUE_SUCCESS;                                 \
}



#define DEQUEUE_GEN(type)  int32_t dequeue_##type(queue_##type##_t *q, type *u)\
{                                                         \
    if (NULL == q)                                        \
        return QUEUE_ERR_NULL;                            \
                                                          \
    if (QUEUE_EMPTY(q))                                   \
        return QUEUE_ERR_EMPTY;                           \
                                                          \
    if (u != NULL)                                        \
        *u = q->data[q->head];                            \
    ++q->head;                                            \
    if (q->head >= sizeof(q->data)/sizeof(q->data[0]))    \
        q->head = 0;                                      \
                                                          \
    return QUEUE_SUCCESS;                                 \
}


#define QUEUE_FUNC_GEN(type)         QUEUE_INIT_GEN(type)\
                                     ENQUEUE_GEN(type)\
                                     DEQUEUE_GEN(type)

QUEUE_FUNC_GEN(int)