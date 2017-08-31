/*
 * example_sm.c
 *
 * Created: Thursday 31. August 2017 - 10:38:50
 *  Author: Tom
 */ 
#include <stdint.h>
#include "fsm.h"
#include "example_sm.h"
#include "example_sm_private.h"

/******************************************************************************
 * LOCAL VARIABLE AND STRUCTS
 */

/*******************************************************************************
 * GUARD
 */
int8_t example_guard1(void *p)
{
    (void)p;
    //enter your code here
    return FSM_FALSE;
}

/*******************************************************************************
 * GUARD
 */
int8_t example_guard2(void *p)
{
    (void)p;
    //enter your code here
    return FSM_FALSE;
}

/*******************************************************************************
 * GUARD
 */
int8_t example_guard3(void *p)
{
    (void)p;
    //enter your code here
    return FSM_FALSE;
}

/*******************************************************************************
 * GUARD
 */
int8_t example_guardsChoice1(void *p)
{
    (void)p;
    //enter your code here
    return FSM_FALSE;
}

/*******************************************************************************
 * GUARD
 */
int8_t example_guardsChoice2(void *p)
{
    (void)p;
    //enter your code here
    return FSM_FALSE;
}


/*******************************************************************************
 * ACTION
 */
void example_action1(void *p)
{
    (void)p;
    //enter your code here
}

/*******************************************************************************
 * ACTION
 */
void example_action2(void *p)
{
    (void)p;
    //enter your code here
}

/*******************************************************************************
 * ACTION
 */
void example_action3(void *p)
{
    (void)p;
    //enter your code here
}


/******************************************************************************
 * API
 */
void example_Init()
{
    example_statemachine_init();
    
    /* EXAMPLE, Committing an Event:
     *     example_Event([Event]);
     *
     * Events:
     *   - example_ebuttonPressed,
     *   - example_eReset,
     *   - example_eAbort
     *
     */
}
