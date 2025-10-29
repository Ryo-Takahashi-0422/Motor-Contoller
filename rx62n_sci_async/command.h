#pragma once
//#include <stdio.h>

// 関数ポインタ型
typedef void (*CommandFunc)(const char*, int, void* context);

// 各コマンド関数の定義
void CommandSpd(const char* cmd, int val, void* context);
void CommandPval(const char* cmd, int val, void* context);
void CommandIval(const char* cmd, int val, void* context);
void CommandDval(const char* cmd, int val, void* context);

// コマンド識別用の列挙型
typedef enum {
    CMD_SPD,
    CMD_PVAL,
    CMD_IVAL,
    CMD_DVAL,
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