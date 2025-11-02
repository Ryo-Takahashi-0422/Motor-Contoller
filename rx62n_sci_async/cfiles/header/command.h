#pragma once

#include <string.h>
#include <stdint.h>
#include "sci.h"
#include <common.h>
//#include <stdio.h>

// 関数ポインタ型
typedef void* (*CommandFunc)(int, void* context);

// 各コマンド関数の定義
void* CommandSpd(int val, void* context);
void* CommandPval(int val, void* context);
void* CommandIval(int val, void* context);
void* CommandDval(int val, void* context);
void* CommandRead(int val, void* context);
void* CommandReadRom(int val, void* context);

// コマンド識別用の列挙型
typedef enum {
    CMD_SPD,
    CMD_PVAL,
    CMD_IVAL,
    CMD_DVAL,
    CMD_READ,
    CMD_UNKNOWN
} CommandType;

// コマンドテーブル構造体
typedef struct {
    const char *name;     // 文字列キー
    CommandType type;     // enum
    CommandFunc func;     // 対応関数
} CommandEntry;

// 他ファイルから参照するための外部宣言
extern const CommandEntry commandTable[];
extern const int COMMAND_COUNT;

// コマンド名をキーに検索して関数ポインタを返す
const CommandEntry* findCommand(const char *cmdName);

static uint8_t command_txbuf[] = {"ACK\n"};

#define NULL_SIZE       (1)