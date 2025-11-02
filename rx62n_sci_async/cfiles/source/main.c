#include <main.h>

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
    //SampleEepromWrite();

    /* Acknowledge polling (Master transfer) */
    //IICAckPolling(TARGET_SLAVE_ADDRESS, 0, 100);  

    /* EEPROM Read (Master transfer and Master receive) */
    //SampleEepromRead();
    
    // TODO : 速度の読み込み
    // redコマンドを実装する
    // 引数にはL/Hアドレスが必要
    // EEPROM読み込み→sci translateで送信する

    /* ==== Initialize peripheral functions ==== */
    peripheral_init();

    /* **** Initialize RAM **** */
    for (cnt = 0; cnt < BUF_SIZE; cnt++)
    {
        rx_buf[cnt] = 0; 
    }
    
    // モータドライバin1を0に
    // in2はMTIOC0BからPWMを受け取る
    R_PG_IO_PORT_Write_PE0(0);
    //R_PG_IO_PORT_Write_PE1(0);

    /* **** Start receive **** */
    do
    {
        result = SCI_StartReceive(rx_buf, BUF_SIZE, cb_sci_rx_end, cb_sci_rx_error);
    }
    while (SCI_BUSY == result);
    
    // 送信完了コールバック関数を登録しておく
    regTxEnd(txEndWithMorterspd);

    /* **** Main loop **** */
    while (1)
    {	    // isRbuffFull→通信完了的な名前に変える
	    if(isRbuffFull)
	    {		    
		result = SCI_StartReceive(rx_buf, BUF_SIZE, cb_sci_rx_end, cb_sci_rx_error);
		isRbuffFull = 0;		    
	    }
	    
	    // EEPROM書き読み確認
	    // 速度指示あり→EEPROM書き込み→読み込んで内容確認
	    if(isGetData)
	    {
		if(strcmp(lastCmd, "spd") == 0)
		{
		    EepromWrite(motorParams.speed, 0x00, 0x00);
	            IICAckPolling(TARGET_SLAVE_ADDRESS, 0, 100);
		    SampleEepromRead();
		    //IICAckPolling(TARGET_SLAVE_ADDRESS, 0, 100);
		}
	    	isGetData = false;
	    }
	    
	        // --- 前回の関数ポインタが設定されていれば実行 ---
	    if (nextFunc != NULL && isProcessNextFunc)
	    {
		isProcessNextFunc = false;
	        nextFunc(nextFuncVal, &motorParams);
		IICAckPolling(TARGET_SLAVE_ADDRESS, 0, 100);
	        nextFunc = NULL;
		//isRbuffFull = 1;
		Test();
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
    isRbuffFull = 1;
}

static void txEndWithMorterspd(int val)
{
    isRbuffFull = 1;
    received_value = val;
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
    // 受信バッファがフルの状態でこの割り込みが発生している
    // この処理中に新たなデータが送信(tera turmで文字入力)されると、
    uint8_t result;
    char cmd[8] = {0};
    int val = 0;
    const CommandEntry *entry;
    CommandFunc returned;
    int i = 0;
       
    // 構造体でspd,pid値などを一元管理/コマンドにもその構造体ポインタ渡して
    // こちらに返す&EEPROMへの保存もそのポインタデータに基づき行う設計でいく
        
    // 受信文字列を解析して実行
    // 改行・復帰コードを除去
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
    
    // 1. コマンド部分をコピー
    while (i < received_bytes && rx_buf[i] != ',' && j < sizeof(cmd)-1) {
        cmd[j++] = rx_buf[i++];
    }
    cmd[j] = '\0';

    // 2. カンマをスキップ
    if (i < received_bytes && rx_buf[i] == ',') i++;

    // 3. 数字部分を解析
    val = 0;
    while (i < received_bytes && rx_buf[i] >= '0' && rx_buf[i] <= '9') {
        val = val * 10 + (rx_buf[i] - '0');
        i++;
    }
    
    // --- 前回の関数ポインタが設定されていれば実行 ---
    if (nextFunc != NULL)
    {
	nextFuncVal = val;
	isProcessNextFunc = true;
//        nextFunc(val, &motorParams);
//        nextFunc = NULL;
        //isRbuffFull = 1;
        return;
    }

    // 前回の関数ポインタが設定されていなければコマンド検索
    entry = findCommand(cmd);
    if (entry && entry->func) {
        returned = entry->func(val, &motorParams);  // 第1引数 'A' は任意
	if (returned != NULL) {
            nextFunc = returned;   // CommandReadRomを登録
        }
	
        received_value = motorParams.speed;
        // コマンド記憶、データ受信フラグオン
        isGetData = true;
        memcpy(lastCmd, cmd, sizeof(cmd));
    } 
    else{}

    memset(rx_buf, 0, sizeof(rx_buf));
    
    // 応答を送信
    do
    {
        result = SCI_StartTransmit(tx_buf, (sizeof(tx_buf) - NULL_SIZE), cb_sci_tx_end);
    }
    while (SCI_BUSY == result);
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

/* End of File */

