/******************************************************************************
* DISCLAIMER

* This software is supplied by Renesas Electronics Corp. and is only 
* intended for use with Renesas products. No other uses are authorized.

* This software is owned by Renesas Electronics Corp. and is protected under 
* all applicable laws, including copyright laws.

* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, 
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
* PARTICULAR PURPOSE AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY 
* DISCLAIMED.

* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORP. NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES 
* FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS 
* AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

* Renesas reserves the right, without notice, to make changes to this 
* software and to discontinue the availability of this software.  
* By using this software, you agree to the additional terms and 
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
******************************************************************************
* Copyright (C) 2010. Renesas Electronics Corporation. All rights reserved.
*******************************************************************************	
* File Name    : iic.c
* Version      : 1.10
* Description  : RIIC single master program
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 16.07.2010 1.00    First Release
*         : 27.11.2011 1.01    Update the function "IIC_RXI_IntEepRead"
*                              for Technical update "TN-RX*-A005A"
*         : 26.09.2014 1.10    Change the baudrate to 400kbps
******************************************************************************/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_apn_iic_eep.h"

/******************************************************************************
Typedef definitions
******************************************************************************/
enum RiicInternalMode_t{
	IIC_MODE_IDLE,					/* For internal variable 'iic_mode' */
	IIC_MODE_EEP_READ,				/* For internal variable 'iic_mode' */
	IIC_MODE_EEP_WRITE				/* For internal variable 'iic_mode' */
};

enum RiicErrCode_t{
	RIIC_ERR_MODE_SP_INT,			/* Return value for function 'IIC_Error' */
	RIIC_ERR_MODE_RX_INT_EEP_WRITE,	/* Return value for function 'IIC_Error' */
	RIIC_ERR_MODE_RX_INT,			/* Return value for function 'IIC_Error' */
	RIIC_ERR_MODE_TX_INT,			/* Return value for function 'IIC_Error' */
	RIIC_ERR_MODE_TE_INT,			/* Return value for function 'IIC_Error' */
	RIIC_ERR_SCL_LOCK,				/* Return value for function 'IIC_Error' */
	RIIC_ERR_SDA_LOCK,				/* Return value for function 'IIC_Error' */
	RIIC_ERR_GEN_CLK_BBSY			/* Return value for function 'IIC_Error' */
};

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static IIC_API_T iic_buff;					/* Structure for IIC function */
static enum RiicInternalMode_t iic_mode;	/* Internal IIC mode buffer*/
static enum RiicStatus_t iic_status;		/* Status buffer for function 'IIC_GetStatus' */
static uint32_t iic_trm_cnt;				/* Internal cunter for transmission */
static uint32_t iic_rcv_cnt;				/* Internal cunter for reception */

