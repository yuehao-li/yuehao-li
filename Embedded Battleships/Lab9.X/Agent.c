#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Agent.h"
#include "Oled.h"
#include "Negotiation.h"
#include "Field.h"
#include "FieldOled.h"
static NegotiationData a, b;
static NegotiationData aH;
static Field own_field, op_field;
static char agentStr[150];
static AgentState State;
static int turnCounter;

void AgentInit(void) {
    State = AGENT_STATE_START;
    turnCounter = 0;
    a = b = 0;
    OledClear(OLED_COLOR_BLACK);
    OledDrawString("New Game? \nPress BTN4 to\nChallenge, or wait\nfor opponent.");
    OledUpdate();
}

Message AgentRun(BB_Event event) {
    Message message;
    message.type = MESSAGE_NONE;
    if (event.type == BB_EVENT_RESET_BUTTON) {
        AgentInit();
        return message;
    }
    switch(State){
        case AGENT_STATE_START:
            if (event.type == BB_EVENT_CHA_RECEIVED) {
                // Change state
                State = AGENT_STATE_ACCEPTING;
                b = rand();
                aH = event.param0;
                message.type = MESSAGE_ACC;
                message.param0 = b;
                FieldInit(&own_field, &op_field);
                FieldAIPlaceAllBoats(&own_field);
                OledClear(OLED_COLOR_BLACK);
                sprintf(agentStr, "ACCEPTING\n  %d = hash_a\n %d = B\n", aH, b);
                OledDrawString(agentStr);
                OledUpdate();
            } else if (event.type == BB_EVENT_START_BUTTON) {
                // Change state
                State = AGENT_STATE_CHALLENGING;
                // A and #a
                a = rand();
                aH = NegotiationHash(a);
                message.type = MESSAGE_CHA;
                message.param0 = aH;
                // Init fields
                FieldInit(&own_field, &op_field);
                // Place boats
                FieldAIPlaceAllBoats(&own_field);
                OledClear(OLED_COLOR_BLACK);
                sprintf(agentStr, "CHALLENGING\n  %d = A\n %d = hash_a\n", a, aH);
                OledDrawString(agentStr);
                OledUpdate();
            }
            break;
        case AGENT_STATE_CHALLENGING:
            if (event.type == BB_EVENT_ACC_RECEIVED) {
                // Receive random number send from B;
                b = event.param0;

                if (NegotiateCoinFlip(a, event.param0) == HEADS)
                    State = AGENT_STATE_WAITING_TO_SEND;
                else
                    State = AGENT_STATE_DEFENDING;
                FieldOledDrawScreen(&own_field, &op_field, FIELD_OLED_TURN_THEIRS, turnCounter);
                message.param0 = a;
                message.type = MESSAGE_REV;
            }
            break;
        case AGENT_STATE_ACCEPTING:
            if (event.type == BB_EVENT_REV_RECEIVED) {
                // Receive random number send from A;
                a = event.param0;

                if (!NegotiationVerify(a, aH)) {
                    OledClear(OLED_COLOR_BLACK);
                    OledDrawString("Cheater!");
                    OledUpdate();
                    State = AGENT_STATE_END_SCREEN;
                    return message;
                }
                if (NegotiateCoinFlip(a, b) == HEADS) {
                    State = AGENT_STATE_DEFENDING;
                    FieldOledDrawScreen(&own_field, &op_field, FIELD_OLED_TURN_THEIRS, turnCounter);
                }
                else {
                    State = AGENT_STATE_ATTACKING;
                    FieldOledDrawScreen(&own_field, &op_field, FIELD_OLED_TURN_MINE, turnCounter);

                    GuessData G = FieldAIDecideGuess(&op_field);
                    message.type = MESSAGE_SHO;
                    message.param0 = G.row;
                    message.param1 = G.col;
                }
            }
            break;
        case AGENT_STATE_ATTACKING:
            if (event.type == BB_EVENT_RES_RECEIVED) {
                GuessData G;
                G.row = event.param0;
                G.col = event.param1;
                G.result = event.param2;
                FieldUpdateKnowledge(&op_field, &G);
                if (op_field.hugeBoatLives == 0 && op_field.largeBoatLives == 0 &&
                        op_field.mediumBoatLives == 0 && op_field.smallBoatLives == 0) {
                    OledClear(OLED_COLOR_BLACK);
                    OledDrawString("You WIN!!");
                    OledUpdate();
                    State = AGENT_STATE_END_SCREEN;
                    return message;
                } else {
                    FieldOledDrawScreen(&own_field, &op_field, FIELD_OLED_TURN_MINE, turnCounter);
                    State = AGENT_STATE_DEFENDING;

                }
            }
            break;
        case AGENT_STATE_DEFENDING:
            if (event.type == BB_EVENT_SHO_RECEIVED) {
                GuessData F;
                F.col = event.param1;
                F.row = event.param0;
                F.result = event.param2;
                FieldRegisterEnemyAttack(&own_field, &F);

                message.type = MESSAGE_RES;
                message.param0 = F.row;
                message.param1 = F.col;
                message.param2 = F.result;
                if (own_field.smallBoatLives == 0 && own_field.mediumBoatLives == 0 &&
                        own_field.largeBoatLives == 0 && own_field.hugeBoatLives == 0) {
                    OledClear(OLED_COLOR_BLACK);
                    OledDrawString("You lose the Game! :(");
                    OledUpdate();
                    State = AGENT_STATE_END_SCREEN;

                    return message;
                } else {
                    FieldOledDrawScreen(&own_field, &op_field, FIELD_OLED_TURN_THEIRS, turnCounter);
                    State = AGENT_STATE_WAITING_TO_SEND;
                }
            }
            break;
        case AGENT_STATE_WAITING_TO_SEND:
            if (event.type == BB_EVENT_MESSAGE_SENT) {
                turnCounter++;
                // Decide Guess send to SHO
                GuessData F = FieldAIDecideGuess(&op_field);
                message.param0 = F.row;
                message.param1 = F.col;
                message.type = MESSAGE_SHO;
                State = AGENT_STATE_ATTACKING;
            }
            break;
        case AGENT_STATE_END_SCREEN:
            break;
        default:
            message.type == MESSAGE_ERROR;
    }
    return message;

    // ---------------------------------------------
    // If version, Use switch to make program faster
    // ---------------------------------------------
    /*
    if (State == AGENT_STATE_START) {
        if (event.type == BB_EVENT_CHA_RECEIVED) {
            // Change state
            State = AGENT_STATE_ACCEPTING;
            b = rand();
            aH = event.param0;
            message.type = MESSAGE_ACC;
            message.param0 = b;
            FieldInit(&own_field, &op_field);
            FieldAIPlaceAllBoats(&own_field);
            OledClear(OLED_COLOR_BLACK);
            sprintf(agentStr, "ACCEPTING\n  %d = hash_a\n %d = B\n", aH, b);
            OledDrawString(agentStr);
            OledUpdate();
        } else if (event.type == BB_EVENT_START_BUTTON) {
            // Change state
            State = AGENT_STATE_CHALLENGING;
            // A and #a
            a = rand();
            aH = NegotiationHash(a);
            message.type = MESSAGE_CHA;
            message.param0 = aH;
            // Init fields
            FieldInit(&own_field, &op_field);
            // Place boats
            FieldAIPlaceAllBoats(&own_field);
            OledClear(OLED_COLOR_BLACK);
            sprintf(agentStr, "CHALLENGING\n  %d = A\n %d = hash_a\n", a, aH);
            OledDrawString(agentStr);
            OledUpdate();
        }
    } else if (State == AGENT_STATE_CHALLENGING) {
        if (event.type == BB_EVENT_ACC_RECEIVED) {
            // Receive random number send from B;
            b = event.param0;

            if (NegotiateCoinFlip(a, event.param0) == HEADS)
                State = AGENT_STATE_WAITING_TO_SEND;
            else
                State = AGENT_STATE_DEFENDING;
            FieldOledDrawScreen(&own_field, &op_field, FIELD_OLED_TURN_THEIRS, turnCounter);
            message.param0 = a;
            message.type = MESSAGE_REV;
        }
    } else if (State == AGENT_STATE_ACCEPTING) {
        if (event.type == BB_EVENT_REV_RECEIVED) {
            // Receive random number send from A;
            a = event.param0;

            if (!NegotiationVerify(a, aH)) {
                OledClear(OLED_COLOR_BLACK);
                OledDrawString("Cheater!");
                OledUpdate();
                State = AGENT_STATE_END_SCREEN;
                return message;
            }
            if (NegotiateCoinFlip(a, b) == HEADS) {
                State = AGENT_STATE_DEFENDING;
                FieldOledDrawScreen(&own_field, &op_field, FIELD_OLED_TURN_THEIRS, turnCounter);
            }
            else {
                State = AGENT_STATE_ATTACKING;
                FieldOledDrawScreen(&own_field, &op_field, FIELD_OLED_TURN_MINE, turnCounter);

                GuessData G = FieldAIDecideGuess(&op_field);
                message.type = MESSAGE_SHO;
                message.param0 = G.row;
                message.param1 = G.col;

            }


        }
    } else if (State == AGENT_STATE_ATTACKING) {
        if (event.type == BB_EVENT_RES_RECEIVED) {
            GuessData G;
            G.row = event.param0;
            G.col = event.param1;
            G.result = event.param2;
            FieldUpdateKnowledge(&op_field, &G);
            if (op_field.hugeBoatLives == 0 && op_field.largeBoatLives == 0 &&
                    op_field.mediumBoatLives == 0 && op_field.smallBoatLives == 0) {
                OledClear(OLED_COLOR_BLACK);
                OledDrawString("You WIN!!");
                OledUpdate();
                State = AGENT_STATE_END_SCREEN;
                return message;
            } else {
                FieldOledDrawScreen(&own_field, &op_field, FIELD_OLED_TURN_MINE, turnCounter);
                State = AGENT_STATE_DEFENDING;

            }
        }
    } else if (State == AGENT_STATE_DEFENDING) {
        if (event.type == BB_EVENT_SHO_RECEIVED) {
            GuessData F;
            F.col = event.param1;
            F.row = event.param0;
            F.result = event.param2;
            FieldRegisterEnemyAttack(&own_field, &F);

            message.type = MESSAGE_RES;
            message.param0 = F.row;
            message.param1 = F.col;
            message.param2 = F.result;
            if (own_field.smallBoatLives == 0 && own_field.mediumBoatLives == 0 &&
                    own_field.largeBoatLives == 0 && own_field.hugeBoatLives == 0) {
                OledClear(OLED_COLOR_BLACK);
                OledDrawString("You lose the Game! :(");
                OledUpdate();
                State = AGENT_STATE_END_SCREEN;

                return message;
            } else {
                FieldOledDrawScreen(&own_field, &op_field, FIELD_OLED_TURN_THEIRS, turnCounter);
                State = AGENT_STATE_WAITING_TO_SEND;

            }
        }
    }        // ----------------------------------
    else if (State == AGENT_STATE_WAITING_TO_SEND) {
        if (event.type == BB_EVENT_MESSAGE_SENT) {
            turnCounter++;
            // Decide Guess send to SHO
            GuessData F = FieldAIDecideGuess(&op_field);
            message.param0 = F.row;
            message.param1 = F.col;
            message.type = MESSAGE_SHO;
            State = AGENT_STATE_ATTACKING;
        }
    } else if (State == AGENT_STATE_END_SCREEN) {
    } else {
        message.type == MESSAGE_ERROR;
    } */
}

AgentState AgentGetState(void) {
    return State;
}

void AgentSetState(AgentState newState) {
    State = newState;
}