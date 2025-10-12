/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* System Name  : Sample program for Asynchronous communication with SCI
* File Name    : main.c
* Version      : 1.00
* Device       : R5F562N8BDBG (RX62N Group)
* Abstract     : Main program
* Tool-Chain   : High-performance Embedded Workshop (Version 4.09.01.007),
*                C/C++ Compiler Package for RX Family (V.1.02 Release 01)
* OS           : not use
* H/W Platform : Renesas Starter Kit for RX62N
* Description  : Asynchronous transmission and reception with SCI.
* Limitation   : none
*******************************************************************************/
/*******************************************************************************
* History      : DD.MM.YYYY Version  Description
*              : 31.05.2013 1.00     First Release
*******************************************************************************/

/*******************************************************************************
Includes <System Includes> , "Project Includes"
*******************************************************************************/
#include <machine.h>
#include <stdint.h>
#include "..\iodefine.h"
#include "sci.h"
#include <stdbool.h>
#include "r_apn_iic_eep.h"

/*******************************************************************************
Macro definitions
*******************************************************************************/
/* **** LEDs **** */
/* ==== LED0 (SCI transmission end) ==== */
//#define LED0_REG_PODR   PORT1.DR.BIT.B5    /* P02: Output data store bit */
//#define LED0_REG_PDR    PORT1.DDR.BIT.B5    /* P02: I/O select bit */
/* ==== LED1 (SCI reception end) ==== */
//#define LED1_REG_PODR   PORT0.DR.BIT.B3     /* P03: Output data store bit */
//#define LED1_REG_PDR    PORT0.DDR.BIT.B3    /* P03: I/O select bit */
/* ==== LED2 (SCI receive error) ==== */
#define LED2_REG_PODR   PORT0.DR.BIT.B5     /* P05: Output data store bit */
#define LED2_REG_PDR    PORT0.DDR.BIT.B5    /* P05: I/O select bit */
/* ==== LEDs output data ==== */
#define LED_ON          (0)                 /* LED on */
#define LED_OFF         (1)                 /* LED off */

/* **** Buffer **** */
#define BUF_SIZE        (12)                /* Buffer size */
#define NULL_SIZE       (1)                 /* Null code size */

#define TARGET_SLAVE_ADDRESS		0xA0	/* EEPROM address */
#define EEPROM_ADDRESS_LENGTH		2u		/* EEPROM address length */

/*******************************************************************************
Exported global variables and functions (to be accessed by other files)
*******************************************************************************/
void main(void);

/*******************************************************************************
Private variables and functions
*******************************************************************************/
/* **** SCI state **** */
static sci_state_t      sci_state = {0x00};
#define SCI_B_TX_BUSY   sci_state.bit.b_tx_busy     /* Transmission busy flag */
#define SCI_B_RX_BUSY   sci_state.bit.b_rx_busy     /* Reception busy flag */
#define SCI_B_RX_ORER   sci_state.bit.b_rx_orer     /* Overrun error flag */
#define SCI_B_RX_FER    sci_state.bit.b_rx_fer      /* Framing error flag */

static uint8_t rx_buf[BUF_SIZE];
static uint8_t tx_buf[] = {"ACK\n"};

static void port_init(void);
static void peripheral_init(void);

/* **** Callback Function **** */
static void cb_sci_tx_end(void);
static void cb_sci_rx_end(void);
static void cb_sci_rx_error(void);

static void irMtu7_A(void);

// I2C
bool IICAckPolling(uint8_t, uint8_t, uint32_t);
static void SampleEepromWrite(void);
static void EepromRandomRead(void);
static void SampleEepromRead(void);
static void CpuCreate(void);
static void CpuIntCreate(void);
static void IICPortCreate(void);
static bool CheckNACK(void);

char isRbuffFull;
int received_value = 1;
// I2C書き込みテスト用
uint8_t write_data[3];
// I2C 読み込みテスト用
uint8_t addr[2];
bool isReadI2C = false;
uint8_t tr, rcv;

uint8_t trm_eeprom_adr[EEPROM_ADDRESS_LENGTH];	/* Buffer for EEPROM addres for write */
uint8_t rcv_eeprom_adr[EEPROM_ADDRESS_LENGTH];	/* Buffer for EEPROM addres for read */
uint8_t trm_buff[8];							/* Buffer for EEPROM for write */
uint8_t rcv_buff[8];							/* Buffer for EEPROM for read */
IIC_API_T iic_buff_prm[2];						/* Structure for IIC function */

signed short Under_over_flow_cnt; 	/* MTU2 u1 ch7 underflow/overflow counter */	

#define		PSW_init  0x00010000

