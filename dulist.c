#include "dulist.h"



#define DULIST_EMPTY(listp) ((listp)->next == (listp)->next->next)


int32_t 
dulist_init(dulist_t *list)
{
    if (NULL == list)
        return DULIST_ERR_NULL;

    list->prev = list->next = list;

    return DULIST_SUCCESS;
}


int32_t
dulist_node_append(dulist_t *list, lnode_t *node)
{
    lnode_t *last_node = NULL;
    
    if (NULL == list || NULL == node)
        return DULIST_ERR_NULL;

    if (node->next != NULL || node->prev != NULL)
        return DULIST_ERR_OCCUPIED;
        
    last_node        = list->prev;
    list->prev       = node;
    last_node->next  = node;
    node->prev       = last_node;
    node->next       = list;
    
    return DULIST_SUCCESS;
}


int32_t
dulist_node_insert_after(lnode_t *node, lnode_t *new_node)
{
    if (NULL == node || NULL == new_node)
        return DULIST_ERR_NULL;

    if (NULL == node->next || NULL == node->prev)
        return DULIST_ERR_LIST_INVALID;
        
    if (new_node->next != NULL || new_node->prev != NULL)
        return DULIST_ERR_OCCUPIED;

    new_node->next   = node->next;
    new_node->prev   = node;
    node->next->prev = new_node;
    node->next       = new_node;

    return DULIST_SUCCESS;
}


int32_t
dulist_node_insert_before(lnode_t *node, lnode_t *new_node)
{
    if (NULL == node || NULL == new_node)
        return DULIST_ERR_NULL;

    if (NULL == node->next || NULL == node->prev)
        return DULIST_ERR_LIST_INVALID;
        
    if (new_node->next != NULL || new_node->prev != NULL)
        return DULIST_ERR_OCCUPIED;

    new_node->next   = node;
    new_node->prev   = node->prev;
    node->prev->next = new_node;
    node->prev       = new_node;

    return DULIST_SUCCESS;
}


int32_t
dulist_pop(dulist_t *list, lnode_t **node)
{
    if (NULL == list || *node == list)
        return DULIST_ERR_NULL;

    if (DULIST_EMPTY(list))
        return DULIST_ERR_LIST_EMPTY;

    *node = list->next;
    list->next = (*node)->next;
    (*node)->next->prev = (*node)->prev;
    (*node)->next = (*node)->prev = NULL;

    return DULIST_SUCCESS;
}


int32_t 
dulist_node_remove(lnode_t *node)
{    
    if (NULL == node)
        return DULIST_ERR_NULL;
    if (DULIST_EMPTY(node))
        return DULIST_ERR_LIST_EMPTY;

    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->prev = node->next = NULL;
    
    return DULIST_SUCCESS;
}


int32_t 
dulist_traval(const dulist_t *list, uint32_t dir, term_handle_t handle, const void *par, lnode_t **node)
{
    lnode_t *cur = NULL;
    int32_t ret;
    
    if (NULL == list || NULL == handle)
        return DULIST_ERR_NULL;

    cur = dir ? list->next : list->prev;
    while (cur != list)
    {
        ret = handle(cur, par);
        if (ret == DULIST_SUCCESS)
        {
            *node = cur;
            return DULIST_SUCCESS;
        }
        cur = dir ? cur->next : cur->prev;
    }

    return DULIST_ERR_NOT_FOUND;
}

