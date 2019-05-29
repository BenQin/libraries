#include "app_scheduler.h"


static app_sched_queue_t app_sched_queue; 

#define QUEUE_FULL() ((app_sched_queue.tail + 1) % MAX_SCHED_QUEUE_SIZE == app_sched_queue.head)
#define QUEUE_EMPTY() (app_sched_queue.head == app_sched_queue.tail)


int32_t 
app_sched_init(void)
{
    app_sched_queue.head = app_sched_queue.tail = 0;

    if (sizeof(app_sched_queue.evt)/sizeof(app_sched_queue.evt[0]) <= 1)
        return APP_SCHED_ERR_SIZE;

    return APP_SCHED_SUCCESS;
}


int32_t 
app_sched_evt_put(evt_t *evt)
{
    if (NULL == evt)
        return APP_SCHED_ERR_NULL;

    if (QUEUE_FULL())
        return APP_SCHED_ERR_NOMEM;

    app_sched_queue.evt[app_sched_queue.tail++] = *evt;
    if (app_sched_queue.tail >= MAX_SCHED_QUEUE_SIZE)
        app_sched_queue.tail = 0;

    return APP_SCHED_SUCCESS;
}


int32_t 
app_sched_evt_get(evt_t *evt)
{
    if (QUEUE_EMPTY())
        return APP_SCHED_ERR_EMPTY;

    if (NULL == evt)
        return APP_SCHED_ERR_NULL;
       
    *evt = app_sched_queue.evt[app_sched_queue.head++];
    if (app_sched_queue.head >= MAX_SCHED_QUEUE_SIZE)
        app_sched_queue.head = 0;

    return APP_SCHED_SUCCESS;
}


void 
app_sched_execute(void)
{
    evt_t evt;
    
    while (app_sched_evt_get(&evt) == APP_SCHED_SUCCESS)
    {
        if (evt.evt_handler != NULL)
            evt.evt_handler(evt.par);
    }
}
