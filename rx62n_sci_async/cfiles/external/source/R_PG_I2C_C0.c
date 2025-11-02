/******************************************************************************
* DISCLAIMER

* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized.

* This software is owned by Renesas Electronics Corporation and is protected under 
* all applicable laws, including copyright laws.

* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, 
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
* PARTICULAR PURPOSE AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY 
* DISCLAIMED.

* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES 
* FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS 
* AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

* Renesas reserves the right, without notice, to make changes to this 
* software and to discontinue the availability of this software.  
* By using this software, you agree to the additional terms and 
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
******************************************************************************
* Copyright (C) 2010-2013 Renesas Electronics Corporation.
* and Renesas Solutions Corporation. All rights reserved.
******************************************************************************
* File Name    : R_PG_I2C_C0.c
* Version      : 1.00
* Device(s)    : 
* Tool-Chain   : 
* H/W Platform : 
* Description  : 
* Limitations  : 
******************************************************************************
* History : 05.10.2025 Version Description
*         :   
******************************************************************************/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "R_PG_I2C_C0.h"


/******************************************************************************
* ID           : 
*
* Include      : 
*
* Declaration  : bool R_PG_I2C_Set_C0(void)
*
* Function Name: R_PG_I2C_Set_C0
*
* Description  : I2Cバスインタフェースチャネルの設定
*
* Arguments    : なし
*
* Return Value : true  : 設定が正しく行われた場合
*              : false : 設定に失敗した場合
*
* Calling Functions : R_IIC_Create
*
* Details      : 詳細についてはリファレンスマニュアルを参照してください。
******************************************************************************/
bool R_PG_I2C_Set_C0(void)
{
	return R_IIC_Create(
		0,
		PDL_IIC_MODE_IIC | PDL_IIC_INT_PCLK_DIV_8 | PDL_IIC_TIMEOUT_DISABLE | PDL_IIC_SDA_DELAY_0 | PDL_IIC_NF_DISABLE,
		PDL_IIC_NTALD_DISABLE | PDL_IIC_SALD_DISABLE | PDL_IIC_SLAVE_0_DISABLE | PDL_IIC_SLAVE_1_DISABLE | PDL_IIC_SLAVE_2_DISABLE | PDL_IIC_SLAVE_GCA_DISABLE | PDL_IIC_DEVICE_ID_DISABLE | PDL_IIC_HOST_ADDRESS_DISABLE,
		0,
		0,
		0,
		( 1 << 31 ) | ( 22 << 8 ) | 22,
		0
	);

}

/******************************************************************************
* ID           : 
*
* Include      : 
*
* Declaration  : bool R_PG_I2C_MasterReceive_C0(bool addr_10bit, uint16_t slave, uint8_t * data, uint16_t count)
*
* Function Name: R_PG_I2C_MasterReceive_C0
*
* Description  : マスタのデータ受信
*
* Arguments    : bool addr_10bit : スレーブアドレスフォーマット (1:10ビット  0:7ビット)
*              : uint16_t slave : スレーブアドレス
*              : uint8_t * data : 受信したデータの格納先の先頭のアドレス
*              : uint16_t count : 受信するデータ数
*
* Return Value : true  : 設定が正しく行われた場合
*              : false : 設定に失敗した場合
*
* Calling Functions : R_IIC_MasterReceive
*
* Details      : 詳細についてはリファレンスマニュアルを参照してください。
******************************************************************************/
bool R_PG_I2C_MasterReceive_C0(bool addr_10bit, uint16_t slave, uint8_t * data, uint16_t count)
{
	uint16_t addr_size = PDL_NO_DATA;

	if( addr_10bit )
	{
		addr_size = PDL_IIC_10_BIT_SLAVE_ADDRESS;
	}
	
	if( data == 0 )
	{
		return false;
	}

	return R_IIC_MasterReceive(
		0,
		addr_size | PDL_IIC_DMAC_DTC_TRIGGER_DISABLE,
		slave,
		data,
		count,
		IIC0MasterReFunc,
		15
	);

}