static void IIC_EEI_IntTimeOut(void);
static void IIC_EEI_IntAL(void);
static void IIC_EEI_IntSP(void);
static void IIC_EEI_IntST(void);
static void IIC_EEI_IntNack(void);
static void IIC_RXI_IntEepRead (void);
static void IIC_TXI_IntEepWrite(void);
static void IIC_TXI_IntEepRead(void);
static void IIC_TEI_IntEepWrite(void);
static void IIC_TEI_IntEepRead(void);
static void IIC_GenClkSP(void);
static void IIC_Error(RiicErrCode_t);
uint16_t ssr;
/******************************************************************************
* Function Name: IIC_Create
* Description  : Initialize and enable RIIC
* Arguments    : none
* Return Value : none
******************************************************************************/
void IIC_Create(void)
{
	/* When clear ICE and set IICRST, all RIIC registers are reset.			*/
	RIIC.ICCR1.BIT.ICE = 0;			/* RIIC disable for RIIC initial */
	RIIC.ICCR1.BIT.IICRST = 1;		/* RIIC all reset */
	
	/* Enable RIIC */
	RIIC.ICCR1.BIT.ICE = 1;			/* Enable RIIC */

	RIIC.ICSER.BIT.SAR0E = 1;
	RIIC.SARL0.BIT.SVA = 0x50;
	
	/* Set baudrate is 400Kbps (when input = 12MHz).		*/
	/* IIC phi = 12MHz (1cyc = 83.33ns).					*/
	RIIC.ICMR1.BIT.CKS = 2;			/* IIC phi = PCLK/4 clock */
	RIIC.ICBRH.BIT.BRH = 6;			/* Set High width of SCL */
	RIIC.ICBRL.BIT.BRL = 11;		/* Set Low width of SCL */

	/* Time out setting */
	RIIC.ICMR2.BIT.TMOL = 0;		/* Disable SCL=Low time out */
	RIIC.ICMR2.BIT.TMOH = 1;		/* Enable SCL=High time out */
	RIIC.ICMR2.BIT.TMOS = 0;		/* Long mode (16bits counter).		*/
									/* PCLK=48MHz, IIC phi=12MHz,	*/
									/* Long mode is about 5.461 ms	*/

	RIIC.ICFER.BIT.TMOE = 1;		/* Enable time out detection */

	/* Disable all address detection. This sample code is for only master mode */
	//RIIC.ICSER.BYTE = 0x00;

	/* ACKWP is protect bit for ACKBT. */
	RIIC.ICMR3.BIT.ACKWP = 1;		/* Disable protect for ACKBT */

	/* Enable/Disable each interrupt */
	RIIC.ICIER.BYTE = 0xBB;			/* b0: Enable Time Out interrupt */
									/* b1: Enable Arbitration Lost interrupt */
									/* b2: Disable Start Condition Detection Interrupt */
									/* b3: Enable Stop condition detection interrupt */
									/* b4: Enable NACK reception interrupt */
									/* b5: Enable Receive Data Full Interrupt */
									/* b6: Disable Transmit End Interrupt */
									/* b7: Enable Transmit Data Empty Interrupt */
//	RIIC.ICIER.BIT.TIE   = 1; // Transmit Data Empty Interrupt Enable
//	RIIC.ICIER.BIT.TEIE  = 1; // Transmit End Interrupt Enable
//	RIIC.ICIER.BIT.RIE   = 1; // Receive Data Full Interrupt Enable
//	RIIC.ICIER.BIT.NAKIE = 1; // NACK Interrupt Enable
//	RIIC.ICIER.BIT.SPIE  = 1; // Stop Condition Interrupt Enable
//	RIIC.ICIER.BIT.STIE  = 0; // (optional)
//	RIIC.ICIER.BIT.ALIE  = 1; // Arbitration Lost Interrupt Enable
//	RIIC.ICIER.BIT.TMOIE   = 1; // Timeout Interrupt Enable

	/* Initialize ram for RIIC */
	iic_mode = IIC_MODE_IDLE;		/* Internal IIC mode */
	iic_status = RIIC_STATUS_IDLE;	/* Status buffer for function 'IIC_GetStatus' */
	iic_trm_cnt = 0;				/* Clear the internal transmission counter for IIC */
	iic_rcv_cnt = 0;				/* Clear the internal reception counter for IIC */

	//RIIC.ICFER.BIT.NACKE = 0;
	RIIC.ICCR1.BIT.IICRST = 0;		/* Clear reset */
	


}

/******************************************************************************
* Function Name: IIC_Destroy
* Description  : Disable RIIC
* Arguments    : none
* Return Value : none
******************************************************************************/
void IIC_Destroy(void)
{
	/* When clear ICE and set IICRST, all RIIC registers are reset. */
	RIIC.ICCR1.BIT.ICE = 0;			/* RIIC disable for RIIC initial */
	RIIC.ICCR1.BIT.IICRST = 1;		/* RIIC all reset */
	RIIC.ICCR1.BIT.IICRST = 0;		/* Clear reset */
}

/******************************************************************************
* Function Name: IIC_EepWrite
* Description  : Start EEPROM write process or Acknowledge polling.
*              : When data1.PreCnt = 0 and data1.RWCnt = 0, this function do as acknowledge polling.
* Argument     : data1
*              : 	SlvAdr - 0x00-0xFE				// Slave Device Address.
*              : 									// Don't set bit0. It's a Read/Write bit
*              : 	PreCnt - 0x0000-0xFFFF			// Memory Addr of EEPROM
*              : 	*pPreData						// Pointer for EEPROM addr buffer
*              : 	RWCnt - 0x00000000-0xFFFFFFFF	// Number of Data
*              : 	*pRWData						// Pointer for Data
* Return Value : RIIC_OK			 - Start EepWrite process correctly
*              : RIIC_MODE_ERROR	 - Running IIC on other mode
*              : RIIC_BUS_BUSY		 - IIC Bus busy
******************************************************************************/
enum RiicEepFnc_t IIC_EepWrite(IIC_API_T data1)
{
	/* Check the IIC mode (controled by software) */
	if(iic_mode != IIC_MODE_IDLE)
	{
		return RIIC_MODE_ERROR;					/* Running IIC on other mode */
	}

