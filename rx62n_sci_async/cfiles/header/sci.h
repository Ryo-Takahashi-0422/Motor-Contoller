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
* File Name    : sci.h
* Version      : 1.00
* Device       : R5F562N8BDBG (RX62N Group)
* Abstract     : Header of serial control program
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
#include "sci_cfg.h"

/* Guards against multiple inclusion */
#ifndef SCI_H
#define SCI_H


/*******************************************************************************
Macro definitions
*******************************************************************************/
/* **** Return value for SCI_StartReceive, SCI_StartTransmit function **** */
#define SCI_OK      (0x00)      /* Transmission/Reception start */
#define SCI_BUSY    (0x01)      /* Transmission/Reception busy */
#define SCI_NG      (0x02)      /* Parameter error */

/*******************************************************************************
Typedef definitions
*******************************************************************************/
/* **** SCI state **** */
#pragma bit_order left
#pragma unpack
typedef union
{
    uint8_t byte;
    struct
    {
        uint8_t b_tx_busy:1;    /* Transmission busy flag   0: Transmission is ready    1: Transmission is busy */
        uint8_t b_rx_busy:1;    /* Reception busy flag      0: Reception is ready       1: Reception is busy */
        uint8_t b_rx_orer:1;    /* Overrun error flag       0: No error                 1: Overrun error */
        uint8_t b_rx_fer :1;    /* Framing error flag       0: No error                 1: Framing error */
        uint8_t          :4;    /* not use */
    } bit;
} sci_state_t;
#pragma packoption
#pragma bit_order

/* **** Callback function **** */
typedef void (* CallBackFunc)(void);

/*******************************************************************************
Exported global variables and functions (to be accessed by other files)
*******************************************************************************/
/* **** User I/F function **** */
void        SCI_Init(void);
uint8_t     SCI_StartReceive(uint8_t * pbuf, uint8_t num, CallBackFunc pcb_rx_end, CallBackFunc pcb_rx_error);
uint8_t     SCI_StartTransmit(const uint8_t * pbuf, uint8_t num, CallBackFunc pcb_tx_end);
sci_state_t SCI_GetState(void);

/*******************************************************************************
Macro definitions
*******************************************************************************/

/* **** Configuration of SCIn **** */

/* ==== SCI0 ==== */
#ifdef SELECT_SCI0

    /* ---- I/O register settings ---- */
    #define SCIn                SCI0
    #define MSTP_SCIn           MSTP(SCI0)
    #define IPR_SCIn            IPR(SCI0, )
    #define IR_SCIn_ERIn        IR(SCI0,ERI0)
    #define IR_SCIn_RXIn        IR(SCI0,RXI0)
    #define IR_SCIn_TXIn        IR(SCI0,TXI0)
    #define IR_SCIn_TEIn        IR(SCI0,TEI0)
    #define IEN_SCIn_ERIn       IEN(SCI0,ERI0)
    #define IEN_SCIn_RXIn       IEN(SCI0,RXI0)
    #define IEN_SCIn_TXIn       IEN(SCI0,TXI0)
    #define IEN_SCIn_TEIn       IEN(SCI0,TEI0)

    /* ---- Interrupt entry ---- */
    #pragma interrupt (Excep_SCIn_ERIn(vect=214))
    #pragma interrupt (Excep_SCIn_RXIn(vect=215))
    #pragma interrupt (Excep_SCIn_TXIn(vect=216))
    #pragma interrupt (Excep_SCIn_TEIn(vect=217))

#endif

/* ==== SCI1 ==== */
#ifdef SELECT_SCI1

    /* ---- I/O register settings ---- */
    #define SCIn                SCI1
    #define MSTP_SCIn           MSTP(SCI1)
    #define IPR_SCIn            IPR(SCI1, )
    #define IR_SCIn_ERIn        IR(SCI1,ERI1)
    #define IR_SCIn_RXIn        IR(SCI1,RXI1)
    #define IR_SCIn_TXIn        IR(SCI1,TXI1)
    #define IR_SCIn_TEIn        IR(SCI1,TEI1)
    #define IEN_SCIn_ERIn       IEN(SCI1,ERI1)
    #define IEN_SCIn_RXIn       IEN(SCI1,RXI1)
    #define IEN_SCIn_TXIn       IEN(SCI1,TXI1)
    #define IEN_SCIn_TEIn       IEN(SCI1,TEI1)

    /* ---- Interrupt entry ---- */
    #pragma interrupt (Excep_SCIn_ERIn(vect=218))
    #pragma interrupt (Excep_SCIn_RXIn(vect=219))
    #pragma interrupt (Excep_SCIn_TXIn(vect=220))
    #pragma interrupt (Excep_SCIn_TEIn(vect=221))

#endif

