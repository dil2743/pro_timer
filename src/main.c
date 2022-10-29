#include "pro_timer.h"

static protimer_t protimer;
static event_t event;

static void protimer_event_dispatcher( protimer_t * const mobj, event_t const * const e);
static char read_button(void);
static event_t generate_event( char inout);
static void protimer_stateTable_init(protimer_t * const mobj);

int main(void)
{
    char input;
    protimer_stateTable_init(&protimer);
    protimer_init(&protimer);
    while(1)
    {
        // 1. read the buttons
        // 2. make a event 
        // 3. send it to event dispactcher 
        input = read_button();
        event = generate_event(input);
        protimer_event_dispatcher(&protimer, &event);
    }
    return 0;    
}


static void protimer_event_dispatcher( protimer_t * const mobj, event_t const * const e)
{   
    event_status_t status;
    protimer_state_t source_state, target_state;
    e_handler_t eHandler;

    source_state = mobj->active_state;
    eHandler = (e_handler_t) mobj->state_table[source_state * MAX_SIGNALS + e->sig];
    if( NULL != eHandler)
        status = (*eHandler)(mobj,e);

    if(EVENT_TRANSITION == status)
    {
        event_t ee;
        /* Run the exit action for the source */
        ee.sig = EXIT;
        target_state = mobj->active_state;
        eHandler = (e_handler_t) mobj->state_table[source_state * MAX_STATE + EXIT];
        if( NULL != eHandler)
            (void)(*eHandler)(mobj,&ee);

        
        /* Run the entry action fot the target */
        ee.sig = ENTRY;
        eHandler = (e_handler_t) mobj->state_table[target_state * MAX_STATE + ENTRY];
        if( NULL != eHandler)
            (void)(*eHandler)(mobj,&ee);
    }
}



static char read_button( void )
{
    char input = '0';
    printf("\nWaiting for input ");
    scanf("%c",&input);
    return input;
}

static event_t generate_event( char input )
{
    event_t e;
    switch(input)
    {
        case 'I':
            e.sig = INC_TIME;
            break;
        case 'D':
            e.sig = DEC_TIME;
            break;
        case 'P':
            e.sig = START_STOP;
            break;
        case 'X':
            e.sig = ABORT;
            break;
        case 'T':
            e.sig = TIME_TICK;
        case 'E':
            e.sig = ENTRY;    
        default:
            e.sig = INVALID;
            break;
    }
    return e;
}

void protimer_stateTable_init(protimer_t * const mobj)
{
    static e_handler_t protimer_state_table[MAX_STATE][MAX_SIGNALS] = 
    {
        /* IncTime - DecTime  - TimeTick 
        - StartStop - Abort  -  Entry - Exit */
        [IDLE] = {&IDEL_incTime, NULL, &IDEL_timeTick,
                  &IDEL_startStop, NULL, &IDEL_entry, &IDEL_exit },
        [TIME_SET] = {&TIME_SET_incTime, &TIME_SET_decTime, NULL,
                  &TIME_SET_startStop, &TIME_SET_abort, &TIME_SET_entry, &TIME_SET_exit },
        [COUNTDOWN] = { NULL, NULL, &COUNTDOWN_timeTick,
                  &COUNTDOWN_startStop, &COUNTDOWN_abort, NULL, &COUNTDOWN_exit },
        [PAUSE] = {&PAUSE_incTime, &PAUSE_decTime, NULL,
                  &PAUSE_startStop, &PAUSE_abort, &PAUSE_entry, &PAUSE_exit },
        [ABORT] = {NULL, NULL, &STAT_timeTick,
                  NULL, NULL, &STAT_entry, &STAT_exit },
    };

    mobj->state_table = (uintptr_t*) &protimer_state_table[0][0];

}