	/* Store the parameter data to ram for IIC */
	iic_buff = data1;

	/* Check IIC bus busy								*/
	/* If IIC bus is not free, return 'RIIC_BUS_BUSY'	*/
	if(RIIC.ICCR2.BIT.BBSY != 0)
	{
		return RIIC_BUS_BUSY;
	}

	/* Set each RAM */
	iic_mode = IIC_MODE_EEP_WRITE;				/* Change the internal mode to EEPROM write mode */
	iic_status = RIIC_STATUS_ON_COMMUNICATION;	/* Change the status to ON COMMUNICATION */
	iic_trm_cnt = 0;							/* Clear the internal transmission counter for IIC */

	/* Generate start condition */
	RIIC.ICCR2.BIT.ST = 1;

	return RIIC_OK;
}

/******************************************************************************
* Function Name: IIC_RandomRead
* Description  : Start Randam read process
* Argument     : data1
*              : 	SlvAdr - 0x00-0xFE				// Slave Device Address.
*              : 									// Don't set bit0. It's a Read/Write bit
*              : 	PreCnt - 0x0000-0xFFFF			// Memory Addr of EEPROM
*              : 	*pPreData						// Pointer for EEPROM addr buffer
*              : 	RWCnt - 0x00000000-0xFFFFFFFF	// Number of Data
*              : 	*pRWData						// Pointer for Data
* Return Value : RIIC_OK			 - Start RandomRead process correctly
*              : RIIC_MODE_ERROR	 - Running IIC on other mode
*              : RIIC_PRM_ERROR		 - Parameter error
*              : RIIC_BUS_BUSY		 - IIC Bus busy
******************************************************************************/
enum RiicEepFnc_t IIC_RandomRead(IIC_API_T data1)
{
	volatile uint8_t tmp;
	volatile uint32_t cnt;

	/* Check the IIC mode (controled by software) */
	if(iic_mode != IIC_MODE_IDLE)
	{
		return RIIC_MODE_ERROR;					/* Running IIC on other mode */
	}

	/* Store the parameter data to ram for IIC */
	iic_buff = data1;

	/* Check parameter */
	if((iic_buff.RWCnt == 0) || ((iic_buff.PreCnt == 0)))
	{
		return RIIC_PRM_ERROR;					/* Parameter error */
	}

	/* Check IIC bus busy								*/
	/* If IIC bus is not free, return 'RIIC_BUS_BUSY'	*/
	if(RIIC.ICCR2.BIT.BBSY != 0)
	{
		return RIIC_BUS_BUSY;
	}

	/* Set each RAM */
	iic_mode = IIC_MODE_EEP_READ;				/* Change the internal mode to EEPROM read mode */
	iic_status = RIIC_STATUS_ON_COMMUNICATION;	/* Change the status to ON COMMUNICATION */
	iic_trm_cnt = 0;							/* Clear the internal transmission counter for IIC */
	iic_rcv_cnt = 0;							/* Clear the internal reception counter for IIC */

	RIIC.ICCR2.BIT.ST = 1;						/* Generate start condition */

	return RIIC_OK;
}