#define		TGRB_INIT_VAL	7500				// Interval=20ms(PCLK/64)
#define		DUTY_VAL	TGRB_INIT_VAL / 100		// Duty:10%
#define		TGRA_INIT_VAL	4687				// Interval=50ms(PCLK/256)
#define		MINIMUM_TGRA_VAL	0x200			// Interval=2.6ms(PCLK/128)

//char isOnError;
/*******************************************************************************
* Outline      : Main processing
* Header       : none
* Function Name: main
* Description  : After initialization, start to SCI transmission and reception.
* Arguments    : none
* Return Value : none
*******************************************************************************/
void main (void)
{
    uint8_t cnt;
    uint8_t result;
    tr = 5;
    
    isRbuffFull = 0;
    received_value = 1;
    //isOnError = 0;
    /* **** Initialize MCU **** */

    /* ---- Disable maskable interrupts ---- */
    clrpsw_i();

    /* ---- Initialization of the clock ---- */
    /* SCKCR - System Clock Control Register
    b31:b28  Reserved - The write value should be 0.
    b27:b24  ICK      - System Clock (ICLK) Select            - 8 times
    b19:b16  BCK      - External Bus Clock (BCLK) Select      - 4 times
    b11:b8   PCK      - Peripheral Module Clock (PCLK) Select - 4 times
    b7:b0    Reserved - The write value should be 0. */
    SYSTEM.SCKCR.LONG = 0x00010100;
    
        /* ==== Enable maskable interrupts ==== */
    //setpsw_i();

    /* ==== Initialize I/O ports ==== */
    port_init();
    
    /* Initialize CPU( Clock setting, INTC and Port for IIC) */
    CpuCreate();
    
    /* Initialize and enable RIIC */
    IIC_Create();
    
    /* EEPROM Write (Master transfer) */
    SampleEepromWrite();

    /* Acknowledge polling (Master transfer) */
    //IICAckPolling(TARGET_SLAVE_ADDRESS, 0, 100);

    // バスフリー待ち
    do
    {
	;
    }while(RIIC.ICCR2.BIT.BBSY != 0);
    
    EepromRandomRead();

    /* EEPROM Read (Master transfer and Master receive) */
    SampleEepromRead();

    /* ==== Initialize peripheral functions ==== */
    peripheral_init();

    /* ==== Enable maskable interrupts ==== */
    setpsw_i();

    /* **** Initialize RAM **** */
    for (cnt = 0; cnt < BUF_SIZE; cnt++)
    {
        rx_buf[cnt] = 0; 
    }
    
    // モータドライバin1を0に
    // in2はMTIOC0BからPWMを受け取る
    R_PG_IO_PORT_Write_PE0(0);
    //R_PG_IO_PORT_Write_PE1(0);

    // tera turm側で改行押すと、その分受信バッファの先頭2byteが''で埋まるのに注意
    /* **** Start receive **** */
    do
    {
        result = SCI_StartReceive(rx_buf, BUF_SIZE, cb_sci_rx_end, cb_sci_rx_error);
    }
    while (SCI_BUSY == result);

    /* **** Main loop **** */
    while (1)
    {	    // isRbuffFull→通信完了的な名前に変える
	    if(isRbuffFull)
	    {		    
		result = SCI_StartReceive(rx_buf, BUF_SIZE, cb_sci_rx_end, cb_sci_rx_error);
		isRbuffFull = 0;		    
	    }
	
    }

}

/*******************************************************************************
* Outline      : Initialization of I/O ports
* Header       : none
* Function Name: port_init
* Description  : Initialize I/O ports.
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void port_init (void)
{

    /* ---- Initialize LEDs ---- */

    /* Set port output data - LEDs OFF */
    //LED0_REG_PODR = LED_OFF;
    //LED1_REG_PODR = LED_OFF;
    //LED2_REG_PODR = LED_OFF;

    /* Set port direction - Output */
    //LED0_REG_PDR = 1;
    //LED1_REG_PDR = 1;
    //LED2_REG_PDR = 1;

    // RXD0 (P20): 入力、周辺機能
    //PORT2.DDR.BIT.B0 = 0;     // 入力
    //PORT2.ICR.BIT.B0 = 1;     // 周辺機能（RXD0）
    //PORT2.ODR.BIT.B0 = 0;     // オープンドレイン無効（デフォルト）

    // TXD0 (P21): 出力、周辺機能
    //PORT2.DDR.BIT.B1 = 1;     // 出力
    //PORT2.PMR.BIT.B1 = 1;     // 周辺機能（TXD0）
    //PORT2.ODR.BIT.B1 = 0;     // オープンドレイン無効（デフォルト）
    
    //PORT1.ODR.BIT.B2 = 1;  // P12: open-drain
    //PORT1.ODR.BIT.B3 = 1;  // P13: open-drain
    
}

