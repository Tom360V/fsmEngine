/*
 * [filename]_sm_private.h
 *
 * Created: [DATE] - [TIME]
 *  Author: Tom
 */ 
#ifndef [filename_capital]_SM_PRIVATE_H_
#define [filename_capital]_SM_PRIVATE_H_

#include <stdio.h>
#include "fsm.h"

/******************************************************************************
 * STATEMACHINE EVENTS
 */
typedef enum
{
[EVENTS]
}[filename]_events_t;

/******************************************************************************
 * STATEMACHINE STATES
 */
typedef enum
{
[STATES]
}[filename]_states_t;

/******************************************************************************
 * STATEMACHINE GUARDS
 */
[GUARDS]

 /******************************************************************************
 * STATEMACHINE ACTIONS
 */
[ACTIONS]
 
 /******************************************************************************
 * STATEMACHINE TABLE
 */
[STATETABLES]

/******************************************************************************
 * STATEMACHINE TABLE
 */
static const FSMState_t [filename]_table[] = { [ADD_STATES] };
static FSMTable_t [filename]_tableData = {[STARTSTATE], FSM_ADDTABLE([filename]_table)};

/******************************************************************************
 * PRIVATE VARIABLE
 */
static tableIdx_t [filename]_tableIdx;

/******************************************************************************
 * FUNCTIONS
 */
static inline void [filename]_statemachine_init(void)
{
    [filename]_tableIdx = FSM_Add(&[filename]_tableData, NULL /*void *pData*/);
}

static inline void [filename]_Event([filename]_events_t event)
{
    FSM_AddEvent([filename]_tableIdx, event);
}

#endif /* [filename_capital]_SM_PRIVATE_H_ */