#ifndef STACK_H
#define STACK_H

#include "types.h"

#define STACK_DEPTH             (16)

#define STACK_SUCCESS           (0)
#define STACK_ERR_EMPTY         (-1)
#define STACK_ERR_FULL          (-2)
#define STACK_ERR_NULL          (-3)


#define STACK_STRUCT_GEN(type, depth)  \
typedef struct            \
{                         \
    int32_t top;          \
    type data[(depth)];   \
} stack_##type##_t


#define STACK_PROTO_GEN(type) \
int32_t stack_init_##type(stack_##type##_t *stack);\
int32_t push_##type(stack_##type##_t *stack, type *u);\
int32_t pop_##type(stack_##type##_t *stack, type *u)


STACK_STRUCT_GEN(int, STACK_DEPTH);
STACK_PROTO_GEN(int);
STACK_STRUCT_GEN(char, STACK_DEPTH);
STACK_PROTO_GEN(char);

#endif