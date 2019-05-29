#include "app_timer.h"
#ifdef USE_APP_SCHEDULER
#include "app_scheduler.h"
#endif


static app_timer_t app_timer[MAX_APP_TIMER_NUM];
static dulist_t free_app_timer_list;
static dulist_t volatile ready_app_timer_list;
static dulist_t volatile running_app_timer_list;
static volatile uint8_t app_timer_inited;
app_timer_evt_t app_timer_evt = {PTHREAD_COND_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, 0};

static int32_t
term_handler(lnode_t *node, const void *par)
{
    app_timer_t *timer_node = NULL;
    const app_timer_id_t *id = (const app_timer_id_t *)par;

    if (NULL == node || NULL == par)
        return APP_TIMER_ERR_NULL;

    timer_node = CONTAINER_OF(node, app_timer_t, node);
    if (timer_node->id.magic_code == id->magic_code)
        return DULIST_SUCCESS;
    else
        return DULIST_ERR_NOT_FOUND;
}


static int32_t
app_timer_node_find(const dulist_t *list, const app_timer_id_t *id, lnode_t **node)
{
    int32_t ret;
    
    if (NULL == list || NULL == node || NULL == id)
        return APP_TIMER_ERR_NULL;

    ret = dulist_traval(list, 1, term_handler, id, node);
    if (ret == DULIST_SUCCESS)
        return APP_TIMER_SUCCESS;

    return APP_TIMER_ERR_NOT_FOUND;
}


int32_t 
app_timer_init(void)
{
    uint32_t i = 0;

    app_timer_inited = 0;
    dulist_init((dulist_t *)&ready_app_timer_list);
    dulist_init((dulist_t *)&running_app_timer_list);
    dulist_init((dulist_t *)&free_app_timer_list);
    for (i = 0; i < sizeof(app_timer)/sizeof(app_timer[0]); ++i)
        dulist_node_append(&free_app_timer_list, &app_timer[i].node);
    app_timer_inited = 1;
    
    return APP_TIMER_SUCCESS;
}


int32_t 
app_timer_create(app_timer_id_t *id, app_timer_mode_t mode, app_timer_timeout_handler_t timeout_handler)
{
    lnode_t *node = NULL;
    app_timer_t *timer_node = NULL;
    int32_t ret;

    if (NULL == id)
        return APP_TIMER_ERR_NULL;

    if (!app_timer_inited)
        return APP_TIMER_ERR_UNINIT;

    ret = dulist_pop(&free_app_timer_list, &node);
    if (ret != DULIST_SUCCESS)
        return ret;
    timer_node                  = CONTAINER_OF(node, app_timer_t, node);
    timer_node->timeout_handler = timeout_handler;
    timer_node->mode            = mode;
    timer_node->par             = NULL;
    timer_node->dur             = 0;
    timer_node->id.magic_code   = timer_node;
    id->magic_code              = timer_node;
    ret = dulist_node_append((dulist_t *)&ready_app_timer_list, node);
    
    return ret;
}


int32_t 
app_timer_start(app_timer_id_t *id, uint32_t dur, void *par)
{
    int32_t ret;
    lnode_t *node = NULL;
    app_timer_t *timer_node = NULL;
        
    if (NULL == id)
        return APP_TIMER_ERR_NULL;
        
    if (0 == dur)
        return APP_TIMER_ERR_PAR;

    if (!app_timer_inited)
        return APP_TIMER_ERR_UNINIT;

    ret = app_timer_node_find((dulist_t *)&ready_app_timer_list, id, &node);
    if (ret != APP_TIMER_SUCCESS)
        return ret;
        
    dulist_node_remove(node);
    timer_node = CONTAINER_OF(node, app_timer_t, node);
    timer_node->dur     = dur;
    timer_node->par     = par;
    timer_node->expired = 0;
    dulist_node_append((dulist_t *)&running_app_timer_list, node);
    
    return APP_TIMER_SUCCESS;
}


int32_t 
app_timer_stop(app_timer_id_t *id)
{
    int ret;
    lnode_t *node = NULL;
        
    if (NULL == id)
        return APP_TIMER_ERR_NULL;

    if (!app_timer_inited)
        return APP_TIMER_ERR_UNINIT;

    ret = app_timer_node_find((dulist_t *)&running_app_timer_list, id, &node);
    if (ret != APP_TIMER_SUCCESS)
        return ret;

    dulist_node_remove(node);
    dulist_node_append((dulist_t *)&ready_app_timer_list, node);
    
    return APP_TIMER_SUCCESS;
}


int32_t 
app_timer_remove(app_timer_id_t *id)
{
    int ret;
    lnode_t *node = NULL;
        
    if (NULL == id)
        return APP_TIMER_ERR_NULL;

    if (!app_timer_inited)
        return APP_TIMER_ERR_UNINIT;

    ret = app_timer_node_find((dulist_t *)&running_app_timer_list, id, &node);
    if (ret == APP_TIMER_SUCCESS)
         app_timer_stop(id);
         
    ret = app_timer_node_find((dulist_t *)&ready_app_timer_list, id, &node);
    if (ret != APP_TIMER_SUCCESS)
        return ret;

    dulist_node_remove(node);
    dulist_node_append(&free_app_timer_list, node);
    id->magic_code = NULL;
    
    return APP_TIMER_SUCCESS;
}


void
tick_handler(void)
{
    lnode_t *cur = running_app_timer_list.next;
    app_timer_t *timer_node = NULL;
    evt_t evt;
    int evt_occur = 0;

    if (!app_timer_inited)
        return;

    while (cur != &running_app_timer_list)
    {
        timer_node = CONTAINER_OF(cur, app_timer_t, node);
        timer_node->expired++;
        if (timer_node->expired >= timer_node->dur)
        {
            switch (timer_node->mode)
            {
                case APP_TIMER_MODE_REPEATED:
                    timer_node->expired = 0;
                    break;
                case APP_TIMER_MODE_SINGLE_SHOT:
                    dulist_node_remove(&timer_node->node);
                    dulist_node_append((dulist_t *)&ready_app_timer_list, &timer_node->node);
                    break;
                default:
                    break;
            }
        #ifdef USE_APP_SCHEDULER
            evt.evt_handler = timer_node->timeout_handler;
            evt.par         = timer_node->par;
            app_sched_evt_put(&evt);
            evt_occur = 1;
        #else
            if (timer_node->timeout_handler != NULL)
                timer_node->timeout_handler(timer_node->par);
        #endif
        }
        cur = cur->next;
    }
    
#ifdef USE_APP_SCHEDULER
    if (evt_occur)
    {
	    pthread_mutex_lock(&app_timer_evt.mutex);
	    app_timer_evt.evt_occur = 1;
	    pthread_cond_signal(&app_timer_evt.cond);
	    pthread_mutex_unlock(&app_timer_evt.mutex);
    }
#endif
}