/******************************************************************************
* Function Name: IIC_GetStatus
* Description  : Get IIC status
* Argument     : data1			// Pointer for IIC communication status buffer
*              : 	RIIC_STATUS_IDLE			 - RIIC is Idle mode. Users can start communication.
*              : 	RIIC_STATUS_ON_COMMUNICATION - RIIC is on communication.
*              : 									Users cannot start communication.
*              : 	RIIC_STATUS_NACK			 - RIIC has received NACK.
*              : 									Users can start next communication.
*              : 	RIIC_STATUS_FAILED			 - RIIC has stopped the communication.
*              : 									Users can start next communication.
*              : 									But the last communication was failed.
*              :
*              : data2			// Pointer for IIC Bus status buffer
*              : 	RIIC_BUS_STATUS_FREE - IIC bus is free
*              : 	RIIC_BUS_STATUS_BBSY - IIC bus is busy
* Return Value : none
******************************************************************************/
void IIC_GetStatus(enum RiicStatus_t *data1, enum RiicBusStatus_t *data2)
{
	*data1 = iic_status;
	

	/* Check bus busy */
	if(RIIC.ICCR2.BIT.BBSY == 0)
	{
		//*data1 = RIIC_STATUS_IDLE; // 本当の遷移条件の調査マスト
		*data2 = RIIC_BUS_STATUS_FREE;
	}else
	{
		*data2 = RIIC_BUS_STATUS_BBSY;


	}
	
//	RIIC.ICCR1.BIT.ICE = 0;      // RIIC 無効化
//RIIC.ICCR1.BIT.IICRST = 1;   // 内部リセット

//RIIC.ICCR1.BIT.IICRST = 0;   // リセット解除
//RIIC.ICCR1.BIT.ICE = 1;      // RIIC 再有効化
}

/******************************************************************************
* Function Name: IIC_EEI_In
* Description  : IIC EEI interrupt function
* Argument     : none
* Return Value : none
******************************************************************************/
void IIC_EEI_Int(void)
{
	/* Check Timeout */
	if((RIIC.ICSR2.BIT.TMOF!=0) && (RIIC.ICIER.BIT.TMOIE!=0))
	{
		IIC_EEI_IntTimeOut();
	}

	/* Check Arbitration Lost */
	if((RIIC.ICSR2.BIT.AL!=0) && (RIIC.ICIER.BIT.ALIE!=0))
	{
		IIC_EEI_IntAL();
	}

	/* Check stop condition detection */
	if((RIIC.ICSR2.BIT.STOP!=0) && (RIIC.ICIER.BIT.SPIE!=0))
	{
		IIC_EEI_IntSP();
	}

	/* Check NACK reception */
	if((RIIC.ICSR2.BIT.NACKF != 0) && (RIIC.ICIER.BIT.NAKIE!=0))
	{
		IIC_EEI_IntNack();
	}

	/* Check start condition detection */
	if((RIIC.ICSR2.BIT.START != 0) && (RIIC.ICIER.BIT.STIE!=0))
	{
		IIC_EEI_IntST();
	}
}

/******************************************************************************
* Function Name: IIC_EEI_IntTimeOut
* Description  : Time out function
*              : This function is called by IIC_EEI_Int()
* Argument     : none
* Return Value : none
******************************************************************************/
static void IIC_EEI_IntTimeOut(void)
{
	IIC_GenClkSP();
}

/******************************************************************************
* Function Name: IIC_EEI_IntAL
* Description  : Arbitration function
*              : This function is called by IIC_EEI_Int()
* Argument     : none
* Return Value : none
******************************************************************************/
static void IIC_EEI_IntAL(void)
{
	IIC_GenClkSP();
}

/******************************************************************************
* Function Name: IIC_EEI_IntSP
* Description  : Stop condition detection function
*              : This function is called by IIC_EEI_Int()
* Argument     : none
* Return Value : none
******************************************************************************/
static void IIC_EEI_IntSP(void)
{
	/* Clear each status flag */
	RIIC.ICSR2.BIT.NACKF = 0;
	RIIC.ICSR2.BIT.STOP = 0;

	/* Enable/Disable each interrupt */
	RIIC.ICIER.BYTE = 0xBB;				/* b0: Enable Time Out interrupt */
										/* b1: Enable Arbitration Lost interrupt */
										/* b2: Disable Start Condition Detection Interrupt */
										/* b3: Enable Stop condition detection interrupt */
										/* b4: Enable NACK reception interrupt */
										/* b5: Enable Receive Data Full Interrupt */
										/* b6: Disable Transmit End Interrupt */
										/* b7: Enable Transmit Data Empty Interrupt */

	switch(iic_mode)
	{
		case IIC_MODE_EEP_WRITE:
			if(iic_buff.RWCnt != 0)		/* When the data number is not '0', */
			{							/* the communication is not complete  */
				iic_status = RIIC_STATUS_FAILED;
			}else
			{
				if(iic_status != RIIC_STATUS_NACK)
				{
					iic_status = RIIC_STATUS_IDLE;
				}
			}
			break;
		case IIC_MODE_EEP_READ:
			if(iic_buff.RWCnt != 0)		/* When the data number is not '0', */
			{							/* the communication is not complete */
				iic_status = RIIC_STATUS_FAILED;
			}else
			{
				iic_status = RIIC_STATUS_IDLE;
			}
			break;
		case IIC_MODE_IDLE:
				/* Other device generates stop condition.			*/
				/* Note: This code is for single master.			*/
			break;
		default:
			IIC_Error(RIIC_ERR_MODE_SP_INT);		/* Internal mode error */
			break;
	}

	/* Initialize ram for RIIC */
	iic_mode = IIC_MODE_IDLE;
	iic_trm_cnt = 0;			/* Clear the internal transmission counter for IIC */
	iic_rcv_cnt = 0;			/* Clear the internal reception counter for IIC */
}

