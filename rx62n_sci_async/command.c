#include <common.h>
#include <command.h>
#include <string.h>

// コマンドテーブル本体（実体はここだけ）
const CommandEntry commandTable[] = {
    {"spd", CMD_SPD, CommandSpd},
    {"pvl", CMD_PVAL, CommandPval},
    {"ivl", CMD_IVAL, CommandIval},
    {"dvl", CMD_DVAL, CommandDval},
};

const int COMMAND_COUNT = sizeof(commandTable) / sizeof(commandTable[0]);

const CommandEntry* findCommand(const char *cmdName) 
{
    int i = 0;
    for (i = 0; i < COMMAND_COUNT; i++) {
        if (strcmp(commandTable[i].name, cmdName) == 0) {
            return &commandTable[i];
        }
    }
    return NULL;
}


void CommandSpd(const char* cmd, int val, void* context)
{
    struct MotorParams* params = (struct MotorParams*)context;
    if (params) params->speed = val;
}

void CommandPval(const char* cmd, int val, void* context)
{
    struct MotorParams* params = (struct MotorParams*)context;
    if (params) params->p = val;    
}

void CommandIval(const char* cmd, int val, void* context)
{
    struct MotorParams* params = (struct MotorParams*)context;
    if (params) params->i = val;    
}

void CommandDval(const char* cmd, int val, void* context)
{
    struct MotorParams* params = (struct MotorParams*)context;
    if (params) params->d = val;    
}

