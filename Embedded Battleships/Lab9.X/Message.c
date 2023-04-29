#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Message.h"
#include "BOARD.h"

typedef enum {
    WAITING, RECORDINGPAYLOAD, RECORDINGCHECKSUM
} DecodeMode;

static int State = WAITING;
static int payloadLength = 0;
static char payloadTemp[MESSAGE_MAX_PAYLOAD_LEN + 1];
static char checksumTemp[MESSAGE_CHECKSUM_LEN + 2];
static char Temp[MESSAGE_MAX_PAYLOAD_LEN];
static int checksumLength = 0;

uint8_t Message_CalculateChecksum(const char* payload) {
    int Length = strlen(payload);
    uint8_t result = 0;
    int i;
    for (i = 0; i < Length; i++) {
        result ^= payload[i];
    }
    return result;
}

int Message_ParseMessage(const char* payload,
        const char* checksum_string, BB_Event * message_event) {
    char payloadCheck[MESSAGE_CHECKSUM_LEN + 1];
    uint8_t result = Message_CalculateChecksum(payload);
    sprintf(payloadCheck, "%02X", result);
    payloadCheck[MESSAGE_CHECKSUM_LEN] = '\0';
    if (strcmp(payloadCheck, checksum_string)) {

        message_event->type = BB_EVENT_ERROR;
        return STANDARD_ERROR;
    }
    if (strlen(checksum_string) != MESSAGE_CHECKSUM_LEN) { // the checksum string is not two characters long
        message_event->type = BB_EVENT_ERROR;
        return STANDARD_ERROR;
    }
    if (strlen(payload) > MESSAGE_MAX_PAYLOAD_LEN) {
        message_event->type = BB_EVENT_ERROR;
        return STANDARD_ERROR;
    }
    char Temp[strlen(payload) + 1];
    strcpy(Temp, payload);

    char *messageId = strtok(Temp, ",");
    unsigned int paramOne, paramTwo, paramThree;
    if (strcmp(messageId, "CHA") == 0) {
        message_event->type = BB_EVENT_CHA_RECEIVED;
        sscanf(payload, PAYLOAD_TEMPLATE_CHA, &paramOne);
        message_event->param0 = paramOne;
    } else if (strcmp(messageId, "ACC") == 0) {
        message_event->type = BB_EVENT_ACC_RECEIVED;
        sscanf(payload, PAYLOAD_TEMPLATE_ACC, &paramOne);
        message_event->param0 = paramOne;
    } else if (strcmp(messageId, "REV") == 0) {
        message_event->type = BB_EVENT_REV_RECEIVED;
        sscanf(payload, PAYLOAD_TEMPLATE_REV, &paramOne);
        message_event->param0 = paramOne;
    } else if (strcmp(messageId, "SHO") == 0) {
        message_event->type = BB_EVENT_SHO_RECEIVED;
        sscanf(payload, PAYLOAD_TEMPLATE_SHO, &paramOne, &paramTwo);
        message_event->param0 = paramOne;
        message_event->param1 = paramTwo;
    } else if (strcmp(messageId, "RES") == 0) {
        message_event->type = BB_EVENT_RES_RECEIVED;
        sscanf(payload, PAYLOAD_TEMPLATE_RES, &paramOne, &paramTwo, &paramThree);
        message_event->param0 = paramOne;
        message_event->param1 = paramTwo;
        message_event->param2 = paramThree;
    } else {
        message_event->type = BB_EVENT_ERROR;
        return STANDARD_ERROR;
    }
    return SUCCESS;
}