/******************************************************************************
* Function Name: CpuCreate
* Description  : CPU initial program
*              : Set CGC, INTC and Port for IIC
* Arguments    : none
* Return Value : none
******************************************************************************/
static void CpuCreate(void)
{
	/* On PowerOnReset, the system clocks are set to		*/
	/* ICLK=24MHz, BCLK=24MHz, PCLK=24MHz (EXTAL=12MHz) 	*/
	/* Reconfigure to 										*/
	/* ICLK=96MHz, BCLK=48MHz, PCLK=48MHz (EXTAL=12MHz)		*/
	//SYSTEM.SCKCR.LONG = 0x00010100;

	/* Initialize CPU interrupt for RIIC1 */
	CpuIntCreate();

	/* Initialize I/O ports for RIIC1 */
	IICPortCreate();

	/* Clear bit 20 of the MSTPB20, Module Stop Register B 					*/
	/* By default the RIIC1 peripheral is 'Stopped'							*/
	/* Enable the peripheral by enabling it in the Module Stop Register 	*/
	/* Refer to section 8 of the Hardware User Manual						*/
#ifdef RIIC_CH0
	MSTP(RIIC0) = 0;
#else
	MSTP(RIIC1) = 0;
#endif
}

/******************************************************************************
* Function Name: CpuIntCreate
* Description  : Initialize CPU interrupt registers for RIIC1
* Arguments    : none
* Return Value : none
******************************************************************************/
static void CpuIntCreate(void)
{
#ifdef RIIC_CH0
	/* The IPR register is used to set the priority level of an interrupt	*/
	/* source.																*/
	/* Here, IPR registers for RIIC1 are set priprity level 4.				*/
	IPR(RIIC0,ICEEI0) = 0x04;
	IPR(RIIC0,ICRXI0) = 0x04;
	IPR(RIIC0,ICTXI0) = 0x04;
	IPR(RIIC0,ICTEI0) = 0x04;

	/* Clear the interrupt request flags. On PowerOnReset, these bit are	*/
	/* cleared to 0. This process is for fail-safe  						*/
	IR(RIIC0,ICEEI0) = 0;
	IR(RIIC0,ICRXI0) = 0;
	IR(RIIC0,ICTXI0) = 0;
	IR(RIIC0,ICTEI0) = 0;

	/* The IERi register is used to enable or disable an interrupt request.	*/
	/* Enable all interrupt for CPU.										*/
	IEN(RIIC0,ICEEI0) = 1;
	IEN(RIIC0,ICRXI0) = 1;
	IEN(RIIC0,ICTXI0) = 1;
	ICU.IER[31].BIT.IEN2 = 1;
//	ICU.IPR[IRQ0].BIT.IPR = 0x0F;   // 最優先
//ICU.IPR[IRQ1].BIT.IPR = 0x0F;
//ICU.IPR[IRQ2].BIT.IPR = 0x0F;
	IEN(RIIC0,ICTEI0) = 1;
#else
	/* The IPR register is used to set the priority level of an interrupt	*/
	/* source.																*/
	/* Here, IPR registers for RIIC1 are set priprity level 4.				*/
	IPR(RIIC1,ICEEI1) = 0x04;
	IPR(RIIC1,ICRXI1) = 0x04;
	IPR(RIIC1,ICTXI1) = 0x04;
	IPR(RIIC1,ICTEI1) = 0x04;

	/* Clear the interrupt request flags. On PowerOnReset, these bit are	*/
	/* cleared to 0. This process is for fail-safe  						*/
	IR(RIIC1,ICEEI1) = 0;
	IR(RIIC1,ICRXI1) = 0;
	IR(RIIC1,ICTXI1) = 0;
	IR(RIIC1,ICTEI1) = 0;

	/* The IERi register is used to enable or disable an interrupt request.	*/
	/* Enable all interrupt for CPU.										*/
	IEN(RIIC1,ICEEI1) = 1;
	IEN(RIIC1,ICRXI1) = 1;
	IEN(RIIC1,ICTXI1) = 1;
	IEN(RIIC1,ICTEI1) = 1;
#endif
}

