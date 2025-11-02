#include <iicComExe.h>

#define TARGET_SLAVE_ADDRESS		0xA0	/* EEPROM address */
#define EEPROM_ADDRESS_LENGTH		2u		/* EEPROM address length */

uint8_t trm_eeprom_adr[EEPROM_ADDRESS_LENGTH];	/* Buffer for EEPROM addres for write */
uint8_t rcv_eeprom_adr[EEPROM_ADDRESS_LENGTH];	/* Buffer for EEPROM addres for read */
uint8_t trm_buff[12];							/* Buffer for EEPROM for write */
uint8_t rcv_buff[12];							/* Buffer for EEPROM for read */
IIC_API_T iic_buff_prm[2];						/* Structure for IIC function */

// 外部から登録されたコールバック関数を保持
static TxEndCallback g_txEndCb = NULL;

void regTxEnd(TxEndCallback cb) {
    g_txEndCb = cb;
}

/******************************************************************************
* Function Name: SampleEepromWrite
* Description  : Sample Eeprom Write
* Argument     : none
* Return Value : none
******************************************************************************/
void SampleEepromWrite(void)
{
	volatile uint32_t cnt;
	
	enum RiicStatus_t tmp_status;
	enum RiicBusStatus_t tmp_bus_status;
	
	trm_buff[0] = 'a';
	trm_buff[1] = 'b';
	trm_buff[2] = 'c';
	trm_buff[3] = 'd';
	trm_buff[4] = 'e';
	trm_buff[5] = 'f';
	trm_buff[6] = 'g';
	trm_buff[7] = 'h';
	trm_buff[8] = '5';
	trm_buff[9] = '9';
	trm_buff[10] = '6';
	trm_buff[11] = '8';
	
	/* Set the address where the RIIC write date to EEPROM */
	trm_eeprom_adr[0] = 0x00;
	trm_eeprom_adr[1] = 0x00;

	/* Set the parameters for 'IIC_EepWrite' */
	iic_buff_prm[0].SlvAdr = TARGET_SLAVE_ADDRESS;		/* Target slave device */
	iic_buff_prm[0].PreCnt = EEPROM_ADDRESS_LENGTH;		/* EEPROM address length */
	iic_buff_prm[0].pPreData = trm_eeprom_adr;			/* Pointer for EEPROM adress buffer */
	iic_buff_prm[0].RWCnt = 12;							/* Data length of writting data */
	iic_buff_prm[0].pRWData = trm_buff;					/* Pointer for writting data buffer */

	/* Start EEPROM Write */
	if(RIIC_OK != IIC_EepWrite(iic_buff_prm[0]))
	{
		/* This software is for single master 					*/
		/* Therefore, return value should be always 'RIIC_OK'	*/
		while(1){};
	}

	/* Wait for communication complete */
	do{
		IIC_GetStatus(&tmp_status, &tmp_bus_status);
	}while(tmp_status == RIIC_STATUS_ON_COMMUNICATION);

	/* Wait for IIC bus free */
	do{
		IIC_GetStatus(&tmp_status, &tmp_bus_status);
	}while(tmp_bus_status != RIIC_BUS_STATUS_FREE);
}

void EepromWrite(int data, int lAddr, int hAddr)
{
	uint32_t len;
	
	enum RiicStatus_t tmp_status;
	enum RiicBusStatus_t tmp_bus_status;
	
	if (data <= 0xFF) {
	// 1バイトで表せる場合
	trm_buff[0] = (uint8_t)data;
	len = 1;
	} else {
	// 2バイト（上位・下位）で格納
	trm_buff[0] = (data >> 8) & 0xFF; // 上位バイト
	trm_buff[1] = data & 0xFF;        // 下位バイト
	len = 2;
	}

	/* Set the address where the RIIC write date to EEPROM */
	trm_eeprom_adr[0] = lAddr;
	trm_eeprom_adr[1] = hAddr;

	/* Set the parameters for 'IIC_EepWrite' */
	iic_buff_prm[0].SlvAdr = TARGET_SLAVE_ADDRESS;		/* Target slave device */
	iic_buff_prm[0].PreCnt = EEPROM_ADDRESS_LENGTH;		/* EEPROM address length */
	iic_buff_prm[0].pPreData = trm_eeprom_adr;			/* Pointer for EEPROM adress buffer */
	iic_buff_prm[0].RWCnt = len;							/* Data length of writting data */
	iic_buff_prm[0].pRWData = trm_buff;					/* Pointer for writting data buffer */

	/* Start EEPROM Write */
	if(RIIC_OK != IIC_EepWrite(iic_buff_prm[0]))
	{
		/* This software is for single master 					*/
		/* Therefore, return value should be always 'RIIC_OK'	*/
		while(1){};
	}

	/* Wait for communication complete */
	do{
		IIC_GetStatus(&tmp_status, &tmp_bus_status);
	}while(tmp_status == RIIC_STATUS_ON_COMMUNICATION);

	/* Wait for IIC bus free */
	do{
		IIC_GetStatus(&tmp_status, &tmp_bus_status);
	}while(tmp_bus_status != RIIC_BUS_STATUS_FREE);
	
	
}