/******************************************************************************
* Function Name: IIC_EEI_IntST
* Description  : Start condition detection function
*              : This function is called by IIC_EEI_Int()
* Argument     : none
* Return Value : none
******************************************************************************/
static void IIC_EEI_IntST(void)
{
	/* Disable Start Condition Detection Interrupt */
	RIIC.ICSR2.BIT.START = 0;
	RIIC.ICIER.BIT.STIE = 0;

	/* Transfer slave device address (Read) */
	RIIC.ICDRT = iic_buff.SlvAdr | 0x01;	/* When master transfer, b0 must be '1' */
}

/******************************************************************************
* Function Name: IIC_EEI_Int
* Description  : NACK reception function
*              : This function is called by IIC_EEI_Int()
* Argument     : none
* Return Value : none
******************************************************************************/
static void IIC_EEI_IntNack(void)
{
	/* Chenge the internal mode */
	iic_status = RIIC_STATUS_NACK;

	/* Disable interrupts */
	RIIC.ICIER.BIT.NAKIE = 0;		/* Disable NACK reception interrupt */
	RIIC.ICIER.BIT.TIE = 0;			/* Disable Transmit Data Empty Interrupt */
	RIIC.ICIER.BIT.TEIE = 0;		/* Disable Transmit End Interrupt */
	RIIC.ICIER.BIT.RIE = 0;			/* Disable Receive Data Full Interrupt */

	/* Generate Stop Condition */
	RIIC.ICSR2.BIT.STOP = 0;
	RIIC.ICCR2.BIT.SP = 1;
}

/******************************************************************************
* Function Name: IIC_RXI_Int
* Description  : Receive Data Full interrupt function
* Argument     : none
* Return Value : none
******************************************************************************/
void IIC_RXI_Int(void)
{
	switch(iic_mode)
	{
		case IIC_MODE_EEP_READ:
			IIC_RXI_IntEepRead();
			break;
		case IIC_MODE_EEP_WRITE:
			IIC_Error(RIIC_ERR_MODE_RX_INT_EEP_WRITE);		/* Internal mode error */
			break;
		default:
			IIC_Error(RIIC_ERR_MODE_RX_INT);				/* Internal mode error */
			break;
	}
}