/******************************************************************************
* Function Name: IICPortCreate
* Description  : Initialize I/O ports for IIC1
* Arguments    : none
* Return Value : none
******************************************************************************/
static void IICPortCreate(void)
{
#ifdef RIIC_CH0
	/* Set port direction to input */
	PORT1.DDR.BIT.B3 = 0;		/* SDA0 */
	PORT1.DDR.BIT.B2 = 0;		/* SCL0 */

	/* Enable the input buffer */
	PORT1.ICR.BIT.B3 = 1;		/* SDA0 */
	PORT1.ICR.BIT.B2 = 1;		/* SCL0 */
#else
	/* Set port direction to input */
	PORT2.DDR.BIT.B0 = 0;		/* SDA1 */
	PORT2.DDR.BIT.B1 = 0;		/* SCL1 */

	/* Enable the input buffer */
	PORT2.ICR.BIT.B0 = 1;		/* SDA1 */
	PORT2.ICR.BIT.B1 = 1;		/* SCL1 */
#endif
}


/*******************************************************************************
* Outline      : Initialization of peripheral functions
* Header       : none
* Function Name: peripheral_init
* Description  : Initialize peripheral functions to use.
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void peripheral_init (void)
{

    /* Initialize SCI */
    SCI_Init();

}

/*******************************************************************************
* Outline      : Callback function (SCI transmission end)
* Header       : sci.h
* Function Name: cb_sci_tx_end
* Description  : This function to be called when transmission has done.
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void cb_sci_tx_end (void)
{

    /* LED0 ON (SCI transmission end) */
    //LED0_REG_PODR = LED_OFF;
    isRbuffFull = 1;

}

/*******************************************************************************
* Outline      : Callback function (SCI reception end)
* Header       : sci.h
* Function Name: cb_sci_rx_end
* Description  : This function to be called when reception has done.
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void cb_sci_rx_end (void)
{
    // 受信バッファがフルの状態でこの割り込みが発生している
    // この処理中に新たなデータが送信(tera turmで文字入力)されると、
    uint8_t result;
       
    write_data[0] = 0x00;   // 書き込み先アドレス上位バイト（例：0x0010 → 0x00）
    write_data[1] = 0x10;   // 書き込み先アドレス下位バイト（→ 0x10）
    write_data[2] = 0xAB;   // 実際に書き込むデータ  
    
    addr[0] = 0x00;
    addr[1] = 0x10;

    /* LED1 ON (SCI reception end) */
    //LED1_REG_PODR = LED_ON;
    //LED0_REG_PODR = LED_ON;
    received_value = atoi((char*)rx_buf);
    if(received_value < 1)
    {
        received_value = 1;
    }
    memset(rx_buf, 0, sizeof(rx_buf));
    
    // 応答を送信
    do
    {
        result = SCI_StartTransmit(tx_buf, (sizeof(tx_buf) - NULL_SIZE), cb_sci_tx_end);
    }
    while (SCI_BUSY == result);
    
    //
//    if(!isReadI2C)
//    {
//	isReadI2C = true;
	
//	/* EEPROM Write (Master transfer) */
//	SampleEepromWrite();
	
//	/* Acknowledge polling (Master transfer) */
//	IICAckPolling(TARGET_SLAVE_ADDRESS, 0, 100);
//    }
//    else
//    {
//	/* EEPROM Read (Master transfer and Master receive) */
//	SampleEepromRead();
//	tr++;
//	isReadI2C = false;
//    }
    
}

/*******************************************************************************
* Outline      : Callback function (SCI receive error)
* Header       : sci.h
* Function Name: cb_sci_rx_error
* Description  : This function to be called when reception error occurred.
*                This sampling code does not include error process. 
*                Add the program when it needed.
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void cb_sci_rx_error (void)
{

    /* LED2 ON (SCI receive error) */
    //LED2_REG_PODR = LED_ON;

    /* ==== Get SCI state ==== */
    sci_state = SCI_GetState();

    /* ---- Overrun error ---- */
    if (0 != SCI_B_RX_ORER)
    {
        /* This sampling code does not include any process. */
        /* Add the program when it needed. */
    }

    /* ---- Framing error ---- */
    if (0 != SCI_B_RX_FER)
    {
        /* This sampling code does not include any process. */
        /* Add the program when it needed. */
    }
    
    //isOnError = 1;
}

static bool CheckNACK(void)
{
    if(RIIC.ICSR2.BIT.NACKF != 0)
    {
        // 停止条件発行
        RIIC.ICSR2.BIT.STOP = 0;
	RIIC.ICCR2.BIT.SP = 1;
		
	// 停止条件発行確認
	do
	{
            ;
	}while(RIIC.ICSR2.BIT.STOP != 1);
		
	// 次の通信のための処理
	RIIC.ICSR2.BIT.NACKF = 0;
	RIIC.ICSR2.BIT.STOP = 0;
	// 送信終了
	    return false;
    }
    
    else
    {
	return true;
    }
}

