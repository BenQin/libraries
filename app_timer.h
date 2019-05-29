#ifndef APP_TIMER_H
#define APP_TIMER_H

#include <pthread.h>
#include "types.h"
#include "dulist.h"

#define MAX_APP_TIMER_NUM            (16)
#define USE_APP_SCHEDULER

#define APP_TIMER_SUCCESS            (0)
#define APP_TIMER_ERR_UNINIT         (-1)
#define APP_TIMER_ERR_NOMEM          (-2)
#define APP_TIMER_ERR_NULL           (-3)
#define APP_TIMER_ERR_NOT_FOUND      (-4)
#define APP_TIMER_ERR_PAR            (-5)
#define APP_TIMER_ERR_LIST_EMPTY     (-6)
#define APP_TIMER_ERR_OCCUPIED       (-7)


typedef struct
{
    void *magic_code;
} app_timer_id_t;

typedef enum
{
    APP_TIMER_MODE_SINGLE_SHOT,
    APP_TIMER_MODE_REPEATED
} app_timer_mode_t;

/* the container of field. pointer points to field. */
#define CONTAINER_OF(ptr, type, member) \
        ((type *)((char *)(ptr) - (unsigned int)&(((type *)0)->member)))


typedef int32_t (*app_timer_timeout_handler_t)(void *par);


typedef struct 
{
    app_timer_id_t              id;
    app_timer_timeout_handler_t timeout_handler;
    app_timer_mode_t            mode;
    void                       *par;
    lnode_t                     node;
    uint32_t                    dur;
    uint32_t                    expired;
} app_timer_t;


typedef struct
{
    pthread_cond_t  cond;
    pthread_mutex_t mutex;
    int             evt_occur;
} app_timer_evt_t;

int32_t app_timer_init(void);
int32_t app_timer_create(app_timer_id_t *id, app_timer_mode_t mode, app_timer_timeout_handler_t timeout_handler);
int32_t app_timer_start(app_timer_id_t *id, uint32_t dur, void *par);
int32_t app_timer_stop(app_timer_id_t *id);
int32_t app_timer_remove(app_timer_id_t *id);
void tick_handler(void);

#endif