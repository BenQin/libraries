#ifndef APP_SCHEDULER_H
#define APP_SCHEDULER_H

#include "types.h"

#define MAX_SCHED_QUEUE_SIZE            (16)

#define APP_SCHED_SUCCESS               (0)
#define APP_SCHED_ERR_NULL              (-1)
#define APP_SCHED_ERR_NOMEM             (-2)
#define APP_SCHED_ERR_EMPTY             (-3)
#define APP_SCHED_ERR_SIZE              (-4)


typedef int32_t (*evt_handler_t)(void *par);

typedef struct
{
    evt_handler_t evt_handler;
    void         *par;
} evt_t;

typedef struct
{
    evt_t    evt[MAX_SCHED_QUEUE_SIZE];
    uint32_t head;
    uint32_t tail;
} app_sched_queue_t;

int32_t app_sched_init(void);
void app_sched_execute(void);
int32_t app_sched_evt_put(evt_t *evt);

#endif