/******************************************************************************
* Function Name: SampleEepromWrite
* Description  : Sample Eeprom Write
* Argument     : none
* Return Value : none
******************************************************************************/
static void SampleEepromWrite(void)
{
	uint8_t slave_addr = 0x50;
	bool isACK;
	char c = 'r';
//	volatile uint32_t cnt;
//	enum RiicStatus_t tmp_status;
//	enum RiicBusStatus_t tmp_bus_status;
	
//	int sizeBuff = (sizeof(trm_buff)/sizeof(trm_buff[0]));

//	/* Set the data for writting to EEROM (Sample data) */
//	for(cnt=0; cnt < sizeBuff; cnt++){
//		trm_buff[cnt] = cnt;
//	}

//	/* Set the address where the RIIC write date to EEPROM */
//	trm_eeprom_adr[0] = 0x00;
//	trm_eeprom_adr[1] = 0x00;

//	/* Set the parameters for 'IIC_EepWrite' */
//	iic_buff_prm[0].SlvAdr = TARGET_SLAVE_ADDRESS;		/* Target slave device */
//	iic_buff_prm[0].PreCnt = EEPROM_ADDRESS_LENGTH;		/* EEPROM address length */
//	iic_buff_prm[0].pPreData = trm_eeprom_adr;			/* Pointer for EEPROM adress buffer */
//	iic_buff_prm[0].RWCnt = 8;							/* Data length of writting data */
//	iic_buff_prm[0].pRWData = trm_buff;					/* Pointer for writting data buffer */

//	/* Start EEPROM Write */
//	if(RIIC_OK != IIC_EepWrite(iic_buff_prm[0]))
//	{
//		/* This software is for single master 					*/
//		/* Therefore, return value should be always 'RIIC_OK'	*/
//		while(1){};
//	}
	
	if(RIIC.ICCR2.BIT.BBSY != 0)
	{
		return RIIC_BUS_BUSY;
	}
	
	// 開始条件発行
	RIIC.ICCR2.BIT.ST = 1;
	
	// NACK確認
	isACK = CheckNACK();
	if(!isACK)
	{
	    return;
	}
	
	// スレーブアドレス+W(0x00)書き込み
	do
	{
		;
	}while(RIIC.ICSR2.BIT.TDRE != 1);
	RIIC.ICDRT = (slave_addr << 1) | 0x00;
	
	// NACK確認
	isACK = CheckNACK();
	if(!isACK)
	{
	    return;
	}
	
	// Hアドレス書き込み
	do
	{
		;
	}while(RIIC.ICSR2.BIT.TDRE != 1);
	RIIC.ICDRT = 0x00;
	
	// NACK確認
	isACK = CheckNACK();
	if(!isACK)
	{
	    return;
	}
	
	// Lアドレス書き込み
	do
	{
		;
	}while(RIIC.ICSR2.BIT.TDRE != 1);
	RIIC.ICDRT = 0x01;
	
	// NACK確認
	isACK = CheckNACK();
	if(!isACK)
	{
	    return;
	}
	
	// データbyte書き込み
	do
	{
		;
	}while(RIIC.ICSR2.BIT.TDRE != 1);
	RIIC.ICDRT = c;

	// TEND待ち
	do
	{
		;
	}while(RIIC.ICSR2.BIT.TEND != 1);
	
	// 停止条件発行
	RIIC.ICSR2.BIT.STOP = 0;
	RIIC.ICCR2.BIT.SP = 1;
	
	// 停止条件発行確認
	do
	{
		;
	}while(RIIC.ICSR2.BIT.STOP != 1);
	
	// 次の通信のための処理
	RIIC.ICSR2.BIT.NACKF = 0;
	RIIC.ICSR2.BIT.STOP = 0;
	
	// ★マスタ送信完了

//	/* Wait for communication complete */
//	do{
//		IIC_GetStatus(&tmp_status, &tmp_bus_status);
//	}while(tmp_status == RIIC_STATUS_ON_COMMUNICATION);

//	/* Wait for IIC bus free */
//	do{
//		IIC_GetStatus(&tmp_status, &tmp_bus_status);
//	}while(tmp_bus_status != RIIC_BUS_STATUS_FREE);
	

}

void wait_ms(uint32_t ms)
{
    volatile uint32_t i;
    while (ms--)
    {
        /* CPUクロック = 100MHz の場合、下のループで約1ms */
        for (i = 0; i < 40000; i++) {
            __nop();   // または空文でもOK
        }
    }
}