int Message_Encode(char *message_string, Message message_to_encode) {

    uint8_t check;
    if (message_to_encode.type == MESSAGE_NONE) {
        return FALSE;
    } else if (message_to_encode.type == MESSAGE_CHA) {
        sprintf(Temp, PAYLOAD_TEMPLATE_CHA, message_to_encode.param0);
        check = Message_CalculateChecksum(Temp);
        sprintf(message_string, MESSAGE_TEMPLATE, Temp, check);
    } else if (message_to_encode.type == MESSAGE_ACC) {
        sprintf(Temp, PAYLOAD_TEMPLATE_ACC, message_to_encode.param0);
        check = Message_CalculateChecksum(Temp);
        sprintf(message_string, MESSAGE_TEMPLATE, Temp, check);
    } else if (message_to_encode.type == MESSAGE_REV) {
        sprintf(Temp, PAYLOAD_TEMPLATE_REV, message_to_encode.param0);
        check = Message_CalculateChecksum(Temp);
        sprintf(message_string, MESSAGE_TEMPLATE, Temp, check);
    } else if (message_to_encode.type == MESSAGE_SHO) {
        sprintf(Temp, PAYLOAD_TEMPLATE_SHO, message_to_encode.param0, message_to_encode.param1);
        check = Message_CalculateChecksum(Temp);
        sprintf(message_string, MESSAGE_TEMPLATE, Temp, check);
    } else if (message_to_encode.type == MESSAGE_RES) {
        sprintf(Temp, PAYLOAD_TEMPLATE_RES, message_to_encode.param0, message_to_encode.param1, message_to_encode.param2);
        check = Message_CalculateChecksum(Temp);
        sprintf(message_string, MESSAGE_TEMPLATE, Temp, check);
    } else if (message_to_encode.type == MESSAGE_ERROR) {
        printf("MESSAGE_ERROR\n");
        return FALSE;
    }
    return strlen(message_string);
}

int Message_Decode(unsigned char char_in, BB_Event * decoded_message_event) {
    if (State == WAITING) {
        if (char_in == '$') {
            State = RECORDINGPAYLOAD;
            payloadLength = 0;
        }
        decoded_message_event->type = MESSAGE_NONE;
    } else if (State == RECORDINGPAYLOAD) {
        if (char_in == '$' || char_in == '\n') {
            State = WAITING;
            decoded_message_event->type = BB_EVENT_ERROR;
            return STANDARD_ERROR;
        }
        if (payloadLength > MESSAGE_MAX_PAYLOAD_LEN) {
            State = WAITING;
            decoded_message_event->type = BB_EVENT_ERROR;
            return STANDARD_ERROR;
        }
        if (char_in == '*') {
            State = RECORDINGCHECKSUM;
            payloadTemp[payloadLength] = '\0';
            checksumLength = 0;
        } else {
            payloadTemp[payloadLength] = char_in;
            payloadLength++;
        }
        decoded_message_event->type = MESSAGE_NONE;
    } else if (State == RECORDINGCHECKSUM) {
        if (checksumLength == MESSAGE_CHECKSUM_LEN) {
            if (char_in != '\n') {
                decoded_message_event->type = BB_EVENT_ERROR;
                State = WAITING;
                return STANDARD_ERROR;
            }
        }
        if (char_in == '\n') {
            if (checksumLength != MESSAGE_CHECKSUM_LEN) {

                decoded_message_event->type = BB_EVENT_ERROR;
                State = WAITING;
                return STANDARD_ERROR;
            }

            checksumTemp[checksumLength + 1] = '\0';
            //checksumTemp[checksumLength] = '\0';
            int check = Message_ParseMessage(payloadTemp, checksumTemp, decoded_message_event);
            if (check) {
                State = WAITING;
                return SUCCESS;

            } else {

                State = WAITING;
                decoded_message_event->type = BB_EVENT_ERROR;
                return STANDARD_ERROR;
            }
        }
        if ('0' <= char_in <= '9' || 'A' <= char_in <= 'F') {
            checksumTemp[checksumLength] = char_in;
            checksumLength++;
        } else {

            State = WAITING;
            decoded_message_event->type = BB_EVENT_ERROR;
            return STANDARD_ERROR;
        }
        decoded_message_event->type = MESSAGE_NONE;
    }
    return SUCCESS;
}