/******************************************************************************
* ID           : 
*
* Include      : 
*
* Declaration  : bool R_PG_I2C_MasterSend_C0(bool addr_10bit, uint16_t slave, uint8_t * data, uint16_t count)
*
* Function Name: R_PG_I2C_MasterSend_C0
*
* Description  : マスタのデータ送信
*
* Arguments    : bool addr_10bit : スレーブアドレスフォーマット (1:10ビット  0:7ビット)
*              : uint16_t slave : スレーブアドレス
*              : uint8_t * data : 送信するデータの格納先の先頭のアドレス
*              : uint16_t count : 送信するデータ数
*
* Return Value : true  : 設定が正しく行われた場合
*              : false : 設定に失敗した場合
*
* Calling Functions : R_IIC_MasterSend
*
* Details      : 詳細についてはリファレンスマニュアルを参照してください。
******************************************************************************/
bool R_PG_I2C_MasterSend_C0(bool addr_10bit, uint16_t slave, uint8_t * data, uint16_t count)
{
	uint16_t addr_size = PDL_NO_DATA;

	if( addr_10bit )
	{
		addr_size = PDL_IIC_10_BIT_SLAVE_ADDRESS;
	}
	
	if( data == 0 )
	{
		return false;
	}

	return R_IIC_MasterSend(
		0,
		PDL_IIC_START_ENABLE | PDL_IIC_STOP_ENABLE | addr_size | PDL_IIC_DMAC_DTC_TRIGGER_DISABLE,
		slave,
		data,
		count,
		IIC0MasterTrFunc,
		15
	);

}

/******************************************************************************
* ID           : 
*
* Include      : 
*
* Declaration  : bool R_PG_I2C_MasterSendWithoutStop_C0(bool addr_10bit, uint16_t slave, uint8_t * data, uint16_t count)
*
* Function Name: R_PG_I2C_MasterSendWithoutStop_C0
*
* Description  : マスタのデータ送信 (STOP条件無し)
*
* Arguments    : bool addr_10bit : スレーブアドレスフォーマット (1:10ビット  0:7ビット)
*              : uint16_t slave : スレーブアドレス
*              : uint8_t * data : 送信するデータの格納先の先頭のアドレス
*              : uint16_t count : 送信するデータ数
*
* Return Value : true  : 設定が正しく行われた場合
*              : false : 設定に失敗した場合
*
* Calling Functions : R_IIC_MasterSend
*
* Details      : 詳細についてはリファレンスマニュアルを参照してください。
******************************************************************************/
bool R_PG_I2C_MasterSendWithoutStop_C0(bool addr_10bit, uint16_t slave, uint8_t * data, uint16_t count)
{
	uint16_t addr_size = PDL_NO_DATA;

	if( addr_10bit )
	{
		addr_size = PDL_IIC_10_BIT_SLAVE_ADDRESS;
	}
	
	if( data == 0 )
	{
		return false;
	}

	return R_IIC_MasterSend(
		0,
		PDL_IIC_START_ENABLE | PDL_IIC_STOP_DISABLE | addr_size | PDL_IIC_DMAC_DTC_TRIGGER_DISABLE,
		slave,
		data,
		count,
		IIC0MasterTrFunc,
		15
	);

}

/******************************************************************************
* ID           : 
*
* Include      : 
*
* Declaration  : bool R_PG_I2C_GenerateStopCondition_C0(void)
*
* Function Name: R_PG_I2C_GenerateStopCondition_C0
*
* Description  : マスタのSTOP条件生成
*
* Arguments    : なし
*
* Return Value : true  : 設定が正しく行われた場合
*              : false : 設定に失敗した場合
*
* Calling Functions : R_IIC_Control
*
* Details      : 詳細についてはリファレンスマニュアルを参照してください。
******************************************************************************/
bool R_PG_I2C_GenerateStopCondition_C0(void)
{
	return R_IIC_Control(
		0,
		PDL_IIC_STOP
	);

}

/******************************************************************************
* ID           : 
*
* Include      : 
*
* Declaration  : bool R_PG_I2C_GetBusState_C0(bool * busy)
*
* Function Name: R_PG_I2C_GetBusState_C0
*
* Description  : バス状態の取得
*
* Arguments    : bool * busy : バスビジー検出フラグの格納先
*
* Return Value : true  : 取得に成功した場合
*              : false : 取得に失敗した場合
*
* Calling Functions : R_IIC_GetStatus
*
* Details      : 詳細についてはリファレンスマニュアルを参照してください。
******************************************************************************/
bool R_PG_I2C_GetBusState_C0(bool * busy)
{
	uint32_t data;
	bool res;

	res = R_IIC_GetStatus(
		0,
		&data,
		PDL_NO_PTR,
		PDL_NO_PTR
	);

	if( busy ){ *busy = (data >> 15) & 0x01; }

	return res;
}

