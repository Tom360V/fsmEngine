/*
 * fsm.c
 *
 *  Created on: Oct 30, 2015
 *      Author: tom
 */

#ifndef FSM_H_
#define FSM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

/******************************************************************************
 * MACROS
 */
#define FSM_ADDSTATE(s) {(FSMTransition_t*)(s), sizeof(s)/sizeof(FSMTransition_t)}
#define FSM_ADDTABLE(t) (FSMState_t*)&(t), sizeof(t)/sizeof(FSMState_t)

#define FSM_FALSE   (0)
#define FSM_TRUE    (1)

/******************************************************************************
 * TYPES
 */
typedef void (* FSM_fpAction_t)(void *p);
typedef int8_t (* FSM_fpGuard_t)(void *p);

typedef int event_t;

typedef int tableIdx_t;

typedef struct
{
    int8_t nofActions;
    FSM_fpAction_t *action;
} FSMAction_t;

typedef struct
{
    int8_t nofGuards;
    FSM_fpGuard_t *guard;
} FSMGuard_t;

typedef struct
{
    event_t eventId;
    int nextState;
//    FSMState_t *nextState;

    FSMGuard_t  guards;
    FSMAction_t actions;
} FSMTransition_t;

typedef struct
{
    FSMTransition_t *transitions;
    int numberOfTransitions;
} FSMState_t;

typedef struct
{
    FSM_fpAction_t Init;
    FSMState_t *table;
    int numberOfStates;
} FSMTable_t;

/******************************************************************************
 * FUNCTIONS
 */
void        FSM_Init(void);
tableIdx_t  FSM_Add(FSMTable_t *fp, void *pData);
int         FSM_AddEvent(tableIdx_t tableIdx, event_t event);
void        FSM_Feed(void);

#ifdef __cplusplus
}
#endif

#endif /* FSM_H_ */
