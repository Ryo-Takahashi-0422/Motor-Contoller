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
* File Name    : sci.c
* Version      : 1.00
* Device       : R5F562N8BDBG (RX62N Group)
* Abstract     : Serial control program
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
#include <stdint.h>
#include "..\iodefine.h"
#include "sci_cfg.h"
#include "sci.h"

/*******************************************************************************
Macro definitions
*******************************************************************************/
/* SSR - Serial Status Register
b7:b6  Reserved - The read value is undefined. The write value should be 1.
b5     ORER     - Overrun Error Flag    - An overrun error has occurred
b4     FER      - Framing Error Flag    - A framing error has occurred
b3     PER      - Parity Error Flag     - A parity error has occurred */
#define SSR_ERROR_FLAGS     (0x38)

/*******************************************************************************
Private variables and functions
*******************************************************************************/
/* **** SCI state **** */
static sci_state_t  state = {0x00};
#define B_TX_BUSY   state.bit.b_tx_busy     /* Transmission busy flag */
#define B_RX_BUSY   state.bit.b_rx_busy     /* Reception busy flag */
#define B_RX_ORER   state.bit.b_rx_orer     /* Overrun error flag */
#define B_RX_FER    state.bit.b_rx_fer      /* Framing error flag */

/* **** SCI transmit/receive buffer **** */
static const uint8_t * pbuf_tx;             /* Pointer to transmit buffer */
static       uint8_t   tx_cnt;              /* Transmit counter */

static       uint8_t * pbuf_rx;             /* Pointer to receive buffer */
static       uint8_t   rx_cnt;              /* Receive counter */

/* **** Pointer to callback function **** */
CallBackFunc pcb_sci_tx_end;
CallBackFunc pcb_sci_rx_end;
CallBackFunc pcb_sci_rx_error;

/* **** SCI interrupt service routine **** */
static void sci_txi_isr(void);
static void sci_tei_isr(void);
static void sci_rxi_isr(void);
static void sci_eri_isr(void);
static void Excep_SCIn_ERIn(void);
static void Excep_SCIn_RXIn(void);
static void Excep_SCIn_TXIn(void);
static void Excep_SCIn_TEIn(void);

volatile uint8_t last_received_byte;
volatile uint8_t rx_index = 0;
volatile uint8_t data_received_flag = 0;

