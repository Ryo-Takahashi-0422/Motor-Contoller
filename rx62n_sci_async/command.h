#pragma once
//#include <stdio.h>

// �֐��|�C���^�^
typedef void (*CommandFunc)(const char*, int, void* context);

// �e�R�}���h�֐��̒�`
void CommandSpd(const char* cmd, int val, void* context);
void CommandPval(const char* cmd, int val, void* context);
void CommandIval(const char* cmd, int val, void* context);
void CommandDval(const char* cmd, int val, void* context);

// �R�}���h���ʗp�̗񋓌^
typedef enum {
    CMD_SPD,
    CMD_PVAL,
    CMD_IVAL,
    CMD_DVAL,
    CMD_UNKNOWN
} CommandType;

// �R�}���h�e�[�u���\����
typedef struct {
    const char *name;     // ������L�[
    CommandType type;     // enum
    CommandFunc func;     // �Ή��֐�
} CommandEntry;

// ���t�@�C������Q�Ƃ��邽�߂̊O���錾
extern const CommandEntry commandTable[];
extern const int COMMAND_COUNT;

// �R�}���h�����L�[�Ɍ������Ċ֐��|�C���^��Ԃ�
const CommandEntry* findCommand(const char *cmdName);