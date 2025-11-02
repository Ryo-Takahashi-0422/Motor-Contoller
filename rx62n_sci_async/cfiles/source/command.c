#include <command.h>

// コマンドテーブル本体（実体はここだけ）
const CommandEntry commandTable[] = {
    {"spd", CMD_SPD, CommandSpd},
    {"pvl", CMD_PVAL, CommandPval},
    {"ivl", CMD_IVAL, CommandIval},
    {"dvl", CMD_DVAL, CommandDval},
    {"red", CMD_READ, CommandRead},
};

const int COMMAND_COUNT = sizeof(commandTable) / sizeof(commandTable[0]);

// Find command on command-table
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

// spd command
void* CommandSpd(int val, void* context)
{
    struct MotorParams* params = (struct MotorParams*)context;
    if (params) params->speed = val;
    
    return NULL;
}

// PID-P command
void* CommandPval(int val, void* context)
{
    struct MotorParams* params = (struct MotorParams*)context;
    if (params) params->p = val;
    
    return NULL;
}

// PID-I command
void* CommandIval(int val, void* context)
{
    struct MotorParams* params = (struct MotorParams*)context;
    if (params) params->i = val;
    
    return NULL;
}

// PID-D command
void* CommandDval(int val, void* context)
{
    struct MotorParams* params = (struct MotorParams*)context;
    if (params) params->d = val;
    
    return NULL;
}

// response to red(Read) request
void* CommandRead(int val, void* context)
{
    uint8_t result;
    
    // 読み込み先アドレスの保存
    struct MotorParams* params = (struct MotorParams*)context;
    if (params) params->address = val;
    
    // 応答を送信
//    do
//    {
//        result = SCI_StartTransmit(command_txbuf, (sizeof(command_txbuf) - NULL_SIZE), NULL);
//    }
//    while (SCI_BUSY == result);
    
    return CommandReadRom;
}

// Read Eeprom and send data
void* CommandReadRom(int val, void* context)
{
    int address;
    uint8_t upper, lower;
    
    // 読み込み先アドレスの取り出し
    struct MotorParams* params = (struct MotorParams*)context;
    if (params) address = params->address;
    
    upper = (address >> 8) & 0xFF;  // 上位8bit
    lower = address & 0xFF;         // 下位8bit
    
    EepromReadAndResponse(lower, upper, val);
       
    return NULL;
}