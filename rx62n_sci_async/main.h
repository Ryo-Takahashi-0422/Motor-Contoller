#pragma once
#include <machine.h>
#include <stdint.h>
#include "..\iodefine.h"
#include "sci.h"
#include <stdbool.h>
#include "r_apn_iic_eep.h"
#include <command.h>
#include <iicComExe.h>
#include <common.h>

/*******************************************************************************
Macro definitions
*******************************************************************************/
/* **** LEDs **** */
#define LED2_REG_PODR   PORT0.DR.BIT.B5     /* P05: Output data store bit */
#define LED2_REG_PDR    PORT0.DDR.BIT.B5    /* P05: I/O select bit */

/* **** LEDs output data **** */
#define LED_ON          (0)                 /* LED on */
#define LED_OFF         (1)                 /* LED off */

/* **** Buffer **** */
#define BUF_SIZE        (32)                /* Buffer size */
#define NULL_SIZE       (1)                 /* Null code size */

/* **** Riic **** */
#define TARGET_SLAVE_ADDRESS		0xA0	/* EEPROM address */

/* **** MTU **** */
#define		PSW_init  0x00010000
#define		TGRB_INIT_VAL	7500				// Interval=20ms(PCLK/64)
#define		DUTY_VAL	TGRB_INIT_VAL / 100		// Duty:10%
#define		TGRA_INIT_VAL	4687				// Interval=50ms(PCLK/256)
#define		MINIMUM_TGRA_VAL	0x200			// Interval=2.6ms(PCLK/128)

/* **** SCI state **** */
static sci_state_t      sci_state = {0x00};
#define SCI_B_TX_BUSY   sci_state.bit.b_tx_busy     /* Transmission busy flag */
#define SCI_B_RX_BUSY   sci_state.bit.b_rx_busy     /* Reception busy flag */
#define SCI_B_RX_ORER   sci_state.bit.b_rx_orer     /* Overrun error flag */
#define SCI_B_RX_FER    sci_state.bit.b_rx_fer      /* Framing error flag */


static uint8_t rx_buf[BUF_SIZE];
static uint8_t tx_buf[] = {"ACK\n"};

/*******************************************************************************
Method definitions
*******************************************************************************/

void main(void);

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

/*******************************************************************************
variable definitions
*******************************************************************************/

char isRbuffFull;
int received_value = 1;
// I2C�������݃e�X�g�p
uint8_t write_data[3];
// I2C �ǂݍ��݃e�X�g�p
uint8_t addr[2];
bool isReadI2C = false;
uint8_t tr, rcv;

signed short Under_over_flow_cnt; 	/* MTU2 u1 ch7 underflow/overflow counter */
struct MotorParams motorParams = {0, 0.0f, 0.0f, 0.0f};

bool isGetData = false;
char lastCmd[8] = {0};