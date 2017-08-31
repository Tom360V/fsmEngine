/*
 * fsm.c
 *
 *  Created on: Oct 30, 2015
 *      Author: tom
 */
#include <stdio.h>
#include <stdint.h>
#include "fsm.h"
#include "fifo.h"

#define NOF_STATEMACHINES   (5)
#define NOF_EVENTS          (10)

#define enum2str(x) #x


#if !DEBUG
    static inline void doNothing(const char *x, ...)
    {
        (void)x;
    }
    #define DEBUGPRINT      //
#else
    #define DEBUGPRINT      printf
#endif

/******************************************************************************
 * LOCAL Typedefs
 */
typedef struct FSMData_t
{
    int currentState;
    void *pData;
    FSMTable_t *pTable;
} FSMData_t;

typedef struct
{
    tableIdx_t tableIdx;
    int event;
} eventItem_t;

/******************************************************************************
 * LOCAL VARIABLE
 */
fifo_t lFiFo;
eventItem_t eventItemList[NOF_EVENTS];

FSMData_t lFSMs[NOF_STATEMACHINES];
static int8_t fsm_initialized = -1;

/******************************************************************************
 * PRIVATE Functions
 */
static inline int8_t _FSM_CheckGuards(FSMTransition_t *transitions, void *pData)
{
    //Check Guards
    int8_t guardResult = FSM_TRUE;
    FSM_fpGuard_t *guard = transitions->guards.guard;
    int8_t guardCnt = transitions->guards.nofGuards;
    while((guardCnt--) > 0)
    {
        if(NULL != guard)
        {
            DEBUGPRINT("\n\t\tGuard: %d", transitions->guards.nofGuards-guardCnt-1);
            guardResult = (*guard)(pData);
            DEBUGPRINT(", pass: %d", guardResult);

            if(guardResult==FSM_FALSE)
            {
                guardCnt = -1;
                break;
            } 
        }
        guard++;
    }
    return guardResult;
}

/******************************************************************************
 * Functions
 */
void FSM_Init(void)
{
    int i;

    FIFO_Init(&lFiFo, &eventItemList, sizeof(eventItem_t), NOF_EVENTS);

    for(i=NOF_STATEMACHINES-1; 0<=i; i--)
    {
        lFSMs[i].pTable    = NULL;
        lFSMs[i].pData     = NULL;
        lFSMs[i].currentState = 0;
    }
    fsm_initialized = 1;
}

tableIdx_t FSM_Add(FSMTable_t *fp, void *pData)
{
    if(fsm_initialized>=1)
    {
        tableIdx_t i;
        for(i=NOF_STATEMACHINES-1; 0<=i; i--)
        {
            if(NULL == lFSMs[i].pTable)
            {
                lFSMs[i].pTable = fp;
                lFSMs[i].pData = pData;

                //Run Init function
                if(NULL != lFSMs[i].pTable->Init)
                {
                    (lFSMs[i].pTable->Init)(lFSMs[i].pData);
                }
                break;
            }
        }
        return i;
    }
    return -1;
}

int FSM_AddEvent(tableIdx_t tableIdx, event_t event)
{
    eventItem_t myEvent;

    if(NULL == lFSMs[tableIdx].pTable)
    {
        return -1;
    }

    myEvent.tableIdx = tableIdx;
    myEvent.event = event;
    if(0 > FIFO_AddItem(&lFiFo, &myEvent))
    {
        return -1;
    }
    return (int)tableIdx;
}


void FSM_Feed(void)
{
    int8_t ret;
    eventItem_t myEvent = {0};

    ret = FIFO_GetItem(&lFiFo, &myEvent);
    if(ret >= 0)
    {
        if(0 > myEvent.tableIdx)
        {
            // No Data
            return;
        }

        int idx;
        FSMTable_t *table;
        FSMState_t *state;
        FSMTransition_t *transitions;

        // Goto current state and ceck all transitions
        table = lFSMs[myEvent.tableIdx].pTable;
        state = table->table;
        state += lFSMs[myEvent.tableIdx].currentState;

        DEBUGPRINT("\nState:%d", lFSMs[myEvent.tableIdx].currentState);
        transitions = state->transitions;
        for(idx=0; idx<state->numberOfTransitions; idx++)
        {
            DEBUGPRINT("\n\tTransition: %d", idx);

            if(transitions->eventId == myEvent.event) //Match Event
            {
                //Check Guards
                int8_t guardResult = _FSM_CheckGuards(transitions, lFSMs[myEvent.tableIdx].pData);

                // Execute action, if guard is valid & available
                if(guardResult == FSM_TRUE)
                {
                    FSM_fpAction_t *action= transitions->actions.action;
                    int8_t actionCnt = transitions->actions.nofActions;
                    while((actionCnt--) > 0)
                    {
                        if(NULL != action)
                        {
                            DEBUGPRINT("\n\t\tAction: %d", transitions->actions.nofActions-actionCnt-1);
                            (*action)(lFSMs[myEvent.tableIdx].pData);
                        }
                        else
                        {
                            DEBUGPRINT("\n\t\tAction: None defined");
                        }
                        action++;
                    }
                    DEBUGPRINT("\n\tChange state: %d -> %d", lFSMs[myEvent.tableIdx].currentState, transitions->nextState);
                    lFSMs[myEvent.tableIdx].currentState = transitions->nextState;
                    break;
                }
            }
            transitions++;
        }
    }
}
