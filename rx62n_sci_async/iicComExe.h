#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "r_apn_iic_eep.h"

bool IICAckPolling(uint8_t, uint8_t, uint32_t);
void SampleEepromWrite(void);
void SampleEepromRead(void);
void EepromWrite(int data, int lAddr, int hAddr);