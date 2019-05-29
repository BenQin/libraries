#ifndef QUEUE_H
#define QUEUE_H

#include "types.h"

#define QUEUE_LEN                  (16)

#define QUEUE_SUCCESS           (0)
#define QUEUE_ERR_EMPTY         (-1)
#define QUEUE_ERR_FULL          (-2)
#define QUEUE_ERR_NULL          (-3)



#define QUEUE_STRUCT_GEN(type, queue_len)  \
typedef struct              \
{                           \
    type data[queue_len];   \
    uint32_t head;          \
    uint32_t tail;          \
} queue_##type##_t


#define QUEUE_PROTO_GEN(type)  \
int32_t queue_init_##type(queue_##type##_t *q);\
int32_t enqueue_##type(queue_##type##_t *q, type *u);\
int32_t dequeue_##type(queue_##type##_t *q, type *u)


QUEUE_STRUCT_GEN(int, QUEUE_LEN);
QUEUE_PROTO_GEN(int);

#endif
