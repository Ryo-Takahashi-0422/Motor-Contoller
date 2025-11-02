/******************************************************************************
* DISCLAIMER
* Please refer to http://www.renesas.com/disclaimer
******************************************************************************
  Copyright (C) 2010. Renesas Electronics Corporation. All rights reserved.
*******************************************************************************
* File Name    : r_apn_iic_eep.h
* Version      : 1.00
* Description  : RIIC program header
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 15.07.2010 1.00    First Release
******************************************************************************/
#ifndef R_APN_IIC_EEP_DEF
#define R_APN_IIC_EEP_DEF

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_apn_grobal.h"

/******************************************************************************
Typedef definitions
******************************************************************************/
struct str_IIC_API_T
{
	uint8_t		SlvAdr;			/* Slave Address, Don't set bit0. It's a Read/Write bit */
	uint16_t	PreCnt;			/* Number of Predata */
	uint8_t		*pPreData;		/* Pointer for PreData (Memory Addr of EEPROM) */
	uint32_t	RWCnt;			/* Number of Data */
	uint8_t		*pRWData;		/* Pointer for Data buffer */
};
typedef struct str_IIC_API_T IIC_API_T;

enum RiicEepFnc_t{
	RIIC_OK,						/* Return value for function 'IIC_EepWrite' and 'IIC_RandomRead' */
	RIIC_BUS_BUSY,					/* Return value for function 'IIC_EepWrite' and 'IIC_RandomRead' */
	RIIC_MODE_ERROR,				/* Return value for function 'IIC_EepWrite' and 'IIC_RandomRead' */
	RIIC_PRM_ERROR					/* Return value for function 'IIC_EepWrite' and 'IIC_RandomRead' */
};

enum RiicStatus_t{
	RIIC_STATUS_IDLE,				/* Return value for function 'IIC_GetStatus' */
	RIIC_STATUS_ON_COMMUNICATION,	/* Return value for function 'IIC_GetStatus' */
	RIIC_STATUS_NACK,				/* Return value for function 'IIC_GetStatus' */
	RIIC_STATUS_FAILED				/* Return value for function 'IIC_GetStatus' */
};

enum RiicBusStatus_t{
	RIIC_BUS_STATUS_FREE,			/* Return value for function 'IIC_GetStatus' */
	RIIC_BUS_STATUS_BBSY			/* Return value for function 'IIC_GetStatus' */
};

/******************************************************************************
Macro definitions
******************************************************************************/
#define RIIC_CH0							/* When use channel0, define 'RIIC_CH0' */
//#define RIIC_CH1							/* When use channel1, define 'RIIC_CH1' */

/* Users don't have to change the below defines. */
#ifdef	RIIC_CH0
	#define RIIC	RIIC0					/* Replace I/O define */
#else
	#define RIIC	RIIC1					/* Replace I/O define */
#endif

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
void IIC_Create(void);
void IIC_Destroy(void);
enum RiicEepFnc_t IIC_EepWrite(IIC_API_T);
enum RiicEepFnc_t IIC_RandomRead(IIC_API_T);
void IIC_GetStatus(enum RiicStatus_t*, enum RiicBusStatus_t*);

/* Interrupt fumctions for IIC */
void IIC_EEI_Int(void);
void IIC_RXI_Int(void);
void IIC_TXI_Int(void);
void IIC_TEI_Int(void);

#endif	/* R_APN_IIC_EEP_DEF */