/*******************************************************************************
* Outline      : User I/F function (Initialization of SCI)
* Header       : sci.h
* Function Name: SCI_Init
* Description  : Initialize SCI.
* Arguments    : none
* Return Value : none
*******************************************************************************/
void SCI_Init (void)
{

    volatile uint16_t wait_cnt; 

    /* ---- SCI interrupt request is disabled ---- */
    IEN_SCIn_ERIn = 0;
    IEN_SCIn_RXIn = 0;
    IEN_SCIn_TXIn = 0;
    IEN_SCIn_TEIn = 0;

    /* ---- Initialization of SCI ---- */

    /* The module stop state of SCIn is canceled */
    MSTP_SCIn = 0;

    /* SCR - Serial Control Register
    b7     TIE  - Transmit Interrupt Enable     - A TXI interrupt request is disabled
    b6     RIE  - Receive Interrupt Enable      - RXI and ERI interrupt requests are disabled
    b5     TE   - Transmit Enable               - Serial transmission is disabled
    b4     RE   - Receive Enable                - Serial reception is disabled
    b2     TEIE - Transmit End Interrupt Enable - A TEI interrupt request is disabled */
    SCIn.SCR.BYTE = 0x00;

    while (0x00 != (SCIn.SCR.BYTE & 0xF0))
    {
        /* Confirm that bit is actually 0 */
    }

    /* ---- Set the I/O port functions ---- */

    /* PFS - Pin Function Control Register
    b7    Reserved - The write value should be 0.
    b6    SCI6S    - SCI6 Pin Select - P01 is designated as the RxD6-A pin
                                       P02 is designated as the SCK6-A pin
                                       P00 is designated as the TxD6-A pin
    b5:4  Reserved - The write value should be 0.
    b3    SCI3S    - SCI3 Pin Select - P16 is designated as the RxD3-A pin
                                       P15 is designated as the SCK3-A pin
                                       P17 is designated as the TxD3-A pin
    b2    SCI2S    - SCI2 Pin Select - P12 is designated as the RxD2-A pin
                                       P11 is designated as the SCK2-A pin
                                       P13 is designated as the TxD2-A pin
    b1    SCI1S    - SCI1 Pin Select - P30 is designated as the RxD1-A pin
                                       P27 is designated as the SCK1-A pin
                                       P26 is designated as the TxD1-A pin
    b0    Reserved - The write value should be 0. */
    IOPORT.PFFSCI.BYTE = 0x00;

    /* ICR - Input Buffer Control Register
    b2     B2 - Pn12 Input Buffer Control - Input buffer is enabled */
    //PORT1.ICR.BIT.B2 = 1;
    // 周辺モジュールの入力端子として使用する場合は、あらかじめ、対応する端子の入力バッファを有効にするためにPORTn.ICRを“1”にする必要があります
    // →周辺モジュールの入力端子 = RXD0
    PORT2.ICR.BIT.B1 = 1;

    /* ---- Initialization of SCI ---- */

    /* Select an On-chip baud rate generator to the clock source */
    SCIn.SCR.BIT.CKE = 1;

    /* SMR - Serial Mode Register
    b7     CM   - Communications Mode   - Asynchronous mode
    b6     CHR  - Character Length      - Selects 8 bits as the data length
    b5     PE   - Parity Enable         - When transmitting : Parity bit addition is not performed
                                          When receiving    : Parity bit checking is not performed
    b3     STOP - Stop Bit Length       - 2 stop bits
    b2     MP   - Multi-Processor Mode  - Multi-processor communications function is disabled
    b1:b0  CKS  - Clock Select          - PCLK clock (n = 0) */
    SCIn.SMR.BYTE = 0x08;

    /* SCMR - Smart Card Mode Register
    b6:b4  Reserved - The write value should be 1.
    b3     SDIR     - Transmitted/Received Data Transfer Direction - Transfer with LSB-first
    b2     SINV     - Transmitted/Received Data Invert  - TDR contents are transmitted as they are. 
                                                          Receive data is stored as it is in RDR.
    b1     Reserved - The write value should be 1.
    b0     SMIF     - Smart Card Interface Mode Select  - Serial communications interface mode */
    SCIn.SCMR.BYTE = 0xF2;

    /* SEMR - Serial Extended Mode Register
    b7:b5  Reserved - The write value should be 0.
    b4     ABCS     - Asynchronous Mode Base Clock Select   - Selects 16 base clock cycles for 1-bit period
    b3:b1  Reserved - The write value should be 0. */
    SCIn.SEMR.BYTE = 0x00;

    /* BRR - Bit Rate Register
    Bit Rate: (48MHz/(64*2^(-1)*57600bps))-1 = 25.04 */
    //SCIn.BRR = 26;
    
    // 57600→9600で再計算 BlueToothに合わせる156.25→157
    SCIn.BRR = 157;

    /* 1-bit interval elapsed
    1-bit interval : 1/57600bps = 17.36us
    1-clock        : 1/96MHz = 0.01us
    Wait Cycle     : 17.36us/0.01us = 1736 */
    for (wait_cnt = 0; wait_cnt < 1736; wait_cnt++)
    {
        /* Wait 1736 cycle */
    }

    /* ---- Initialization of SCI interrupt ---- */

    /* SCI interrupt priority level is 1 */
    IPR_SCIn = 1;

    /* Interrupt request is cleared (Edge interrupt) */
    IR_SCIn_RXIn = 0;
    IR_SCIn_TXIn = 0;

    /* ---- Enable serial reception and transmission ---- */

    /* RXI and ERI interrupt requests are enabled */
    SCIn.SCR.BIT.RIE = 1;

    /* A TXI interrupt request is enabled */
    SCIn.SCR.BIT.TIE = 1;

    /* Serial transmission and reception are enabled */
    SCIn.SCR.BYTE = SCIn.SCR.BYTE | 0x30;            /* Setting the RE bit allows RXDn to be used.
    //↑と同意                                                    Setting the TE bit allows TXDn to be used. */
    //SCIn.SCR.BIT.TE = 1;
    //SCIn.SCR.BIT.RE = 1;
}