/******************************************************************************
* Function Name: SampleEepromRead
* Description  : Sample Eeprom Read (Random read)
* Argument     : none
* Return Value : none
******************************************************************************/
void SampleEepromRead(void)
{
	volatile uint32_t cnt;
	enum RiicStatus_t tmp_status;
	enum RiicBusStatus_t tmp_bus_status;

	/* Clear the data buffer */
	for(cnt=0; cnt < (sizeof(rcv_buff)/sizeof(rcv_buff[0])); cnt++){
		rcv_buff[cnt] = 0;
	}

	/* Set the address where the RIIC read date to EEPROM */
	/* In this sample, RIIC read data from address 0x0000 */
	rcv_eeprom_adr[0] = 0x00;
	rcv_eeprom_adr[1] = 0x00;

	/* Set the parameter for 'IIC_RandomRead' */
	iic_buff_prm[1].SlvAdr = TARGET_SLAVE_ADDRESS + 1;		/* Target slave device */
	iic_buff_prm[1].PreCnt = EEPROM_ADDRESS_LENGTH;		/* EEPROM address length */
	iic_buff_prm[1].pPreData = rcv_eeprom_adr;			/* Pointer for EEPROM adress buffer */
	iic_buff_prm[1].RWCnt = 12;							/* Data length of read data */
	iic_buff_prm[1].pRWData = rcv_buff;					/* Pointer for read data buffer */

	/* Start EEPROM Read */
	if(RIIC_OK != IIC_RandomRead(iic_buff_prm[1]))
	{
		/* This software is for single master 					*/
		/* Therefore, return value should be always 'RIIC_OK'	*/
		while(1){};
	}

	/* Wait for communication complete */
	do{
		IIC_GetStatus(&tmp_status, &tmp_bus_status);
	}while(tmp_status == RIIC_STATUS_ON_COMMUNICATION);

	/* Wait for IIC bus free */
	do{
		IIC_GetStatus(&tmp_status, &tmp_bus_status);
	}while(tmp_bus_status != RIIC_BUS_STATUS_FREE);
}

/******************************************************************************
* Function Name: EepromRead
* Description  : Eeprom 1byte Read (Random read)
* Argument     : none
* Return Value : none
******************************************************************************/
void EepromReadAndResponse(int lAddr, int hAddr, int len)
{
	volatile uint32_t cnt;
	uint8_t result;
	enum RiicStatus_t tmp_status;
	enum RiicBusStatus_t tmp_bus_status;

	/* Clear the data buffer */
	for(cnt=0; cnt < (sizeof(rcv_buff)/sizeof(rcv_buff[0])); cnt++){
		rcv_buff[cnt] = 0;
	}

	/* Set the address where the RIIC read date to EEPROM */
	/* In this sample, RIIC read data from address 0x0000 */
	rcv_eeprom_adr[0] = lAddr;
	rcv_eeprom_adr[1] = hAddr;

	/* Set the parameter for 'IIC_RandomRead' */
	iic_buff_prm[1].SlvAdr = TARGET_SLAVE_ADDRESS + 1;		/* Target slave device */
	iic_buff_prm[1].PreCnt = EEPROM_ADDRESS_LENGTH;		/* EEPROM address length */
	iic_buff_prm[1].pPreData = rcv_eeprom_adr;			/* Pointer for EEPROM adress buffer */
	iic_buff_prm[1].RWCnt = len;							/* Data length of read data */
	iic_buff_prm[1].pRWData = rcv_buff;					/* Pointer for read data buffer */

	/* Start EEPROM Read */
	if(RIIC_OK != IIC_RandomRead(iic_buff_prm[1]))
	{
		/* This software is for single master 					*/
		/* Therefore, return value should be always 'RIIC_OK'	*/
		while(1){};
	}

	/* Wait for communication complete */
	do{
		IIC_GetStatus(&tmp_status, &tmp_bus_status);
	}while(tmp_status == RIIC_STATUS_ON_COMMUNICATION);

	/* Wait for IIC bus free */
	do{
		IIC_GetStatus(&tmp_status, &tmp_bus_status);
	}while(tmp_bus_status != RIIC_BUS_STATUS_FREE);
}

void txEndWrapper(int unused) {
    g_txEndCb(rcv_buff[0]);
}

void Test()
{
	uint8_t result;
	rcv_buff[1] = '\n';
	// 応答を送信
	do
	{
		result = SCI_StartTransmit(rcv_buff, (sizeof(rcv_buff) - NULL_SIZE), txEndWrapper);
	}
	while (SCI_BUSY == result);	
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
