#ifndef DULIST_H
#define DULIST_H

#include "types.h"


#define DULIST_SUCCESS               (0)
#define DULIST_ERR_NULL              (-1)
#define DULIST_ERR_LIST_EMPTY        (-2)
#define DULIST_ERR_OCCUPIED          (-3)
#define DULIST_ERR_LIST_INVALID      (-4)
#define DULIST_ERR_NOT_FOUND         (-5)



struct lnode;
typedef struct lnode
{
    struct lnode *prev;
    struct lnode *next;
} lnode_t;

typedef lnode_t dulist_t;

typedef int32_t (*term_handle_t)(lnode_t *node, const void *par);


int32_t dulist_init(dulist_t *list);
int32_t dulist_node_append(dulist_t *list, lnode_t *node);
int32_t dulist_pop(dulist_t *list, lnode_t **node);
int32_t dulist_node_insert_after(lnode_t *node, lnode_t *new_node);
int32_t dulist_node_insert_before(lnode_t *node, lnode_t *new_node);
int32_t dulist_node_remove(lnode_t *node);
int32_t dulist_traval(const dulist_t *list, uint32_t dir, term_handle_t handle, const void *par, lnode_t **node);
#endif