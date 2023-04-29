#include <stdio.h>
#include "Agent.h"
#include "Field.h"
#include "Oled.h"
#include "Negotiation.h"
#include "BattleBoats.h"

static NegotiationData a, b, ah;
static BB_Event battleboatEvent;

int main() {
    char* states[] = {
        "AGENT_STATE_START",
        "AGENT_STATE_CHALLENGING",
        "AGENT_STATE_ACCEPTING",
        "AGENT_STATE_ATTACKING",
        "AGENT_STATE_DEFENDING",
        "AGENT_STATE_WAITING_TO_SEND",
        "AGENT_STATE_END_SCREEN",
        "AGENT_STATE_SETUP_BOATS"
    };
    char* MessageState[] = {
        "MESSAGE_NONE",
        "MESSAGE_CHA",
        "MESSAGE_ACC",
        "MESSAGE_REV",
        "MESSAGE_SHO",
        "MESSAGE_RES",

        "MESSAGE_ERROR"
    };
    BOARD_Init();
    OledInit();
    AgentInit();

    printf("\n********************************************************************\n");
    printf("*                           START AGENT TEST                       *\n");
    printf("********************************************************************\n");


    printf("----------------------------Test Start State-----------------------\n");
    Message message;
    AgentState currentState = AgentGetState();
    printf("Expect State: AGENT_STATE_START\nTest result: %s\n", states[currentState]);

    printf("----------------------------Test Set State-------------------------\n");
    AgentSetState(AGENT_STATE_DEFENDING);
    currentState = AgentGetState();
    printf("Expect State: AGENT_STATE_DEFENDING\nTest result: %s\n", states[currentState]);

    printf("---------------------------Test challenging-----------------------\n");
    AgentSetState(AGENT_STATE_START);
    battleboatEvent.type = BB_EVENT_START_BUTTON;
    message = AgentRun(battleboatEvent);
    currentState = AgentGetState();
    printf("Expect State: AGENT_STATE_CHALLENGING\nTest result: %s\n", states[currentState]);
    printf("Message.type = MESSAGE_CHA\nTest result: %s\n", MessageState[message.type]);

    printf("---------------------------Test cheat-----------------------------\n");
    AgentSetState(AGENT_STATE_START);
    battleboatEvent.type = BB_EVENT_CHA_RECEIVED;
    battleboatEvent.param0 = 12345;
    message = AgentRun(battleboatEvent);
    currentState = AgentGetState();
    printf("Expect State: AGENT_STATE_ACCEPTING\nTest result: %s\n", states[currentState]);
    printf("Message.type = MESSAGE_ACC\n Test result: %s\n", MessageState[message.type]);
    battleboatEvent.type = BB_EVENT_REV_RECEIVED;
    battleboatEvent.param0 = 61;
    message = AgentRun(battleboatEvent);
    currentState = AgentGetState();
    printf("Expect State: AGENT_STATE_END_SCREEN\nTest result: %s\n", states[currentState]);
    AgentInit();
    currentState = AgentGetState();
    printf("Expect State: AGENT_STATE_START\nTest result: %s\n", states[currentState]);

    printf("----------------------Test attack->defending----------------------\n");

    battleboatEvent.type = BB_EVENT_RES_RECEIVED;
    AgentSetState(AGENT_STATE_ATTACKING);
    battleboatEvent.param2 = RESULT_HUGE_BOAT_SUNK;
    message = AgentRun(battleboatEvent);
    currentState = AgentGetState();
    printf("Expect State: AGENT_STATE_DEFENDING\nTest result: %s\n", states[currentState]);
    printf("---------------------------Test win-----------------------------\n");
    AgentSetState(AGENT_STATE_ATTACKING);
    battleboatEvent.param2 = RESULT_SMALL_BOAT_SUNK;
    message = AgentRun(battleboatEvent);
    AgentSetState(AGENT_STATE_ATTACKING);
    battleboatEvent.param2 = RESULT_MEDIUM_BOAT_SUNK;
    message = AgentRun(battleboatEvent);
    AgentSetState(AGENT_STATE_ATTACKING);
    battleboatEvent.param2 = RESULT_LARGE_BOAT_SUNK;
    message = AgentRun(battleboatEvent);
    currentState = AgentGetState();
    printf("Expect State: AGENT_STATE_END_SCREEN\nTest result: %s\n", states[currentState]);

    printf("---------------------------Test defending->wait to send-----------\n");
    AgentInit();
    AgentSetState(AGENT_STATE_DEFENDING);
    battleboatEvent.type = BB_EVENT_SHO_RECEIVED;
    battleboatEvent.param2 = RESULT_LARGE_BOAT_SUNK;
    message = AgentRun(battleboatEvent);
    currentState = AgentGetState();
    printf("Expect State: AGENT_STATE_WAITING_TO_SEND\nTest result: %s\n", states[currentState]);

    printf("--------------------------Test Lose------------------------------\n");

    AgentInit();
    int i;
    for (i = 0; i < FIELD_ROWS * FIELD_COLS; i++) {
        AgentSetState(AGENT_STATE_DEFENDING);
        battleboatEvent.type = BB_EVENT_SHO_RECEIVED;
        battleboatEvent.param0 = i / FIELD_COLS;
        battleboatEvent.param1 = i % FIELD_COLS;
        message = AgentRun(battleboatEvent);
    }
    currentState = AgentGetState();
    printf("Expect State: AGENT_STATE_END_SCREEN\nTest result: %s\n", states[currentState]);


    printf("If No difference between expects and results, Tests Passed\n");
    while (TRUE);
}