// ランダム(アドレス指定)読み出し
static void EepromRandomRead(void)
{
	uint8_t slave_addr = 0x50;
	volatile uint8_t rBuff;
	bool isACK;
		
	if(RIIC.ICCR2.BIT.BBSY != 0)
	{
		return RIIC_BUS_BUSY;
	}
	
//	do
//	{
//		;
//	}while((RIIC.ICCR2.BIT.BBSY == 0));
	

	// 書き込み開始条件発行
	RIIC.ICCR2.BIT.ST = 1;
	
	// NACK確認
	isACK = CheckNACK();
	if(!isACK)
	{
	    return;
	}
	
	// スレーブアドレス+W(0x00)書き込み
	do
	{
		;
	}while(RIIC.ICSR2.BIT.TDRE != 1);
	RIIC.ICDRT = (slave_addr << 1) | 0x00;
	
	wait_ms(50);
	
	// NACK確認
	isACK = CheckNACK();
	if(!isACK)
	{
	    return;
	}
	
	// Hアドレス書き込み
	do
	{
		;
	}while(RIIC.ICSR2.BIT.TDRE != 1);
	RIIC.ICDRT = 0x00;
	
	wait_ms(50);
	
	// NACK確認
	isACK = CheckNACK();
	if(!isACK)
	{
	    return;
	}
	
	// ★ブレークポイント無しでNACKが返る(恐らく他でも発生する　一つ前のSampleEEpWriteでは発生しない...)
	// NACKが返るとRIICは処理を中断するため、どう復帰させるかがポイント
	// Lアドレス書き込み
	do
	{
		;
	}while(RIIC.ICSR2.BIT.TDRE != 1);
	RIIC.ICDRT = 0x00;
	
	wait_ms(50);
	
	// NACK確認
	isACK = CheckNACK();
	if(!isACK)
	{
	    return;
	}
	
	// TEND待ち
	do
	{
		;
	}while(RIIC.ICSR2.BIT.TEND != 1);
	
	// 停止条件発行せず次の通信へ
	//RIIC.ICSR2.BIT.STOP = 0;
	//RIIC.ICCR2.BIT.SP = 1;
	//RIIC.ICSR2.BIT.NACKF = 0;
	//RIIC.ICSR2.BIT.START = 0;
	//RIIC.ICSR2.BIT.STOP = 0;
	
	// ランダム読み込み開始条件発行
	RIIC.ICCR2.BIT.RS = 1;
	
	// スレーブアドレス+W(0x00)書き込み
	do
	{
		;
	}while(RIIC.ICSR2.BIT.TDRE != 1);
	RIIC.ICDRT = (slave_addr << 1) | 0x01;
	
	wait_ms(50);
	
	// RDRF待ち
	do
	{
		;
	}while(RIIC.ICSR2.BIT.RDRF != 1);
	
		// データ取得
	rBuff = RIIC.ICDRR;
	rcv_buff[0] = rBuff;
	
	// NACK確認
	isACK = CheckNACK();
	if(!isACK)
	{
	    return;
	}
	

	// No ACk　このまま終了
	
	//RIIC.ICMR3.BIT.ACKBT = 0;
	// RDRF待ち
	do
	{
		;
	}while(RIIC.ICSR2.BIT.RDRF != 1);
	

        // 停止条件発行
        RIIC.ICSR2.BIT.STOP = 0;
	RIIC.ICCR2.BIT.SP = 1;
	rBuff = RIIC.ICDRR;
	RIIC.ICMR3.BIT.WAIT = 0;
	// 無しで00 00 'e'がrbuffへ格納される
//   	rBuff = RIIC.ICDRR;
//	rcv_buff[0] = rBuff;
	// 停止条件発行確認
	do
	{
            ;
	}while(RIIC.ICSR2.BIT.STOP != 1);
		
	// 次の通信のための処理
	RIIC.ICSR2.BIT.NACKF = 0;
	RIIC.ICSR2.BIT.STOP = 0;
	
}
/******************************************************************************
* Function Name: SampleEepromRead
* Description  : Sample Eeprom Read (Random read)
* Argument     : none
* Return Value : none
******************************************************************************/
static void SampleEepromRead(void)
{
	uint8_t slave_addr = 0x50;
	uint8_t rcv_buff[256];
	volatile uint8_t tmp;
	bool isACK;
//	volatile uint32_t cnt;
//	enum RiicStatus_t tmp_status;
//	enum RiicBusStatus_t tmp_bus_status;

//	/* Clear the data buffer */
//	for(cnt=0; cnt < (sizeof(rcv_buff)/sizeof(rcv_buff[0])); cnt++){
//		rcv_buff[cnt] = 0;
//	}

//	/* Set the address where the RIIC read date to EEPROM */
//	/* In this sample, RIIC read data from address 0x0000 */
//	rcv_eeprom_adr[0] = 0x00;
//	rcv_eeprom_adr[1] = 0x00;

//	/* Set the parameter for 'IIC_RandomRead' */
//	iic_buff_prm[1].SlvAdr = TARGET_SLAVE_ADDRESS + 1;		/* Target slave device */
//	iic_buff_prm[1].PreCnt = EEPROM_ADDRESS_LENGTH;		/* EEPROM address length */
//	iic_buff_prm[1].pPreData = rcv_eeprom_adr;			/* Pointer for EEPROM adress buffer */
//	iic_buff_prm[1].RWCnt = 8;							/* Data length of read data */
//	iic_buff_prm[1].pRWData = rcv_buff;					/* Pointer for read data buffer */

//	/* Start EEPROM Read */
//	if(RIIC_OK != IIC_RandomRead(iic_buff_prm[1]))
//	{
//		/* This software is for single master 					*/
//		/* Therefore, return value should be always 'RIIC_OK'	*/
//		while(1){};
//	}

//	/* Wait for communication complete */
//	do{
//		IIC_GetStatus(&tmp_status, &tmp_bus_status);
//	}while(tmp_status == RIIC_STATUS_ON_COMMUNICATION);

//	/* Wait for IIC bus free */
//	do{
//		IIC_GetStatus(&tmp_status, &tmp_bus_status);
//	}while(tmp_bus_status != RIIC_BUS_STATUS_FREE);
	if(RIIC.ICCR2.BIT.BBSY != 0)
	{
		return RIIC_BUS_BUSY;
	}
	
	// 開始条件発行
	RIIC.ICCR2.BIT.ST = 1;
	
	// スレーブアドレス+W(0x00)書き込み
	do
	{
		;
	}while(RIIC.ICSR2.BIT.TDRE != 1);
	RIIC.ICDRT = (slave_addr << 1) | 0x01;
	
	// RDRF待ち
	do
	{
		;
	}while(RIIC.ICSR2.BIT.RDRF != 1);	
	
	
	// NACK確認
	if(RIIC.ICSR2.BIT.NACKF != 0)
	{
            // 停止条件発行
            RIIC.ICSR2.BIT.STOP = 0;
	    RIIC.ICCR2.BIT.SP = 1;
	    
	    // ダミーリード
	    tmp = RIIC.ICDRR;
	    
	    // 停止条件発行確認
	    do
	    {
                ;
	    }while(RIIC.ICSR2.BIT.STOP != 1);
		
	    // 次の通信のための処理
	    RIIC.ICSR2.BIT.NACKF = 0;
	    RIIC.ICSR2.BIT.STOP = 0;
	    
	    return;
	}
	
	// ダミーリードして受信開始
	tmp = RIIC.ICDRR;
	
	// RDRF待ち
	do
	{
		;
	}while(RIIC.ICSR2.BIT.RDRF != 1);		
	
	
	
}