/******************************************************************************
* Function Name: IIC_RXI_IntEepRead
* Description  : RXI interrupt function for EepRead process
*              : This function is called by IIC_RXI_Int
* Argument     : none
* Return Value : none
******************************************************************************/
static void IIC_RXI_IntEepRead (void)
{
	volatile uint8_t tmp;

	if(iic_rcv_cnt == 0){
		if(iic_buff.RWCnt <= 2){
			/* When WAIT bit is set, RIIC outputs SCL=Low after it receives 9th clock.	*/
			/* Then RIIC generates stop condition after it reads receive data.			*/
			RIIC.ICMR3.BIT.WAIT = 1;
		}

		if(iic_buff.RWCnt <= 1){
			/* If RIIC sends ACK when it receives last data, EEPROM may prevent RIIC generating 	*/
			/* stop condition. Because EEPROM desn't know how many RIIC try to read data.		*/
			/* When RIIC sends ACK, EEPROM realizes the next rising SCL for stop condition as 	*/
			/* next first bit SCL.																*/
			RIIC.ICMR3.BIT.ACKBT = 1;		/* Set Nack for final data.*/
		}

		/* dummy read */
		tmp = RIIC.ICDRR;
	}
	else{
		if(iic_buff.RWCnt == 3){
			/* When WAIT bit is set, RIIC outputs SCL=Low after it receives 9th clock.	*/
			/* Then RIIC generates stop condition after it reads receive data.			*/
			RIIC.ICMR3.BIT.WAIT = 1;
		}

		if(iic_buff.RWCnt == 2){
			/* If RIIC sends ACK when it receives last data, EEPROM may prevent RIIC generating 	*/
			/* stop condition. Because EEPROM desn't know how many RIIC try to read data.		*/
			/* When RIIC sends ACK, EEPROM realizes the next rising SCL for stop condition as 	*/
			/* next first bit SCL.																*/
			RIIC.ICMR3.BIT.ACKBT = 1;		/* Set Nack for final data.*/
		}

		if(iic_buff.RWCnt == 1){
			/* Receive final data */
			/* Prepare to generate Stop Condition */
			RIIC.ICSR2.BIT.STOP = 0;
			RIIC.ICCR2.BIT.SP = 1;

			/* After read final data, RIIC generates stop condtion */
			*iic_buff.pRWData++ = RIIC.ICDRR;	/* Read final data */
			iic_buff.RWCnt--;
			RIIC.ICMR3.BIT.WAIT = 0;
		}
		else{
			/* Read data */
			*iic_buff.pRWData++ = RIIC.ICDRR;
			iic_buff.RWCnt--;
		}
	}

	/* Increase internal reception counter on RAM. It is a receive data counter. */
	iic_rcv_cnt++;
}

/******************************************************************************
* Function Name: IIC_TXI_Int
* Description  : Transmit Data Empty Interrupt function
* Argument     : none
* Return Value : none
******************************************************************************/
void IIC_TXI_Int(void)
{
	switch(iic_mode)
	{
		case IIC_MODE_EEP_WRITE:
			IIC_TXI_IntEepWrite();
			break;
		case IIC_MODE_EEP_READ:
			IIC_TXI_IntEepRead();
			break;
		default:
			IIC_Error(RIIC_ERR_MODE_TX_INT);		/* Internal mode error */
			break;
	}
}

/******************************************************************************
* Function Name: IIC_TXI_IntEepWrite
* Description  : TXI interrupt function for EepWrite process
*              : This function is called by IIC_TXI_Int
* Argument     : none
* Return Value : none
******************************************************************************/
static void IIC_TXI_IntEepWrite(void)
{
	/* Increase internal transmission counter on RAM. */
	iic_trm_cnt++;

	/* Transfer slave device address */
	if(iic_trm_cnt == 1)
	{
		/* Transfer slave device address */
		RIIC.ICDRT = iic_buff.SlvAdr & 0xFE;	/* When master transfer, b0 must be '0' */
		return;
	};

	/* Transfer eeprom address for writting */
	if(iic_buff.PreCnt != 0)
	{
		RIIC.ICDRT = *iic_buff.pPreData++;
		iic_buff.PreCnt--;
		return;
	}

	/* Transfer data for writting */
	if(iic_buff.RWCnt != 0)
	{
		RIIC.ICDRT = *iic_buff.pRWData++;
		iic_buff.RWCnt--;
		return;
	}else
	{
		RIIC.ICIER.BIT.TEIE = 1;				/* Enable Transmit End Interrupt */
	}
}

/******************************************************************************
* Function Name: IIC_TXI_IntEepRead
* Description  : TXI interrupt function for EepRead process
*              : This function is called by IIC_TXI_Int
* Argument     : none
* Return Value : none
******************************************************************************/
static void IIC_TXI_IntEepRead(void)
{
	/* Increase internal transmission counter on RAM. */
	iic_trm_cnt++;

	/* Transfer slave device address */
	if(iic_trm_cnt == 1)
	{
		/* Transfer slave device address */
		RIIC.ICDRT = iic_buff.SlvAdr & 0xFE;	/* When master transfer, b0 must be '0' */
		return;
	};

	/* Transfer eeprom address for writting */
	if(iic_buff.PreCnt != 0)
	{
		RIIC.ICDRT = *iic_buff.pPreData++;
		iic_buff.PreCnt--;
		return;
	}else
	{
		RIIC.ICIER.BIT.TEIE = 1;				/* Enable Transmit End Interrupt */
	}
}