/*******************************************************************************
* Outline      : User I/F function (SCI reception start)
* Header       : sci.h
* Function Name: SCI_StartReceive
* Description  : Start SCI reception.
* Arguments    : pbuf -
*                   Pointer to where to store the received data
*                num -
*                   Number of receive byte
*                pcb_rx_end -
*                   Pointer to callback function (SCI reception end)
*                pcb_rx_error -
*                   Pointer to callback function (SCI receive error)
* Return Value : SCI_NG    - 
*                   Parameter error
*                SCI_BUSY  -
*                   Reception busy
*                SCI_OK    -
*                   Reception start
*******************************************************************************/
uint8_t SCI_StartReceive (uint8_t * pbuf, uint8_t num, CallBackFunc pcb_rx_end, CallBackFunc pcb_rx_error)
{

    uint8_t return_value = SCI_NG;

    /* **** Check the parameter **** */
    if (0 == num)
    {
        return (return_value);
    }

    /* **** Check the reception state **** */
    if (1 == B_RX_BUSY)
    {
        /* ==== Busy ==== */
        return_value = SCI_BUSY;
    }
    else
    {
        /* ==== Ready ==== */

        /* ---- Set the RAM ---- */
        B_RX_BUSY        = 1;
        B_RX_ORER        = 0;
        B_RX_FER         = 0;
        pbuf_rx          = pbuf;
        rx_cnt           = num;
        pcb_sci_rx_end   = pcb_rx_end;
        pcb_sci_rx_error = pcb_rx_error;

        /* ---- Enable ERI and RXI interrupt requests ---- */
        IEN_SCIn_ERIn = 1;
        IEN_SCIn_RXIn = 1;

        return_value = SCI_OK;
    }

    return (return_value);

}

/*******************************************************************************
* Outline      : User I/F function (SCI transmission start)
* Header       : sci.h
* Function Name: SCI_StartTransmit
* Description  : Start SCI transmission.
* Arguments    : pbuf -
*                   Pointer to where to store the transmit data
*                num  -
*                   Number of transmit byte
*                pcb_tx_end -
*                   Pointer to callback function (SCI transmission end)
* Return Value : SCI_NG   - 
*                   Parameter error
*                SCI_BUSY -
*                   Transmission busy
*                SCI_OK   -
*                   Transmission start
*******************************************************************************/
uint8_t SCI_StartTransmit (const uint8_t * pbuf, uint8_t num, CallBackFunc pcb_tx_end)
{

    uint8_t return_value = SCI_NG;
    

    /* **** Check the parameter **** */
    if (0 == num)
    {
        return (return_value);
    }

    /* **** Check the reception state **** */
    if (1 == B_TX_BUSY)
    {
        /* ==== Busy ==== */
        return_value = SCI_BUSY;
    }
    else
    {
        /* ==== Ready ==== */

        /* ---- Set the RAM ---- */
        B_TX_BUSY      = 1;
        pbuf_tx        = pbuf;
        tx_cnt         = num;
        pcb_sci_tx_end = pcb_tx_end;

        /* ---- Enable TXI interrupt request ---- */
        IEN_SCIn_TXIn = 1;

        return_value = SCI_OK;
    }

    return (return_value);

}

/*******************************************************************************
* Outline      : User I/F function (Get SCI state)
* Header       : sci.h
* Function Name: SCI_GetState
* Description  : Return the SCI state.
* Arguments    : none
* Return Value : sci_state_t.bit.b_tx_busy - Transmission busy flag
*                   0: Transmission is ready
*                   1: Transmission is busy
*                sci_state_t.bit.b_rx_busy - Reception busy flag
*                   0: Reception is ready
*                   1: Reception is busy
*                sci_state_t.bit.b_rx_orer - Overrun error flag
*                   0: No error
*                   1: Overrun error
*                sci_state_t.bit.b_rx_fer  - Framing error flag
*                   0: No error
*                   1: Framing error
*******************************************************************************/
sci_state_t SCI_GetState (void)
{

    return (state);

}

/*******************************************************************************
* Outline      : Transmit buffer empty interrupt
* Header       : none
* Function Name: sci_txi_isr
* Description  : Called in TXI interrupt processing. Write transmit data.
*                When transmit the last data, Disable TXI and enable TEI interrupt request.
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void sci_txi_isr (void)
{

    /* ==== Write transmit data to TDR register ==== */
    SCIn.TDR = *pbuf_tx;

    if (0 == SCIn.SSR.BIT.TEND)
    {
        /* Dummy read the SSR register */
    }

    pbuf_tx++;
    tx_cnt--;

    /* ==== When transmit the last data ==== */
    if (0 == tx_cnt)
    {
        /* ---- Disable TXI interrupt request ---- */
        IEN_SCIn_TXIn    = 0;

        /* ---- Enable TEI interrupt request ---- */
        SCIn.SCR.BIT.TEIE = 1;
        IEN_SCIn_TEIn     = 1;
    }

}