/******************************************************************************
* Function Name: IICAckPolling
* Description  : Acknowledge polling
*              : This function is locked until target eeprom reply ACK.
* Argument     : in_addr1 - Slave device address
*              :          - 0x00-0xFE	// Don't set bit0. It's a Read/Write bit
*              : in_num - Number of acknowledge polling
*              :        - 0x00-0xFF		// When number is 0, continue to wait for ACK
*              : in_len - Length between each polling
*              :         - 0x00000000-0xFFFFFFFF
* Return Value : Receive ACK - true
*              : Not receive ACK - false
******************************************************************************/
bool IICAckPolling(uint8_t in_addr1, uint8_t in_num, uint32_t in_len)
{
	IIC_API_T tmp_buff;
	enum RiicStatus_t tmp_status;
	enum RiicBusStatus_t tmp_bus_status;
	int8_t tmp_num;
	volatile uint32_t cnt1, cnt2;

	/* Store the parameter data to ram for IIC */
	tmp_buff.SlvAdr = in_addr1;
	tmp_buff.PreCnt = 0;
	tmp_buff.RWCnt = 0;

	/* Store the number for polling */
	tmp_num = in_num;

	/* Acknowledge polling until target eeprom reply ACK */
	do{
		/* Start Acknowledge polling (Master transfer) */
		while(RIIC_OK != IIC_EepWrite(tmp_buff)){};

		/* Wait for communication complete */
		do{
			IIC_GetStatus(&tmp_status, &tmp_bus_status);
		}while(tmp_status == RIIC_STATUS_ON_COMMUNICATION);

		/* When EEPROM reply NACK, wait for some time.	*/
		/* Then start Acknowledge polling again 		*/
		if(tmp_status == RIIC_STATUS_NACK)
		{
			for(cnt1=in_len; cnt1!=0; cnt1--)		/* wait time for eeprom ack */
			{
				for(cnt2=0; cnt2<100; cnt2++);
			}
		}
		/* When recive ACK, return 'true' */
		else if(tmp_status == RIIC_STATUS_IDLE)
		{
			return true;
		}

		tmp_num--;		/* Decrement polling counter */
	}while((tmp_num != 0) || (in_num == 0));

	/* When don't receive ACK, return 'false' */
	return false;
}

