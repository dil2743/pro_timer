#ifndef PRO_TIMER 
#define PRO_TIMER 

#include <stdint.h>
#include <stdio.h>

#define MAX_VAL UINT_LEAST32_MAX
/* Signals of the application */
typedef enum
{
    INC_TIME,
    DEC_TIME,
    START_STOP,
    TIME_TICK,
    ABORT,
/* internal activity signals */
    ENTRY,
    EXIT,
    INVALID,
}protimer_signal_t;

/* Various states of the application */
typedef enum
{
    IDLE,
    TIME_SET,
    COUNTDOWN,
    PAUSE,
    STAT,
}protimer_state_t;

/* Main application structure*/
typedef struct 
{
    uint32_t curr_time;
    uint32_t elpsed_time;
    uint32_t pro_time;
    protimer_state_t active_state;
}protimer_t;

/* Generic super strucre */
typedef struct
{
    uint8_t sig;
}event_t;

/* For user generated events*/
typedef struct
{
    event_t super;
}protimer_user_event_t;

/* For tick event*/
typedef struct 
{
    event_t super;
    uint32_t ss;
}protimer_tick_event_t;

typedef enum
{
    EVENT_HANDLED,
    EVENT_IGNORED,
    EVENT_TRANSITION,
}event_status_t;

void protimer_init(protimer_t *mobj);
event_status_t protimer_state_mechine(protimer_t * const mobj, event_t const * const e);
#endif
