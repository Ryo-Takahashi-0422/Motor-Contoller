#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "sci.h"
#include "r_apn_iic_eep.h"
#include <command.h>

typedef void (*TxEndCallback)(int);

void regTxEnd(TxEndCallback cb);

bool IICAckPolling(uint8_t, uint8_t, uint32_t);
void SampleEepromWrite(void);
void SampleEepromRead(void);
void EepromWrite(int data, int lAddr, int hAddr);
void EepromReadAndResponse(int lAddr, int hAddr, int len);
void Test();