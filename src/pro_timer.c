#include "pro_timer.h"


static event_status_t protimer_state_handler_IDLE(protimer_t *const mobj, event_t const *const e);
static event_status_t protimer_state_handler_TIME_SET(protimer_t *const mobj, event_t const *const e);
static event_status_t protimer_state_handler_COUNTDOWN(protimer_t *const mobj, event_t const *const e);
static event_status_t protimer_state_handler_PAUSE(protimer_t *const mobj, event_t const *const e);
static event_status_t protimer_state_handler_STAT(protimer_t *const mobj, event_t const *const e);


static void disp_msg( char* msg);
static void disp_time( uint32_t time);
static void disp_clr( void );
static void do_beep( void );

/// @brief : public function for initialization 
/// @param mobj : protimer object 

void protimer_init(protimer_t *mobj)
{
    event_t ee;
    ee.sig = ENTRY;
    mobj->active_state = IDLE;
    mobj->curr_time = 0;
    mobj->elpsed_time = 0;
    protimer_state_mechine(mobj, &ee);
    printf("protimer inti done \n");
}

event_status_t protimer_state_mechine(protimer_t * const mobj, event_t const * const e)
{
    switch (mobj->active_state)
    {
        case IDLE:
            return protimer_state_handler_IDLE(mobj, e);
            break;
        
        case TIME_SET:
            return protimer_state_handler_TIME_SET(mobj, e);
            break;
        
        case COUNTDOWN:
            return protimer_state_handler_COUNTDOWN(mobj, e);
            break;
        
        case PAUSE:
            return protimer_state_handler_PAUSE(mobj, e);
            break;
        
        case STAT:
            return protimer_state_handler_STAT(mobj, e);
            break;
        
        default:
            break;
    }
}

static event_status_t protimer_state_handler_IDLE(protimer_t *const mobj, event_t const *const e)
{

    switch (e->sig)
    {
        case ENTRY:
            disp_msg("SET TIME");
            disp_time(0);
            mobj->curr_time = 0;
            mobj->elpsed_time = 0;
            return EVENT_HANDLED;
            break;

        case INC_TIME:
            mobj->curr_time += 60;
            mobj->active_state = TIME_SET;
            return EVENT_TRANSITION;
            break;

        case TIME_TICK:
            if( ((protimer_tick_event_t *)(e))->ss == 5)
            {
                do_beep();
                return EVENT_HANDLED;
            }
            return EVENT_IGNORED;
            break;
    
        case EXIT:
            disp_clr();
            return EVENT_HANDLED;
            break;

        case START_STOP:
            mobj->active_state = STAT;
            return EVENT_TRANSITION;
            break;
    default:
        break;
    } // END of switch 
    return EVENT_IGNORED;
}

static event_status_t protimer_state_handler_TIME_SET(protimer_t *const mobj, event_t const *const e)
{
    switch (e->sig)
    {
        case ENTRY:
            disp_time(mobj->curr_time);
            return EVENT_HANDLED;
            break;

        case INC_TIME:
            if(mobj->curr_time < (MAX_VAL - 60) )
            {
                mobj->curr_time += 60;
                disp_time(mobj->curr_time);
            }
            return EVENT_HANDLED;
            break;

        case DEC_TIME:
            if(mobj->curr_time >= 60)
            {
                mobj->curr_time -= 60;
                disp_time(mobj->curr_time);
            }
            return EVENT_HANDLED;
            break;
            
        case EXIT:
            disp_clr();
            return EVENT_HANDLED;
            break;

        case START_STOP:
            if( mobj->curr_time >= 60)
            {
                mobj->active_state = COUNTDOWN;
                return EVENT_TRANSITION;
            }
            return EVENT_IGNORED;
            break;
        
        case ABORT:
            mobj->active_state = IDLE;
            return EVENT_TRANSITION;
            break;
        
        default:
            break;
    } // END of switch 
    return EVENT_IGNORED;
}

static event_status_t protimer_state_handler_COUNTDOWN(protimer_t *const mobj, event_t const *const e)
{
    static uint8_t tick_count = 0;
    switch (e->sig)
    {
        
        case TIME_TICK:
            if(++tick_count >= 10)
            {
                tick_count = 0;
                mobj->curr_time --;
                mobj->elpsed_time ++;
                disp_time(mobj->curr_time);

                if(0 == mobj->curr_time)
                {
                    mobj->active_state = IDLE;
                    return EVENT_TRANSITION;
                }
                return EVENT_HANDLED;
            }
            return EVENT_IGNORED;
            break;
    
        case EXIT:
            mobj->pro_time = mobj->elpsed_time;
            mobj->elpsed_time = 0;
            return EVENT_HANDLED;
            break;

        case START_STOP:
            mobj->active_state = PAUSE;
            return EVENT_TRANSITION;
            break;

        case ABORT:
            mobj->active_state = IDLE;
            return EVENT_TRANSITION;
            break;
    default:
        break;
    } // END of switch 
    return EVENT_IGNORED;
}

static event_status_t protimer_state_handler_PAUSE(protimer_t *const mobj, event_t const *const e)
{
    switch (e->sig)
    {
        case ENTRY:
            disp_msg("PAUSED\n");
            return EVENT_HANDLED;
            break;

        case INC_TIME:
            if(mobj->curr_time < (MAX_VAL - 60) )
            {
                mobj->curr_time += 60;
                mobj->active_state = TIME_SET;
                return EVENT_TRANSITION;
            }
            return EVENT_IGNORED;
            break;

        case DEC_TIME:
            if(mobj->curr_time >= 60)
            {
                mobj->curr_time -= 60;
                mobj->active_state = TIME_SET;
                return EVENT_TRANSITION;
            }
            return EVENT_IGNORED;
            break;
            
        case EXIT:
            disp_clr();
            return EVENT_HANDLED;
            break;

        case START_STOP:
            mobj->active_state = COUNTDOWN;
            return EVENT_TRANSITION;
            break;
        
        case ABORT:
            mobj->active_state = IDLE;
            return EVENT_TRANSITION;
            break;
        
        default:
            break;
    } // END of switch 
    return EVENT_IGNORED;
}

static event_status_t protimer_state_handler_STAT(protimer_t *const mobj, event_t const *const e)
{   
    static uint8_t tick_count = 0;
    switch (e->sig)
    {
        case ENTRY:
            disp_msg("PRODUCTIVE TIME \t ");
            disp_time(mobj->pro_time);
            return EVENT_HANDLED;
            break;

        case TIME_TICK:
            if( ++tick_count >= 30)
            {
                tick_count = 0;
                mobj->active_state = IDLE;
                return EVENT_TRANSITION;
            }
            return EVENT_IGNORED;
            break;
    
        case EXIT:
            disp_clr();
            return EVENT_HANDLED;
            break;

        default:
            break;
    } // END of switch 
    return EVENT_IGNORED;
}

///////////////////// HELPER FUNCTIONS /////////////


static void disp_msg( char* msg)
{
    printf("%s\n",msg);
}

static void disp_time( uint32_t time)
{
    printf("TIME %d\n",time);
}

static void disp_clr(void)
{
    printf(" DISPLAY CLR\n");
}

static void do_beep(void)
{
    printf("BLOOP BLOOP ...\n");
}