/******************************************************************************
* Function Name: IIC_TEI_Int
* Description  : Transmit End Interrupt function
* Argument     : none
* Return Value : none
******************************************************************************/
void IIC_TEI_Int(void)
{
	switch(iic_mode)
	{
		case IIC_MODE_EEP_WRITE:
			IIC_TEI_IntEepWrite();
			break;
		case IIC_MODE_EEP_READ:
			IIC_TEI_IntEepRead();
			break;
		default:
			IIC_Error(RIIC_ERR_MODE_TE_INT);	/* Internal mode error */
			break;
	}
}

/******************************************************************************
* Function Name: IIC_TEI_IntEepWrite
* Description  : TEI interrupt function for EepWrite process
*              : This function is called by IIC_TEI_Int
* Argument     : none
* Return Value : none
******************************************************************************/
static void IIC_TEI_IntEepWrite(void)
{
	/* Disable Transmit End Interrupt */
	RIIC.ICIER.BIT.TEIE = 0;

	/* Generate stop condition */
	RIIC.ICSR2.BIT.STOP = 0;
	RIIC.ICCR2.BIT.SP = 1;
}

/******************************************************************************
* Function Name: IIC_TEI_IntEepRead
* Description  : TEI interrupt function for EepRead process
*              : This function is called by IIC_TEI_Int
* Argument     : none
* Return Value : none
******************************************************************************/
static void IIC_TEI_IntEepRead(void)
{
	RIIC.ICSR2.BIT.START = 0;		/* Clear Start Condition Detection flag */
	RIIC.ICIER.BIT.STIE = 1;		/* Enable Start Condition Detection Interrupt */
	RIIC.ICIER.BIT.TEIE = 0;		/* Disable Transmit End Interrupt */

	/* Generate restart condition */
	RIIC.ICCR2.BIT.RS = 1;
}