/*******************************************************************************
* Outline      : Transmit end interrupt
* Header       : none
* Function Name: sci_tei_isr
* Description  : Called in TEI interrupt processing. Disable serial transmission.
*                Call the Callback function (SCI transmission end).
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void sci_tei_isr (void)
{

    /* ---- Disable TEI interrupt request ---- */
    IEN_SCIn_TEIn     = 0;
    SCIn.SCR.BIT.TEIE = 0;

    while (0 != SCIn.SCR.BIT.TEIE)
    {
        /* Confirm that bit is actually 0 */
    }

    /* ---- Clear the transmission busy flag ---- */
    B_TX_BUSY = 0;

    /* ---- Callback function ---- */
    pcb_sci_tx_end();

}

/*******************************************************************************
* Outline      : Receive data full interrupt
* Header       : none
* Function Name: sci_rxi_isr
* Description  : Called in RXI interrupt processing. Store the received data.
*                When received the last data, disable serial reception and
*                Call the Callback function (SCI reception end).
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void sci_rxi_isr (void)
{

//    /* ==== Store the received data ==== */
//    *pbuf_rx = SCIn.RDR;
//    pbuf_rx++;
//    rx_cnt--;

//    /* ==== When received the last data ==== */
//    if (0 == rx_cnt)
//    {
//        /* ---- Disable ERI and RXI interrupt requests ---- */
//        IEN_SCIn_RXIn = 0;
//        IEN_SCIn_ERIn = 0;

//        /* RXI interrupt request is cleared */
//        IR_SCIn_RXIn = 0;

//        /* ---- Clear the reception busy flag ---- */
//        B_RX_BUSY = 0;

//        /* ---- Callback function ---- */
//        pcb_sci_rx_end();
//    }
    
    //last_received_byte = SCIn.RDR;
    //*pbuf_rx = last_received_byte;
    *pbuf_rx = SCIn.RDR;
    last_received_byte = *pbuf_rx;
    pbuf_rx++;
    rx_cnt--;

    // \rの次の文字を読んでしまう→\nなので、受信完了コールバック呼び出しが2度立て続けに呼ばれACK*2になっている。
    if (last_received_byte == '\r')// || last_received_byte == '\n')
    {
        // ここで受信完了とみなす
        // 余りのカウントは無視して割り込み停止
        IEN_SCIn_RXIn = 0;
        IEN_SCIn_ERIn = 0;
        IR_SCIn_RXIn = 0;
        B_RX_BUSY = 0;

        // 受信完了コールバック呼び出し
        pcb_sci_rx_end();

        // 終端文字を入れるのはコールバック内で
    }
    
    
    
//    else if (rx_cnt == 0)
//    {
//        // バッファ満タンになったら無理やり完了処理（安全対策）
//        IEN_SCIn_RXIn = 0;
//        IEN_SCIn_ERIn = 0;
//        IR_SCIn_RXIn = 0;
//        B_RX_BUSY = 0;

//        pcb_sci_rx_end();
//    }

}

