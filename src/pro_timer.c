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
    e_handler_t eHandler;
    event_t ee;
    ee.sig = ENTRY;
    mobj->active_state = IDLE;
    mobj->curr_time = 0;
    mobj->elpsed_time = 0;
    eHandler = (e_handler_t) mobj->state_table[IDLE * MAX_SIGNALS + ENTRY];
    if( NULL != eHandler)
    {
        (*eHandler)(mobj,&ee);
        printf("protimer inti done \n");
    }
    else
    {
        printf("protimer inti FAILED \n");
    }
}

event_status_t IDEL_entry(protimer_t *const mobj, event_t const *const e)
{
    disp_msg("SET TIME");
    disp_time(0);
    mobj->curr_time = 0;
    mobj->elpsed_time = 0;
    return EVENT_HANDLED;
}
event_status_t IDEL_incTime(protimer_t *const mobj, event_t const *const e)
{
    mobj->curr_time += 60;
    mobj->active_state = TIME_SET;
    return EVENT_TRANSITION;    
}
event_status_t IDEL_timeTick(protimer_t *const mobj, event_t const *const e)
{
    if( ((protimer_tick_event_t *)(e))->ss == 5)
    {
        do_beep();
        return EVENT_HANDLED;
    }
    return EVENT_IGNORED;
}
event_status_t IDEL_exit(protimer_t *const mobj, event_t const *const e)
{ 
    disp_clr();
    return EVENT_HANDLED;
}
event_status_t IDEL_startStop(protimer_t *const mobj, event_t const *const e)
{
    mobj->active_state = STAT;
    return EVENT_TRANSITION;

}

event_status_t TIME_SET_entry(protimer_t *const mobj, event_t const *const e)
{
    disp_time(mobj->curr_time);
    return EVENT_HANDLED;
}
event_status_t TIME_SET_incTime(protimer_t *const mobj, event_t const *const e)
{
   if(mobj->curr_time < (MAX_VAL - 60) )
    {
        mobj->curr_time += 60;
        disp_time(mobj->curr_time);
    }
    return EVENT_HANDLED; 
}
event_status_t TIME_SET_decTime(protimer_t *const mobj, event_t const *const e)
{
   if(mobj->curr_time >= 60)
    {
        mobj->curr_time -= 60;
        disp_time(mobj->curr_time);
    }
    return EVENT_HANDLED; 
}
event_status_t TIME_SET_exit(protimer_t *const mobj, event_t const *const e)
{
    disp_clr();
    return EVENT_HANDLED; 
}
event_status_t TIME_SET_startStop(protimer_t *const mobj, event_t const *const e)
{
    if( mobj->curr_time >= 60)
    {
        mobj->active_state = COUNTDOWN;
        return EVENT_TRANSITION;
    }
    return EVENT_IGNORED;
}
event_status_t TIME_SET_abort(protimer_t *const mobj, event_t const *const e)
{
    mobj->active_state = IDLE;
    return EVENT_TRANSITION;
}

event_status_t COUNTDOWN_timeTick(protimer_t *const mobj, event_t const *const e)
{
    static uint8_t tick_count = 0;
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
}
event_status_t COUNTDOWN_exit(protimer_t *const mobj, event_t const *const e)
{
    mobj->pro_time = mobj->elpsed_time;
    mobj->elpsed_time = 0;
    return EVENT_HANDLED;
}
event_status_t COUNTDOWN_startStop(protimer_t *const mobj, event_t const *const e)
{
    mobj->active_state = PAUSE;
    return EVENT_TRANSITION; 
}
event_status_t COUNTDOWN_abort(protimer_t *const mobj, event_t const *const e)
{
    mobj->active_state = IDLE;
    return EVENT_TRANSITION;
}

event_status_t PAUSE_entry(protimer_t *const mobj, event_t const *const e)
{

    disp_msg("PAUSED\n");
    return EVENT_HANDLED;
}
event_status_t PAUSE_incTime(protimer_t *const mobj, event_t const *const e)
{
    if(mobj->curr_time < (MAX_VAL - 60) )
    {
        mobj->curr_time += 60;
        mobj->active_state = TIME_SET;
        return EVENT_TRANSITION;
    }
    return EVENT_IGNORED;  
}
event_status_t PAUSE_decTime(protimer_t *const mobj, event_t const *const e)
{
    if(mobj->curr_time >= 60)
    {
        mobj->curr_time -= 60;
        mobj->active_state = TIME_SET;
        return EVENT_TRANSITION;
    }
    return EVENT_IGNORED;
}
event_status_t PAUSE_exit(protimer_t *const mobj, event_t const *const e)
{
    disp_clr();
    return EVENT_HANDLED;
}
event_status_t PAUSE_startStop(protimer_t *const mobj, event_t const *const e)
{
    mobj->active_state = COUNTDOWN;
    return EVENT_TRANSITION;
}
event_status_t PAUSE_abort(protimer_t *const mobj, event_t const *const e)
{
    mobj->active_state = IDLE;
    return EVENT_TRANSITION;
}

event_status_t STAT_entry(protimer_t *const mobj, event_t const *const e)
{
    disp_msg("PRODUCTIVE TIME \t ");
    disp_time(mobj->pro_time);
    return EVENT_HANDLED;
}
event_status_t STAT_exit(protimer_t *const mobj, event_t const *const e)
{
    disp_clr();
    return EVENT_HANDLED;
}
event_status_t STAT_timeTick(protimer_t *const mobj, event_t const *const e)
{
    static uint8_t tick_count = 0;
    if( ++tick_count >= 30)
    {
        tick_count = 0;
        mobj->active_state = IDLE;
        return EVENT_TRANSITION;
    }
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