//========================  Interrupt Function  ===============================

// MTU2 ch0 Compare Match A Interrupt
// Change PWM Pulse Duty
int cnt = 0;
float a = 0.0f;
void Mtu0IcCmAIntFunc(void)
{
	static bool f_pwm;
	unsigned short tgra,tgrb,tgrc,tgrd,tgre,tgrf;
	int dVal = DUTY_VAL * received_value + 1;
	
	if(dVal < TGRB_INIT_VAL * 0.01)
	{
		dVal = TGRB_INIT_VAL * 0.01;
	}

	/* Get TGRB Register Value */
	R_PG_Timer_GetTGR_MTU_U0_C0(&tgra,&tgrb,&tgrc,&tgrd,&tgre,&tgrf);

	/* Change PWM Duty */
//	if(!f_pwm){
//		if(tgrb > dVal )		// Down Duty(Up Light)
//			tgrb -= dVal;
//		else
//			f_pwm = 1;
//	}else{
//		if(tgrb < TGRB_INIT_VAL)	// Up Duty(Down Light)
//			tgrb += dVal;
//		else
//			f_pwm = 0;
//	}
	
	// 値を直に書き込み、LEDとモータドライバへ出力
	tgrb = dVal;
	// MTU2 unit0 ch0(LED)TGR_Bレジスタ書き込み
	R_PG_Timer_SetTGR_B_MTU_U0_C0(tgrb);	
}

// MTU2 Unit1 ch6 Input Capture B Interrupt(by encoder pulse count clock(1/4))
// A相パルス立ち上がりをキャッチ
void Mtu6IcCmBIntFunc(void)
{
	// MTU6のTGRB,TGRDにはパルス入力カウントされ、ある値で0に戻るループを繰り返している
// 以下のサンプルコードによる計算を参考にして、パルス幅を計算できそう
	//    volatile unsigned char dummy;
//    signed long data;
  
//    /* ---- Clearing of TGIB interrupt flag ---- */
//    MTU20.TSR.BYTE &= 0xfd;
//    dummy = MTU20.TSR.BYTE;     /* TSR register dummy read  */

//    data = MTU20.TGRB - MTU20.TGRD;    
//        /* Read TGRB capture register */
//        /* Read TGRD buffer register  */

//   if ( data < 0){
//       TGRD0_B0_data_diff = data + (CH0_TGRC_CYCLE+1);	/* set data */
//   }else{
//       TGRD0_B0_data_diff = data;					/* set data */
//   }
}

// MTU2 Unit1 ch6 Compare Match C Interrupt
// 1ms : TCNT_0 = 5999でコンペアマッチ割り込み
void Mtu6IcCmCIntFunc(void)
{
	irMtu7_A();
}

// MTU2 Unit1 ch7 Compare Match A Interrupt
// ch6のCコンペアマッチによりキャプチャ割り込み発生(1msデフォ)
static void irMtu7_A(void)
{
    //unsigned short tgra,tgrb,tgrc,tgrd,tgre,tgrf;
    //R_PG_Timer_GetTGR_MTU_U1_C7(&tgra,&tgrb,&tgrc,&tgrd,&tgre,&tgrf);
    unsigned short tcnt;
    R_PG_Timer_HaltCount_MTU_U1_C7();
    R_PG_Timer_SetCounterValue_MTU_U1_C7(0);
    R_PG_Timer_StartCount_MTU_U1_C7();
	
}

// MTU2 Unit1 ch7 OverFlow Interrupt
void Mtu7OvIntFunc(void)
{
    //volatile unsigned char dummy;
    
    /* ---- Clearing of TCFV Overflow Flag ---- */
    //MTU7.TSR.BYTE &= 0xef;
    //dummy = MTU7.TSR.BYTE;     /* register dummy read */
	
    Under_over_flow_cnt++;
	
}

// MTU2 Unit1 ch7 UnderFlow Interrupt
void Mtu7UnIntFunc(void)
{
    //volatile unsigned char dummy;
    
    /* ---- Clearing of TCFU Underflow Flag ---- */
    //MTU7.TSR.BYTE &= 0xdf;
    //dummy = MTU7.TSR.BYTE;     /* register dummy read */

    Under_over_flow_cnt--;
}

// I2C全データ受信完了時割り込み
void IIC0MasterReFunc(void)
{
	int i = 0;
	i = rcv;
}

// I2C全データ送信完了時割り込み
void IIC0MasterTrFunc(void)
{
	int i = 0;
	i = tr;
}

/* End of File */