/*******************************************************************************
* Outline      : Receive error interrupt
* Header       : none
* Function Name: sci_eri_isr
* Description  : Called in ERI interrupt processing. Disable serial reception.
*                Call the Callback function (SCI receive error).
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void sci_eri_isr (void)
{

    volatile uint8_t dummy;

    /* ==== Error processing ==== */

    /* ---- Overrun error ---- */
    if (1 == SCIn.SSR.BIT.ORER)
    {
        B_RX_ORER = 1;
    }

    /* ---- Framing error ---- */
    if (1 == SCIn.SSR.BIT.FER)
    {
        B_RX_FER = 1;
    }

    /* Dummy-read the RDR register */
    dummy = SCIn.RDR;

    /* ---- Disable serial transmission and reception ---- */
    SCIn.SCR.BYTE = SCIn.SCR.BYTE & 0xCF;

    while (0 != (SCIn.SCR.BYTE & 0x30))
    {
        /* Confirm that bit is actually 0 */
    }

    /* ---- Clear the error flag ---- */
    SCIn.SSR.BYTE = (SCIn.SSR.BYTE & ~SSR_ERROR_FLAGS) | 0xC0;

    while (0x00 != (SCIn.SSR.BYTE & SSR_ERROR_FLAGS))
    {
        /* Confirm that bit is actually 0 */
    }

    /* ---- Disable ERI and RXI interrupt requests ---- */
    IEN_SCIn_ERIn    = 0;
    IEN_SCIn_RXIn    = 0;
    SCIn.SCR.BIT.RIE = 0;

    while (0 != SCIn.SCR.BIT.RIE)
    {
        /* Confirm that bit is actually 0 */
    }

    /* ---- Disable TXI and TEI interrupt requests ---- */
    IEN_SCIn_TXIn    = 0;
    SCIn.SCR.BIT.TIE = 0;

    while (0 != SCIn.SCR.BIT.TIE)
    {
        /* Confirm that bit is actually 0 */
    }

    IEN_SCIn_TEIn     = 0;
    SCIn.SCR.BIT.TEIE = 0;
    
    while (0 != SCIn.SCR.BIT.TEIE)
    {
        /* Confirm that bit is actually 0 */
    }

    /* RXI interrupt request is cleared */
    IR_SCIn_RXIn = 0;

    /* TXI interrupt request is cleared */
    IR_SCIn_TXIn = 0;

    /* ---- Clear busy flag ---- */
    B_RX_BUSY = 0;
    B_TX_BUSY = 0;

    /* ---- Callback function ---- */
    pcb_sci_rx_error();
    
    // オーバーランエラー状態解除→ICUとSCIによる受信割り込み、受信エラー割り込み要求を許可する。上述のコードで禁止されたものを復活させて、再び受信可能な状態にしている。
    B_RX_ORER = 0;
    SCIn.SCR.BIT.RE = 1; // シリアル受信動作を許可
    // ↓IER_SCI0_ERI0,IER_SCI0_RXI0は隣り合うbitであることが分かる
    IEN_SCIn_ERIn    = 1; // ICU.IER[0x1A].BIT.IEN6 IER_SCI0_ERI0 = 0x1A, IEN_SCI0_ERI0 = IEN6　つまり、SCI0_ERI0の割り込みコントローラ(ICU)における割り込み要求許可ﾚｼﾞｽﾀ(IER)はICU.IER[0x1A]における7bit目を1にすれば、割り込み許可(IEN)状態となる
    IEN_SCIn_RXIn    = 1; // ICU.IER[0x1A].BIT.IEN7 IER_SCI0_RXI0 = 0x1A, IEN_SCI0_RXI0 = IEN7　つまり、SCI0_RXI0の割り込みコントローラ(ICU)における割り込み要求許可ﾚｼﾞｽﾀ(IER)はICU.IER[0x1A]における8bit目を1にすれば、割り込み許可(IEN)状態となる
    SCIn.SCR.BIT.RIE = 1; // RXIおよびERI割り込み要求を許可

}

/*******************************************************************************
* Outline      : SCIn.ERIn interrupt processing (Level interrupt)
* Header       : none
* Function Name: Excep_SCIn_ERIn
* Description  : Perform SCIn.ERIn interrupt processing.
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void Excep_SCIn_ERIn (void)
{

    do
    {
        /* Confirm interrupt request source */
        while ((0 != SCIn.SCR.BIT.RIE) && (0x00 != (SCIn.SSR.BYTE & SSR_ERROR_FLAGS)))
        {
            /* Clear interrupt request source and Perform interrupt request source processing */
            sci_eri_isr();
        }
    }
    /* Confirm IR flag */
    while (0 != IR_SCIn_ERIn);

}

/*******************************************************************************
* Outline      : SCIn.RXIn interrupt processing (Edge interrupt)
* Header       : none
* Function Name: Excep_SCIn_RXIn
* Description  : Perform SCIn.RXIn interrupt processing.
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void Excep_SCIn_RXIn (void)
{

    sci_rxi_isr();

}

/*******************************************************************************
* Outline      : SCIn.TXIn interrupt processing (Edge interrupt)
* Header       : none
* Function Name: Excep_SCIn_TXIn
* Description  : Perform SCIn.TXIn interrupt processing.
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void Excep_SCIn_TXIn (void)
{

    sci_txi_isr();

}

/*******************************************************************************
* Outline      : SCIn.TEIn interrupt processing (Level interrupt)
* Header       : none
* Function Name: Excep_SCIn_TEIn
* Description  : Perform SCIn.TEIn interrupt processing.
* Arguments    : none
* Return Value : none
*******************************************************************************/
static void Excep_SCIn_TEIn (void)
{

    do
    {
        /* Confirm interrupt request source */
        while ((0 != SCIn.SCR.BIT.TEIE) && (0 != SCIn.SSR.BIT.TEND))
        {
            /* Clear interrupt request source and Perform interrupt request source processing */
            sci_tei_isr();
        }
    }
    /* Confirm IR flag */
    while (0 != IR_SCIn_TEIn);

}

/* End of File */

