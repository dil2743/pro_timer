#include "pro_timer.h"

static protimer_t protimer;
static event_t event;

static void protimer_event_dispatcher( protimer_t * const mobj, event_t const * const e);
static char read_button(void);
static event_t generate_event( char inout);

int main(void)
{
    char input;
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

    source_state = mobj->active_state;
    status = (*mobj->active_state)(mobj, e);

    if(EVENT_TRANSITION == status)
    {
        event_t ee;
        /* Run the exit action for the source */
        /* Run the entry action fot the target */
        ee.sig = EXIT;
        target_state = mobj->active_state;
        (void)(*source_state)(mobj, &ee);

        ee.sig = ENTRY;
        (void)(*target_state)(mobj, &ee);
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
