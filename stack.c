#include "stack.h"


#define STACK_EMPTY(stackp)          ((stackp)->top == -1)
#define STACK_FULL(stackp)           ((stackp)->top + 1 >= sizeof((stackp)->data)/sizeof((stackp)->data[0]))



#define STACK_INIT_GEN(type)  int32_t stack_init_##type(stack_##type##_t *stack)\
{                             \
    if (NULL == stack)        \
        return STACK_ERR_NULL;\
                              \
    stack->top = -1;          \
                              \
    return STACK_SUCCESS;     \
}


#define STACK_PUSH_GEN(type) int32_t push_##type(stack_##type##_t *stack, type *u)\
{                                     \
    if (NULL == stack || NULL == u)   \
        return STACK_ERR_NULL;        \
                                      \
    if (STACK_FULL(stack))            \
        return STACK_ERR_FULL;        \
                                      \
    stack->data[++stack->top] = *u;   \
                                      \
    return STACK_SUCCESS;             \
}



#define STACK_POP_GEN(type)  int32_t pop_##type(stack_##type##_t *stack, type *u)\
{                                         \
    if (NULL == stack || NULL == u)       \
        return STACK_ERR_NULL;            \
                                          \
    if (STACK_EMPTY(stack))               \
        return STACK_ERR_EMPTY;           \
                                          \
    if (u != NULL)                        \
        *u = stack->data[stack->top];     \
    --stack->top;                         \
                                          \
    return STACK_SUCCESS;                 \
}



#define STACK_FUNC_GEN(type) STACK_INIT_GEN(type)\
                             STACK_PUSH_GEN(type)\
                             STACK_POP_GEN(type)


STACK_FUNC_GEN(int)
STACK_FUNC_GEN(char)
                        