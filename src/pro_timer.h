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
    MAX_SIGNALS,
}protimer_signal_t;

/* Various states of the application */
typedef enum
{
    IDLE,
    TIME_SET,
    COUNTDOWN,
    PAUSE,
    STAT,
    MAX_STATE,
}protimer_state_t;

struct protimer_tag;
struct event_tag;
enum event_status_tag;

typedef enum event_status_tag (*e_handler_t)(struct protimer_tag * const, struct event_tag const *const);

/* Main application structure*/
typedef struct protimer_tag
{
    uint32_t curr_time;
    uint32_t elpsed_time;
    uint32_t pro_time;
    protimer_state_t active_state;
    uintptr_t *state_table;
}protimer_t;

/* Generic super strucre */
typedef struct event_tag
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

typedef enum event_status_tag
{
    EVENT_HANDLED,
    EVENT_IGNORED,
    EVENT_TRANSITION,
}event_status_t;

void protimer_init(protimer_t *mobj);
event_status_t protimer_state_mechine(protimer_t * const mobj, event_t const * const e);


 // function definations 
event_status_t IDEL_entry(protimer_t *const mobj, event_t const *const e);
event_status_t IDEL_incTime(protimer_t *const mobj, event_t const *const e);
event_status_t IDEL_timeTick(protimer_t *const mobj, event_t const *const e);
event_status_t IDEL_exit(protimer_t *const mobj, event_t const *const e);
event_status_t IDEL_startStop(protimer_t *const mobj, event_t const *const e);

event_status_t TIME_SET_entry(protimer_t *const mobj, event_t const *const e);
event_status_t TIME_SET_incTime(protimer_t *const mobj, event_t const *const e);
event_status_t TIME_SET_decTime(protimer_t *const mobj, event_t const *const e);
event_status_t TIME_SET_exit(protimer_t *const mobj, event_t const *const e);
event_status_t TIME_SET_startStop(protimer_t *const mobj, event_t const *const e);
event_status_t TIME_SET_abort(protimer_t *const mobj, event_t const *const e);

event_status_t COUNTDOWN_timeTick(protimer_t *const mobj, event_t const *const e);
event_status_t COUNTDOWN_exit(protimer_t *const mobj, event_t const *const e);
event_status_t COUNTDOWN_startStop(protimer_t *const mobj, event_t const *const e);
event_status_t COUNTDOWN_abort(protimer_t *const mobj, event_t const *const e);

event_status_t PAUSE_entry(protimer_t *const mobj, event_t const *const e);
event_status_t PAUSE_incTime(protimer_t *const mobj, event_t const *const e);
event_status_t PAUSE_decTime(protimer_t *const mobj, event_t const *const e);
event_status_t PAUSE_exit(protimer_t *const mobj, event_t const *const e);
event_status_t PAUSE_startStop(protimer_t *const mobj, event_t const *const e);
event_status_t PAUSE_abort(protimer_t *const mobj, event_t const *const e);

event_status_t STAT_entry(protimer_t *const mobj, event_t const *const e);
event_status_t STAT_exit(protimer_t *const mobj, event_t const *const e);
event_status_t STAT_timeTick(protimer_t *const mobj, event_t const *const e);


#endif
