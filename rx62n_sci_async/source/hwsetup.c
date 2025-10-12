/************************************************************************
*
* Device     : RX/RX600/RX62N
*
* File Name  : hwsetup.c
*
* Abstract   : Hardware Setup file.
*
* NOTE       : THIS IS A TYPICAL EXAMPLE.
*
* Copyright (C) 2009 Renesas Electronics Corporation.
* and Renesas Solutions Corporation. All rights reserved.
*
************************************************************************/
//#include "R_PG_sample_ext.h"
#include "..\iodefine.h"
extern void HardwareSetup(void);

void HardwareSetup(void)
{

	R_PG_Clock_Set();		// Set System Clock(ICLK=96MHz,PCLK=24MHz,BCLK=24MHz)

	R_PG_Timer_Set_MTU_U0_C0();				// Initial MTU2 Unit0,ch0(for Output PWM Pulse)
	R_PG_Timer_StartCount_MTU_U0_C0();		// MTU2 ch0 Count start
	
	R_PG_Timer_Set_MTU_U1_C6();
	R_PG_Timer_Set_MTU_U1_C7();
	
	R_PG_Timer_StartCount_MTU_U1_C6();
	R_PG_Timer_StartCount_MTU_U1_C7();
	
	R_PG_IO_PORT_Set_P1();
	R_PG_IO_PORT_Set_PE();

	R_PG_I2C_Set_C0();

}
