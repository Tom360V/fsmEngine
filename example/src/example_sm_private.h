/*
 * example_sm_private.h
 *
 * Created: Thursday 31. August 2017 - 10:38:50
 *  Author: Tom
 */ 
#ifndef EXAMPLE_SM_PRIVATE_H_
#define EXAMPLE_SM_PRIVATE_H_

#include <stdio.h>
#include "fsm.h"

/******************************************************************************
 * STATEMACHINE EVENTS
 */
typedef enum
{
    example_ebuttonPressed = 0,
    example_eReset,
    example_eAbort
}example_events_t;

/******************************************************************************
 * STATEMACHINE STATES
 */
typedef enum
{
    example_eStatebla = 0,
    example_eStateblabla,
    example_eStatekies1,
}example_states_t;

/******************************************************************************
 * STATEMACHINE GUARDS
 */
int8_t example_guard1(void *p);
int8_t example_guard2(void *p);
int8_t example_guard3(void *p);
int8_t example_guardsChoice1(void *p);
int8_t example_guardsChoice2(void *p);

 /******************************************************************************
 * STATEMACHINE ACTIONS
 */
void example_action1(void *p);
void example_action2(void *p);
void example_action3(void *p);
 
 /******************************************************************************
 * STATEMACHINE TABLE
 */
static const FSMTransition_t example_Statebla[] = {
	{	example_ebuttonPressed, 	example_eStatekies1,
		{2, (FSM_fpGuard_t []){example_guard1, example_guard3} },
		{2, (FSM_fpAction_t[]){example_action1, example_action3} }
	},
	{	example_eAbort, 	example_eStateblabla,
		{1, (FSM_fpGuard_t []){example_guard2} },
		{1, (FSM_fpAction_t[]){example_action2} }
	},
	{	example_eReset, 	example_eStatebla,
		{1, (FSM_fpGuard_t []){example_guard2} },
		{1, (FSM_fpAction_t[]){example_action2} }
	},
};

static const FSMTransition_t example_Stateblabla[] = {
	{	example_eReset, 	example_eStatebla,
		{1, (FSM_fpGuard_t []){example_guard3} },
		{1, (FSM_fpAction_t[]){example_action3} }
	},
};

static const FSMTransition_t example_Statekies1[] = {
	{	example_eStatebla,
		{1, (FSM_fpGuard_t []){example_guardsChoice1} },
		{2, (FSM_fpAction_t[]){example_action1, example_action2} }
	},
	{	example_eStateblabla,
		{1, (FSM_fpGuard_t []){example_guardsChoice2} },
		{1, (FSM_fpAction_t[]){example_action2} }
	},
};



/******************************************************************************
 * STATEMACHINE TABLE
 */
static const FSMState_t example_table[] = { FSM_ADDSTATE(example_Statebla), FSM_ADDSTATE(example_Stateblabla), FSM_ADDSTATE(example_Statekies1) };
static FSMTable_t example_tableData = {example_eStatebla, FSM_ADDTABLE(example_table)};

/******************************************************************************
 * PRIVATE VARIABLE
 */
static tableIdx_t example_tableIdx;

/******************************************************************************
 * FUNCTIONS
 */
static inline void example_statemachine_init(void)
{
    example_tableIdx = FSM_Add(&example_tableData, NULL /*void *pData*/);
}

static inline void example_Event(example_events_t event)
{
    FSM_AddEvent(example_tableIdx, event);
}

#endif /* EXAMPLE_SM_PRIVATE_H_ */