/* ==== SCI2 ==== */
#ifdef SELECT_SCI2

    /* ---- I/O register settings ---- */
    #define SCIn                SCI2
    #define MSTP_SCIn           MSTP(SCI2)
    #define IPR_SCIn            IPR(SCI2, )
    #define IR_SCIn_ERIn        IR(SCI2,ERI2)
    #define IR_SCIn_RXIn        IR(SCI2,RXI2)
    #define IR_SCIn_TXIn        IR(SCI2,TXI2)
    #define IR_SCIn_TEIn        IR(SCI2,TEI2)
    #define IEN_SCIn_ERIn       IEN(SCI2,ERI2)
    #define IEN_SCIn_RXIn       IEN(SCI2,RXI2)
    #define IEN_SCIn_TXIn       IEN(SCI2,TXI2)
    #define IEN_SCIn_TEIn       IEN(SCI2,TEI2)

    /* ---- Interrupt entry ---- */
    #pragma interrupt (Excep_SCIn_ERIn(vect=222))
    #pragma interrupt (Excep_SCIn_RXIn(vect=223))
    #pragma interrupt (Excep_SCIn_TXIn(vect=224))
    #pragma interrupt (Excep_SCIn_TEIn(vect=225))

#endif

/* ==== SCI3 ==== */
#ifdef SELECT_SCI3

    /* ---- I/O register settings ---- */
    #define SCIn                SCI3
    #define MSTP_SCIn           MSTP(SCI3)
    #define IPR_SCIn            IPR(SCI3, )
    #define IR_SCIn_ERIn        IR(SCI3,ERI3)
    #define IR_SCIn_RXIn        IR(SCI3,RXI3)
    #define IR_SCIn_TXIn        IR(SCI3,TXI3)
    #define IR_SCIn_TEIn        IR(SCI3,TEI3)
    #define IEN_SCIn_ERIn       IEN(SCI3,ERI3)
    #define IEN_SCIn_RXIn       IEN(SCI3,RXI3)
    #define IEN_SCIn_TXIn       IEN(SCI3,TXI3)
    #define IEN_SCIn_TEIn       IEN(SCI3,TEI3)

    /* ---- Interrupt entry ---- */
    #pragma interrupt (Excep_SCIn_ERIn(vect=226))
    #pragma interrupt (Excep_SCIn_RXIn(vect=227))
    #pragma interrupt (Excep_SCIn_TXIn(vect=228))
    #pragma interrupt (Excep_SCIn_TEIn(vect=229))

#endif

/* ==== SCI5 ==== */
#ifdef SELECT_SCI5

    /* ---- I/O register settings ---- */
    #define SCIn                SCI5
    #define MSTP_SCIn           MSTP(SCI5)
    #define IPR_SCIn            IPR(SCI5, )
    #define IR_SCIn_ERIn        IR(SCI5,ERI5)
    #define IR_SCIn_RXIn        IR(SCI5,RXI5)
    #define IR_SCIn_TXIn        IR(SCI5,TXI5)
    #define IR_SCIn_TEIn        IR(SCI5,TEI5)
    #define IEN_SCIn_ERIn       IEN(SCI5,ERI5)
    #define IEN_SCIn_RXIn       IEN(SCI5,RXI5)
    #define IEN_SCIn_TXIn       IEN(SCI5,TXI5)
    #define IEN_SCIn_TEIn       IEN(SCI5,TEI5)

    /* ---- Interrupt entry ---- */
    #pragma interrupt (Excep_SCIn_ERIn(vect=234))
    #pragma interrupt (Excep_SCIn_RXIn(vect=235))
    #pragma interrupt (Excep_SCIn_TXIn(vect=236))
    #pragma interrupt (Excep_SCIn_TEIn(vect=237))

#endif

/* ==== SCI6 ==== */
#ifdef SELECT_SCI6

    /* ---- I/O register settings ---- */
    #define SCIn                SCI6
    #define MSTP_SCIn           MSTP(SCI6)
    #define IPR_SCIn            IPR(SCI6, )
    #define IR_SCIn_ERIn        IR(SCI6,ERI6)
    #define IR_SCIn_RXIn        IR(SCI6,RXI6)
    #define IR_SCIn_TXIn        IR(SCI6,TXI6)
    #define IR_SCIn_TEIn        IR(SCI6,TEI6)
    #define IEN_SCIn_ERIn       IEN(SCI6,ERI6)
    #define IEN_SCIn_RXIn       IEN(SCI6,RXI6)
    #define IEN_SCIn_TXIn       IEN(SCI6,TXI6)
    #define IEN_SCIn_TEIn       IEN(SCI6,TEI6)

    /* ---- Interrupt entry ---- */
    #pragma interrupt (Excep_SCIn_ERIn(vect=238))
    #pragma interrupt (Excep_SCIn_RXIn(vect=239))
    #pragma interrupt (Excep_SCIn_TXIn(vect=240))
    #pragma interrupt (Excep_SCIn_TEIn(vect=241))
   
#endif

#endif /* SCI_H */

/* End of File */