/******************************************************************************
* ID           : 
*
* Include      : 
*
* Declaration  : bool R_PG_I2C_GetEvent_C0(bool * nack, bool * stop, bool * start, bool * lost, bool * timeout)
*
* Function Name: R_PG_I2C_GetEvent_C0
*
* Description  : 検出イベントの取得
*
* Arguments    : bool * nack : NACK検出フラグ格納先
*              : bool * stop : STOP条件検出フラグ格納先
*              : bool * start : START条件検出フラグ格納先
*              : bool * lost : アービトレーションロロスト検出フラグ格納先
*              : bool * timeout : タイムアウト検出フラグ格納先
*
* Return Value : true  : 取得に成功した場合
*              : false : 取得に失敗した場合
*
* Calling Functions : R_IIC_GetStatus
*
* Details      : 詳細についてはリファレンスマニュアルを参照してください。
******************************************************************************/
bool R_PG_I2C_GetEvent_C0(bool * nack, bool * stop, bool * start, bool * lost, bool * timeout)
{
	uint32_t data;
	bool res;

	res = R_IIC_GetStatus(
		0,
		&data,
		PDL_NO_PTR,
		PDL_NO_PTR
	);

	if( nack ){
		*nack = (data >> 12) & 0x01;
	}
	if( stop ){
		*stop = (data >> 11) & 0x01;
	}
	if( start ){
		*start = (data >> 10) & 0x01;
	}
	if( lost ){
		*lost = (data >> 9) & 0x01;
	}
	if( timeout ){
		*timeout = (data >> 8) & 0x01;
	}

	return res;
}

/******************************************************************************
* ID           : 
*
* Include      : 
*
* Declaration  : bool R_PG_I2C_GetReceivedDataCount_C0(uint16_t * count)
*
* Function Name: R_PG_I2C_GetReceivedDataCount_C0
*
* Description  : 受信済みデータ数の取得
*
* Arguments    : uint16_t * count : 受信データ数の格納先
*
* Return Value : true  : 取得に成功した場合
*              : false : 取得に失敗した場合
*
* Calling Functions : R_IIC_GetStatus
*
* Details      : 詳細についてはリファレンスマニュアルを参照してください。
******************************************************************************/
bool R_PG_I2C_GetReceivedDataCount_C0(uint16_t * count)
{
	if( count == 0 )
	{
		return false;
	}

	return R_IIC_GetStatus(
		0,
		PDL_NO_PTR,
		PDL_NO_PTR,
		count
	);

}

/******************************************************************************
* ID           : 
*
* Include      : 
*
* Declaration  : bool R_PG_I2C_GetSentDataCount_C0(uint16_t * count)
*
* Function Name: R_PG_I2C_GetSentDataCount_C0
*
* Description  : 送信済みデータ数の取得
*
* Arguments    : uint16_t * count : 送信データ数の格納先
*
* Return Value : true  : 取得に成功した場合
*              : false : 取得に失敗した場合
*
* Calling Functions : R_IIC_GetStatus
*
* Details      : 詳細についてはリファレンスマニュアルを参照してください。
******************************************************************************/
bool R_PG_I2C_GetSentDataCount_C0(uint16_t * count)
{
	if( count == 0 )
	{
		return false;
	}

	return R_IIC_GetStatus(
		0,
		PDL_NO_PTR,
		count,
		PDL_NO_PTR
	);

}

/******************************************************************************
* ID           : 
*
* Include      : 
*
* Declaration  : bool R_PG_I2C_Reset_C0(void)
*
* Function Name: R_PG_I2C_Reset_C0
*
* Description  : バスのリセット
*
* Arguments    : なし
*
* Return Value : true  : 設定が正しく行われた場合
*              : false : 設定に失敗した場合
*
* Calling Functions : R_IIC_Control
*
* Details      : 詳細についてはリファレンスマニュアルを参照してください。
******************************************************************************/
bool R_PG_I2C_Reset_C0(void)
{
	return R_IIC_Control(
		0,
		PDL_IIC_RESET
	);

}

/******************************************************************************
* ID           : 
*
* Include      : 
*
* Declaration  : bool R_PG_I2C_StopModule_C0(void)
*
* Function Name: R_PG_I2C_StopModule_C0
*
* Description  : I2Cバスインタフェースチャネルの停止
*
* Arguments    : なし
*
* Return Value : true  : 停止に成功した場合
*              : false : 停止に失敗した場合
*
* Calling Functions : R_IIC_Destroy
*
* Details      : 詳細についてはリファレンスマニュアルを参照してください。
******************************************************************************/
bool R_PG_I2C_StopModule_C0(void)
{
	return R_IIC_Destroy( 0 );

}



