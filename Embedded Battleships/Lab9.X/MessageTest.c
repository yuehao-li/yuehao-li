#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Message.h"
#include "BOARD.h"
#include "BattleBoats.h"
static Message messageTest;
static BB_Event TestStruct;

int main(int argc, char* argv[]) {
    char* TestCHA = "CHA,2";
    char* TestACC = "ACC,3";
    char* TestREV = "REV,4";
    char* TestSHO = "SHO,2,9";
    char* TestRES = "RES,7,8,9";

    char SumStr[20];
    uint8_t Checksum;
    // ----------------------- Checksum Test-------------------------------
    printf("----------------------- Checksum Test-------------------------------\n");
    Checksum = Message_CalculateChecksum(TestCHA);
    printf("\"CHA,2\" Checksum: %02X\n", Checksum);
    Checksum = Message_CalculateChecksum(TestACC);
    printf("\"ACC,3\" Checksum: %02X\n", Checksum);
    Checksum = Message_CalculateChecksum(TestREV);
    printf("\"REV,4\" Checksum: %02X\n", Checksum);
    Checksum = Message_CalculateChecksum(TestSHO);
    printf("\"SHO,2,9\" Checksum: %02X\n", Checksum);
    Checksum = Message_CalculateChecksum(TestRES);
    printf("\"%s\" Checksum: %02X\n", TestRES, Checksum);
    // ---------------------------ParseMessage Test----------------------------
    printf("---------------------------ParseMessage Test----------------------------\n");
    sprintf(SumStr, "%02X", Message_CalculateChecksum(TestREV));
    SumStr[2] = '\0';
    Message_ParseMessage(TestREV, SumStr, &TestStruct);
    printf("Result expect: 4 0 0, Type: 5\n");
    printf("Param0: %d\nParam1: %d\nParam2: %d\n", TestStruct.param0, TestStruct.param1, TestStruct.param2);
    printf("Type: %d\n", TestStruct.type);

    sprintf(SumStr, "%02X", Message_CalculateChecksum(TestRES));
    SumStr[2] = '\0';
    Message_ParseMessage(TestRES, SumStr, &TestStruct);
    printf("Result expect: 7 8 9, Type: 7\n");
    printf("Param0: %d\nParam1: %d\nParam2: %d\n", TestStruct.param0, TestStruct.param1, TestStruct.param2);
    printf("Type: %d\n", TestStruct.type);

    sprintf(SumStr, "%02X", Message_CalculateChecksum(TestSHO));
    SumStr[2] = '\0';
    Message_ParseMessage(TestSHO, SumStr, &TestStruct);
    printf("Result expect: 2 9 0, Type: 6\n");
    printf("Param0: %d\nParam1: %d\nParam2: %d\n", TestStruct.param0, TestStruct.param1, TestStruct.param2);
    printf("Type: %d\n", TestStruct.type);
    printf("----------------------------Decode Test-----------------------------\n");
    if (!Message_Decode('*', &TestStruct))
        printf("ERROR!\n");
    if (!Message_Decode('$', &TestStruct))
        printf("ERROR!\n");
    if (!Message_Decode('E', &TestStruct))
        printf("ERROR!\n");
    if (!Message_Decode('!', &TestStruct))
        printf("ERROR!\n");
    if (!Message_Decode('$', &TestStruct))
        printf("ERROR!\n");
    printf("If No Error, Decoder has bugs\n");
    if (!Message_Decode('$', &TestStruct))
        printf("ERROR!\n");
    if (!Message_Decode('\n', &TestStruct))
        printf("ERROR!\n");
    printf("If No Error, Decoder has bugs\n");

    if (!Message_Decode('$', &TestStruct))
        printf("ERROR!\n");
    if (!Message_Decode('R', &TestStruct))
        printf("ERROR!\n");
    if (!Message_Decode('E', &TestStruct))
        printf("ERROR!\n");
    if (!Message_Decode('S', &TestStruct))
        printf("ERROR!\n");
    if (!Message_Decode(',', &TestStruct))
        printf("ERROR!\n");
    if (!Message_Decode('7', &TestStruct))
        printf("ERROR!\n");
    if (!Message_Decode(',', &TestStruct))
        printf("ERROR!\n");
    if (!Message_Decode('8', &TestStruct))
        printf("ERROR!\n");
    if (!Message_Decode(',', &TestStruct))
        printf("ERROR!\n");
    if (!Message_Decode('9', &TestStruct))
        printf("ERROR!\n");
    if (!Message_Decode('*', &TestStruct))
        printf("ERROR!\n");
    if (!Message_Decode('5', &TestStruct))
        printf("ERROR!\n");
    if (!Message_Decode('E', &TestStruct))
        printf("ERROR!\n");
    if (!Message_Decode('\n', &TestStruct))
        printf("ERROR!\n");
    printf("Result expect: 7 8 9, Type: 7\n");
    printf("Param0: %d\nParam1: %d\nParam2: %d\n", TestStruct.param0, TestStruct.param1, TestStruct.param2);
    printf("Type: %d\n", TestStruct.type);

    printf("-------------------------Encode Test-----------------------\n");
    char messagestr[MESSAGE_MAX_LEN];
    messageTest.param0 = 3;
    messageTest.param1 = 3;
    messageTest.param2 = 1;
    messageTest.type = MESSAGE_RES;
    Message_Encode(messagestr, messageTest);
    if (strcmp(messagestr, "$RES,3,3,1*59\n") == 0)
        printf("Passed\n");
    else
        printf("ERROR: Encode Error!\n");
    messageTest.param0 = 43182;
    messageTest.param1 = 8;
    messageTest.param2 = 9;
    messageTest.type = MESSAGE_CHA;
    Message_Encode(messagestr, messageTest);
    if (strcmp(messagestr, "$CHA,43182*5A\n") == 0)
        printf("Passed\n");
    else
        printf("ERROR: Encode Error!\n");

}