/******************************************************************************
* Function Name: IIC_GenClkSP
* Description  : This function reset RIIC. Then do return processing.
*              : Return processing is 
*              : Step1 - Rest RIIC (try to be SCL=High and SDA=Hgih)
*              :         If SCL=Low though reset RIIC, other device out put SCL=Low
*              : Step2 - If SDA=Low, RIIC generates some clocks
*              : Step3 - After SDA=High, RIIC generates stop condition.
* Argument     : none
* Return Value : none
******************************************************************************/
static void IIC_GenClkSP(void)
{
	volatile uint16_t cnt;

	/* ICE=1 and IICRST=1 clear some status flags.									*/
	/* This reset desn't clear some settings, baud rate, slave address and so on.	*/
	RIIC.ICCR1.BIT.IICRST = 1;			/* Reset RIIC */

	/* Time out setting */
	RIIC.ICFER.BIT.TMOE = 0;			/* Disable time out detection */
	RIIC.ICMR2.BIT.TMOL = 1;			/* Disable SCL=Low time out */
	RIIC.ICMR2.BIT.TMOH = 1;			/* Enable SCL=High time out */
	RIIC.ICMR2.BIT.TMOS = 0;			/* Long mode (16bits counter), PCLK=48MHz,		*/
										/* IIC phi=12MHz, Long mode is about 5.461 ms	*/

	RIIC.ICCR1.BIT.IICRST = 0;			/* Clear reset */

	for(cnt=0; cnt<100; cnt++);			/* If RIIC output SDA=Low, SDA output is released	*/
										/* by IICRST=1. But IIC bus is pulluped therefore 	*/
										/* the SDA rising is not immediate.					*/

	RIIC.ICFER.BIT.TMOE = 1;			/* Enable time out detection */

	/* When SCL=Low though IICRST=1, other device output SCL=Low */
	if(RIIC.ICCR1.BIT.SCLI == 0)		/* Check SCL level */
	{
		IIC_Error(RIIC_ERR_SCL_LOCK);
	}

	/* After reset RIIC, this flow generates Stop condition.			*/
	/* But MST bit have to be set when RIIC generates Stop condition.	*/
	/* Here, disable protect, then set TRS and MST bits.				*/
	/* If set only MST, RIIC generates some clocks because RDRF=0.		*/
	/* When MST=1, TRS=0 and RDRF=0, RIIC starts Master Receive.		*/
	RIIC.ICMR1.BIT.MTWP = 1;
	RIIC.ICCR2.BIT.TRS = 1;
	RIIC.ICCR2.BIT.MST = 1;

	/* If other devices output SDA=Low, RIIC generates some clocks.		*/
	/* Usually, devices stop SDA=Low when they recieve some clocks.		*/
	if(RIIC.ICCR1.BIT.SDAI == 0)		/* Check SDA level */
	{
		for(cnt=0; cnt<10; cnt++){		/* Generate 10 clocks until SDA=High */
			if(RIIC.ICCR1.BIT.SDAI == 0)
			{
				RIIC.ICCR1.BIT.CLO = 1;				/* Generate a clock */
				while(RIIC.ICCR1.BIT.CLO != 0)		/* Wait for complete a generating clock */
				{
					if(RIIC.ICSR2.BIT.TMOF !=0)		/* When other device output SCL=Low,	*/
					{								/* time out occur.						*/
						RIIC.ICSR2.BIT.TMOF =0;
						break;
					}
				}
			}else
			{
				break;					/* When SDA=High, stop generating clocks */
			}

			/* Call error function if SDA = Low after RIIC generates some clocks */
			if((cnt == 9)&&(RIIC.ICCR1.BIT.SDAI == 0))
			{
				IIC_Error(RIIC_ERR_SDA_LOCK);
			}
		}
	}

	if(RIIC.ICCR2.BIT.BBSY == 0)		/* When Bus free, RIIC does't generate Stop condition */
	{
		RIIC.ICCR1.BIT.IICRST = 1;		/* Reset RIIC */
		RIIC.ICCR1.BIT.IICRST = 0;		/* Clear reset */
	}else
	{
		/* Generate stop condition */
		RIIC.ICSR2.BIT.STOP = 0;
		RIIC.ICCR2.BIT.SP = 1;
	}

	/* Enable MST/TRS Write Protect */
	RIIC.ICMR1.BIT.MTWP = 0;

	/* Set time out */
	RIIC.ICFER.BIT.TMOE = 0;			/* Disable time out detection */
	RIIC.ICMR2.BIT.TMOL = 0;			/* Disable SCL=Low time out */
	RIIC.ICMR2.BIT.TMOH = 1;			/* Enable SCL=High time out */
	RIIC.ICMR2.BIT.TMOS = 0;			/* Long mode (16bits counter), PCLK=48MHz,		*/
										/* IIC phi=12MHz. Long mode is about 5.461 ms	*/
	RIIC.ICFER.BIT.TMOE = 1;			/* Enable time out detection */
}

/******************************************************************************
* Function Name: IIC_Error
* Description  : Usually this function is not called.
* Argument     : error_code
*              :    RIIC_ERR_MODE_SP_INT
*              :    RIIC_ERR_MODE_RX_INT_EEP_WRITE
*              :    RIIC_ERR_MODE_RX_INT
*              :    RIIC_ERR_MODE_TX_INT
*              :    RIIC_ERR_MODE_TE_INT
*              :    RIIC_ERR_SCL_LOCK
*              :    RIIC_ERR_SDA_LOCK
*              :    RIIC_ERR_GEN_CLK_BBSY
* Return Value : none
******************************************************************************/
static void IIC_Error(enum RiicErrCode_t error_code)
{
	switch(error_code)
	{
		case RIIC_ERR_MODE_SP_INT:
			break;
		case RIIC_ERR_MODE_RX_INT_EEP_WRITE:
			break;
		case RIIC_ERR_MODE_RX_INT:
			break;
		case RIIC_ERR_MODE_TX_INT:
			break;
		case RIIC_ERR_MODE_TE_INT:
			break;
		case RIIC_ERR_SCL_LOCK:
			break;
		case RIIC_ERR_SDA_LOCK:
			break;
		case RIIC_ERR_GEN_CLK_BBSY:
			break;
		default:
			break;
	}

	while(1);
}

