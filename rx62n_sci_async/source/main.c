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
#pragma once
#include <machine.h>
#include <stdint.h>
#include "..\iodefine.h"
#include "sci.h"
#include <stdbool.h>
#include "r_apn_iic_eep.h"
#include <command.h>
#include <iicComExe.h>

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
static void CpuCreate(void);
static void CpuIntCreate(void);
static void IICPortCreate(void);
static bool CheckNACK(void);

char isRbuffFull;
int received_value = 1;
// I2C�������݃e�X�g�p
uint8_t write_data[3];
// I2C �ǂݍ��݃e�X�g�p
uint8_t addr[2];
bool isReadI2C = false;
uint8_t tr, rcv;

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
    //clrpsw_i();
    /* ==== Enable maskable interrupts ==== */
    setpsw_i();
    
    /* ---- Initialization of the clock ---- */
    /* SCKCR - System Clock Control Register
    b31:b28  Reserved - The write value should be 0.
    b27:b24  ICK      - System Clock (ICLK) Select            - 8 times
    b19:b16  BCK      - External Bus Clock (BCLK) Select      - 4 times
    b11:b8   PCK      - Peripheral Module Clock (PCLK) Select - 4 times
    b7:b0    Reserved - The write value should be 0. */
    SYSTEM.SCKCR.LONG = 0x00010100;

    /* ==== Initialize I/O ports ==== */
    port_init();
    
    /* Initialize CPU( Clock setting, INTC and Port for IIC) */
    CpuCreate();
    
    /* Initialize and enable RIIC */
    IIC_Create();  

    /* EEPROM Write (Master transfer) */
    SampleEepromWrite();

    /* Acknowledge polling (Master transfer) */
    IICAckPolling(TARGET_SLAVE_ADDRESS, 0, 100);  

    /* EEPROM Read (Master transfer and Master receive) */
    SampleEepromRead();

    /* ==== Initialize peripheral functions ==== */
    peripheral_init();

    /* ==== Enable maskable interrupts ==== */
    //setpsw_i();

    /* **** Initialize RAM **** */
    for (cnt = 0; cnt < BUF_SIZE; cnt++)
    {
        rx_buf[cnt] = 0; 
    }
    
    // ���[�^�h���C�oin1��0��
    // in2��MTIOC0B����PWM���󂯎��
    R_PG_IO_PORT_Write_PE0(0);
    //R_PG_IO_PORT_Write_PE1(0);

    /* **** Start receive **** */
    do
    {
        result = SCI_StartReceive(rx_buf, BUF_SIZE, cb_sci_rx_end, cb_sci_rx_error);
    }
    while (SCI_BUSY == result);

    /* **** Main loop **** */
    while (1)
    {	    // isRbuffFull���ʐM�����I�Ȗ��O�ɕς���
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

    // RXD0 (P20): ���́A���Ӌ@�\
    //PORT2.DDR.BIT.B0 = 0;     // ����
    //PORT2.ICR.BIT.B0 = 1;     // ���Ӌ@�\�iRXD0�j
    //PORT2.ODR.BIT.B0 = 0;     // �I�[�v���h���C�������i�f�t�H���g�j

    // TXD0 (P21): �o�́A���Ӌ@�\
    //PORT2.DDR.BIT.B1 = 1;     // �o��
    //PORT2.PMR.BIT.B1 = 1;     // ���Ӌ@�\�iTXD0�j
    //PORT2.ODR.BIT.B1 = 0;     // �I�[�v���h���C�������i�f�t�H���g�j
    
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

static char tolower_local(char c)
{
    if (c >= 'A' && c <= 'Z')
        return c + ('a' - 'A');
    return c;
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
    uint8_t received_bytes = 0;
    int j = 0;
    // ��M�o�b�t�@���t���̏�Ԃł��̊��荞�݂��������Ă���
    // ���̏������ɐV���ȃf�[�^�����M(tera turm�ŕ�������)�����ƁA
    uint8_t result;
    char cmd[8] = {0};
    int val = 0;
    const CommandEntry *entry;
    int i = 0;
       
    /* LED1 ON (SCI reception end) */
    //LED1_REG_PODR = LED_ON;
    //LED0_REG_PODR = LED_ON;
    received_value = atoi((char*)rx_buf);
    if(received_value < 1)
    {
        received_value = 1;
    }
//    memset(rx_buf, 0, sizeof(rx_buf));
    // �\���̂�spd,pid�l�Ȃǂ��ꌳ�Ǘ�/�R�}���h�ɂ����̍\���̃|�C���^�n����
    // ������ɕԂ�&EEPROM�ւ̕ۑ������̃|�C���^�f�[�^�Ɋ�Â��s���݌v�ł���
    
    // ��M���������͂��Ď��s
    // ���s�E���A�R�[�h������
    for (i = 0; i < BUF_SIZE; i++) {
        if (rx_buf[i] == '\r' || rx_buf[i] == '\n') {
            rx_buf[i] = '\0';
        }
	else
	{
	    received_bytes++;
	}
    }
    
    i = 0;
    
	// 1. �R�}���h�������R�s�[
	while (i < received_bytes && rx_buf[i] != ',' && j < sizeof(cmd)-1) {
	    cmd[j++] = rx_buf[i++];
	}
	cmd[j] = '\0';

	// 2. �J���}���X�L�b�v
	if (i < received_bytes && rx_buf[i] == ',') i++;

	// 3. �������������
	val = 0;
	while (i < received_bytes && rx_buf[i] >= '0' && rx_buf[i] <= '9') {
	    val = val * 10 + (rx_buf[i] - '0');
	    i++;
	}

    // ��M������̉�� ("cmd, val"�`��)
    //if (sscanf((char *)rx_buf, "%3[^,],%d", cmd, &val) == 2) {
        // �����������ē���i�啶�����M�ł�OK�ɂ������ꍇ�j
        //for (i = 0; cmd[i]; i++) cmd[i] = tolower_local(cmd[i]);

        entry = findCommand(cmd);
        if (entry && entry->func) {
            entry->func(cmd, val);  // ��1���� 'A' �͔C��
        } else {

        }
    //} 
    //else 
    //{

    //}

    memset(rx_buf, 0, sizeof(rx_buf));
    
    // �����𑗐M
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
        // ��~�������s
        RIIC.ICSR2.BIT.STOP = 0;
	RIIC.ICCR2.BIT.SP = 1;
		
	// ��~�������s�m�F
	do
	{
            ;
	}while(RIIC.ICSR2.BIT.STOP != 1);
		
	// ���̒ʐM�̂��߂̏���
	RIIC.ICSR2.BIT.NACKF = 0;
	RIIC.ICSR2.BIT.STOP = 0;
	// ���M�I��
	    return false;
    }
    
    else
    {
	return true;
    }
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
	
	// �l�𒼂ɏ������݁ALED�ƃ��[�^�h���C�o�֏o��
	tgrb = dVal;
	// MTU2 unit0 ch0(LED)TGR_B���W�X�^��������
	R_PG_Timer_SetTGR_B_MTU_U0_C0(tgrb);	
}

// MTU2 Unit1 ch6 Input Capture B Interrupt(by encoder pulse count clock(1/4))
// A���p���X�����オ����L���b�`
void Mtu6IcCmBIntFunc(void)
{
	// MTU6��TGRB,TGRD�ɂ̓p���X���̓J�E���g����A����l��0�ɖ߂郋�[�v���J��Ԃ��Ă���
// �ȉ��̃T���v���R�[�h�ɂ��v�Z���Q�l�ɂ��āA�p���X�����v�Z�ł�����
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
// 1ms : TCNT_0 = 5999�ŃR���y�A�}�b�`���荞��
void Mtu6IcCmCIntFunc(void)
{
	irMtu7_A();
}

// MTU2 Unit1 ch7 Compare Match A Interrupt
// ch6��C�R���y�A�}�b�`�ɂ��L���v�`�����荞�ݔ���(1ms�f�t�H)
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

/* End of File */

