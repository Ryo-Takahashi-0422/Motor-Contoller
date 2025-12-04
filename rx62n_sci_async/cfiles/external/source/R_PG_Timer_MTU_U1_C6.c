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
* File Name    : R_PG_Timer_MTU_U1_C6.c
* Version      : 1.00
* Device(s)    : 
* Tool-Chain   : 
* H/W Platform : 
* Description  : 
* Limitations  : 
******************************************************************************
* History : 15.11.2025 Version Description
*         :   
******************************************************************************/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "R_PG_Timer_MTU_U1_C6.h"


/******************************************************************************
* ID           : 
*
* Include      : 
*
* Declaration  : bool R_PG_Timer_Set_MTU_U1_C6(void)
*
* Function Name: R_PG_Timer_Set_MTU_U1_C6
*
* Description  : MTUの設定
*
* Arguments    : なし
*
* Return Value : true  : 設定が正しく行われた場合
*              : false : 設定に失敗した場合
*
* Calling Functions : R_MTU2_Create
*                   : R_MTU2_Create_load_defaults
*                   : R_MTU2_Set
*
* Details      : 詳細についてはリファレンスマニュアルを参照してください。
******************************************************************************/
bool R_PG_Timer_Set_MTU_U1_C6(void)
{
	bool res;
	R_MTU2_Create_structure parameters;

	R_MTU2_Create_load_defaults( &parameters );

	parameters.data2 = PDL_MTU2_MODE_NORMAL | PDL_MTU2_SYNC_DISABLE;
	parameters.data3 = PDL_MTU2_CLK_PCLK_DIV_4 | PDL_MTU2_CLK_RISING | PDL_MTU2_CLEAR_TGRC;
	parameters.data4 = PDL_MTU2_ADC_TRIG_TGRA_DISABLE;
	parameters.data5 = PDL_MTU2_BUFFER_AC_DISABLE | PDL_MTU2_BUFFER_BD_ENABLE | PDL_MTU2_BUFFER_EF_DISABLE;
	parameters.data6 = PDL_MTU2_A_OC_DISABLED | PDL_MTU2_B_IC_COUNT;
	parameters.data7 = PDL_MTU2_C_OC_LOW_CM_HIGH;
	parameters.data9 = 0;
	parameters.data10 = 0;
	parameters.data11 = 0;
	parameters.data12 = 5999;
	parameters.data13 = 0;
	parameters.data14 = 0;
	parameters.data15 = 0;
	parameters.data16 = 0;
	parameters.data17 = 0;
	parameters.data18 = 0;
	parameters.func1 = PDL_NO_FUNC;
	parameters.func2 = Mtu6IcCmBIntFunc;
	parameters.func3 = PDL_NO_FUNC;
	parameters.func4 = PDL_NO_FUNC;
	parameters.data19 = 15;
	parameters.func5 = PDL_NO_FUNC;
	parameters.func6 = PDL_NO_FUNC;
	parameters.func7 = PDL_NO_FUNC;
	parameters.func8 = PDL_NO_FUNC;
	parameters.data20 = 0;

	res = R_MTU2_Set( PDL_MTU2_PIN_CLKEFGH_A );

	if( !res ){
		return res;
	}

	return R_MTU2_Create(
		6,
		&parameters
	);

}

/******************************************************************************
* ID           : 
*
* Include      : 
*
* Declaration  : bool R_PG_Timer_StartCount_MTU_U1_C6(void)
*
* Function Name: R_PG_Timer_StartCount_MTU_U1_C6
*
* Description  : MTUのカウント動作開始
*
* Arguments    : なし
*
* Return Value : true  : カウント動作の再開が正しく行われた場合
*              : false : カウント動作の再開に失敗した場合
*
* Calling Functions : R_MTU2_ControlChannel
*
* Details      : 詳細についてはリファレンスマニュアルを参照してください。
******************************************************************************/
bool R_PG_Timer_StartCount_MTU_U1_C6(void)
{
	R_MTU2_ControlChannel_structure parameters;

	parameters.data2 = PDL_MTU2_START;
	parameters.data3 = PDL_NO_DATA;
	parameters.data4 = PDL_NO_DATA;
	parameters.data5 = PDL_NO_DATA;
	parameters.data6 = PDL_NO_DATA;
	parameters.data7 = PDL_NO_DATA;
	parameters.data8 = PDL_NO_DATA;
	parameters.data9 = PDL_NO_DATA;
	parameters.data10 = PDL_NO_DATA;
	parameters.data11 = PDL_NO_DATA;
	parameters.data12 = PDL_NO_DATA;

	return R_MTU2_ControlChannel(
		6,
		&parameters
	);

}

/******************************************************************************
* ID           : 
*
* Include      : 
*
* Declaration  : bool R_PG_Timer_HaltCount_MTU_U1_C6(void)
*
* Function Name: R_PG_Timer_HaltCount_MTU_U1_C6
*
* Description  : MTUのカウント動作を一時停止
*
* Arguments    : なし
*
* Return Value : true  : 停止に成功した場合
*              : false : 停止に失敗した場合
*
* Calling Functions : R_MTU2_ControlChannel
*
* Details      : 詳細についてはリファレンスマニュアルを参照してください。
******************************************************************************/
bool R_PG_Timer_HaltCount_MTU_U1_C6(void)
{
	R_MTU2_ControlChannel_structure parameters;

	parameters.data2 = PDL_MTU2_STOP;
	parameters.data3 = PDL_NO_DATA;
	parameters.data4 = PDL_NO_DATA;
	parameters.data5 = PDL_NO_DATA;
	parameters.data6 = PDL_NO_DATA;
	parameters.data7 = PDL_NO_DATA;
	parameters.data8 = PDL_NO_DATA;
	parameters.data9 = PDL_NO_DATA;
	parameters.data10 = PDL_NO_DATA;
	parameters.data11 = PDL_NO_DATA;
	parameters.data12 = PDL_NO_DATA;

	return R_MTU2_ControlChannel(
		6,
		&parameters
	);

}

/******************************************************************************
* ID           : 
*
* Include      : 
*
* Declaration  : bool R_PG_Timer_GetCounterValue_MTU_U1_C6(uint16_t * counter_val)
*
* Function Name: R_PG_Timer_GetCounterValue_MTU_U1_C6
*
* Description  : MTUのカウンタ値を取得
*
* Arguments    : uint16_t * counter_val : カウンタ値の格納先
*
* Return Value : true  : 取得に成功した場合
*              : false : 取得に失敗した場合
*
* Calling Functions : R_MTU2_ReadChannel
*
* Details      : 詳細についてはリファレンスマニュアルを参照してください。
******************************************************************************/
bool R_PG_Timer_GetCounterValue_MTU_U1_C6(uint16_t * counter_val)
{
	if( counter_val == 0 ){ return false; }

	return R_MTU2_ReadChannel(
		6,
		PDL_NO_PTR,
		counter_val,
		PDL_NO_PTR,
		PDL_NO_PTR,
		PDL_NO_PTR,
		PDL_NO_PTR,
		PDL_NO_PTR,
		PDL_NO_PTR
	);

}

/******************************************************************************
* ID           : 
*
* Include      : 
*
* Declaration  : bool R_PG_Timer_SetCounterValue_MTU_U1_C6(uint16_t counter_val)
*
* Function Name: R_PG_Timer_SetCounterValue_MTU_U1_C6
*
* Description  : MTUのカウンタ値を設定
*
* Arguments    : uint16_t counter_val : カウンタに設定する値
*
* Return Value : true  : カウンタ値の設定に成功した場合
*              : false : カウンタ値の設定に失敗した場合
*
* Calling Functions : R_MTU2_ControlChannel
*
* Details      : 詳細についてはリファレンスマニュアルを参照してください。
******************************************************************************/
bool R_PG_Timer_SetCounterValue_MTU_U1_C6(uint16_t counter_val)
{
	R_MTU2_ControlChannel_structure parameters;

	parameters.data2 = PDL_NO_DATA;
	parameters.data3 = PDL_MTU2_REGISTER_COUNTER;
	parameters.data4 = counter_val;
	parameters.data5 = PDL_NO_DATA;
	parameters.data6 = PDL_NO_DATA;
	parameters.data7 = PDL_NO_DATA;
	parameters.data8 = PDL_NO_DATA;
	parameters.data9 = PDL_NO_DATA;
	parameters.data10 = PDL_NO_DATA;
	parameters.data11 = PDL_NO_DATA;
	parameters.data12 = PDL_NO_DATA;

	return R_MTU2_ControlChannel(
		6,
		&parameters
	);

}

/******************************************************************************
* ID           : 
*
* Include      : 
*
* Declaration  : bool R_PG_Timer_GetTGR_MTU_U1_C6(uint16_t * tgr_a_val, uint16_t * tgr_b_val, uint16_t * tgr_c_val, uint16_t * tgr_d_val, uint16_t * tgr_e_val, uint16_t * tgr_f_val)
*
* Function Name: R_PG_Timer_GetTGR_MTU_U1_C6
*
* Description  : ジェネラルレジスタの値の取得
*
* Arguments    : uint16_t * tgr_a_val : ジェネラルレジスタA値の格納先
*              : uint16_t * tgr_b_val : ジェネラルレジスタB値の格納先
*              : uint16_t * tgr_c_val : ジェネラルレジスタC値の格納先
*              : uint16_t * tgr_d_val : ジェネラルレジスタD値の格納先
*              : uint16_t * tgr_e_val : ジェネラルレジスタE値の格納先
*              : uint16_t * tgr_f_val : ジェネラルレジスタF値の格納先
*
* Return Value : true  : 取得に成功した場合
*              : false : 取得に失敗した場合
*
* Calling Functions : R_MTU2_ReadChannel
*
* Details      : 詳細についてはリファレンスマニュアルを参照してください。
******************************************************************************/
bool R_PG_Timer_GetTGR_MTU_U1_C6(uint16_t * tgr_a_val, uint16_t * tgr_b_val, uint16_t * tgr_c_val, uint16_t * tgr_d_val, uint16_t * tgr_e_val, uint16_t * tgr_f_val)
{
	uint16_t * local_a = PDL_NO_PTR;
	uint16_t * local_b = PDL_NO_PTR;
	uint16_t * local_c = PDL_NO_PTR;
	uint16_t * local_d = PDL_NO_PTR;
	uint16_t * local_e = PDL_NO_PTR;
	uint16_t * local_f = PDL_NO_PTR;

	if( tgr_a_val ){
		local_a = tgr_a_val;
	}
	if( tgr_b_val ){
		local_b = tgr_b_val;
	}
	if( tgr_c_val ){
		local_c = tgr_c_val;
	}
	if( tgr_d_val ){
		local_d = tgr_d_val;
	}
	if( tgr_e_val ){
		local_e = tgr_e_val;
	}
	if( tgr_f_val ){
		local_f = tgr_f_val;
	}

	return R_MTU2_ReadChannel(
		6,
		PDL_NO_PTR,
		PDL_NO_PTR,
		local_a,
		local_b,
		local_c,
		local_d,
		local_e,
		local_f
	);

}

/******************************************************************************
* ID           : 
*
* Include      : 
*
* Declaration  : bool R_PG_Timer_SetTGR_A_MTU_U1_C6(uint16_t tgr_a_val)
*
* Function Name: R_PG_Timer_SetTGR_A_MTU_U1_C6
*
* Description  : ジェネラルレジスタの値の設定
*
* Arguments    : uint16_t tgr_a_val : ジェネラルレジスタに設定する値
*
* Return Value : true  : 設定が正しく行われた場合
*              : false : 設定に失敗した場合
*
* Calling Functions : R_MTU2_ControlChannel
*
* Details      : 詳細についてはリファレンスマニュアルを参照してください。
******************************************************************************/
bool R_PG_Timer_SetTGR_A_MTU_U1_C6(uint16_t tgr_a_val)
{
	R_MTU2_ControlChannel_structure parameters;

	parameters.data2 = PDL_NO_DATA;
	parameters.data3 = PDL_MTU2_REGISTER_TGRA;
	parameters.data4 = PDL_NO_DATA;
	parameters.data5 = tgr_a_val;
	parameters.data6 = PDL_NO_DATA;
	parameters.data7 = PDL_NO_DATA;
	parameters.data8 = PDL_NO_DATA;
	parameters.data9 = PDL_NO_DATA;
	parameters.data10 = PDL_NO_DATA;
	parameters.data11 = PDL_NO_DATA;
	parameters.data12 = PDL_NO_DATA;

	return R_MTU2_ControlChannel(
		6,
		&parameters
	);

}

/******************************************************************************
* ID           : 
*
* Include      : 
*
* Declaration  : bool R_PG_Timer_SetTGR_B_MTU_U1_C6(uint16_t tgr_b_val)
*
* Function Name: R_PG_Timer_SetTGR_B_MTU_U1_C6
*
* Description  : ジェネラルレジスタの値の設定
*
* Arguments    : uint16_t tgr_b_val : ジェネラルレジスタに設定する値
*
* Return Value : true  : 設定が正しく行われた場合
*              : false : 設定に失敗した場合
*
* Calling Functions : R_MTU2_ControlChannel
*
* Details      : 詳細についてはリファレンスマニュアルを参照してください。
******************************************************************************/
bool R_PG_Timer_SetTGR_B_MTU_U1_C6(uint16_t tgr_b_val)
{
	R_MTU2_ControlChannel_structure parameters;

	parameters.data2 = PDL_NO_DATA;
	parameters.data3 = PDL_MTU2_REGISTER_TGRB;
	parameters.data4 = PDL_NO_DATA;
	parameters.data5 = PDL_NO_DATA;
	parameters.data6 = tgr_b_val;
	parameters.data7 = PDL_NO_DATA;
	parameters.data8 = PDL_NO_DATA;
	parameters.data9 = PDL_NO_DATA;
	parameters.data10 = PDL_NO_DATA;
	parameters.data11 = PDL_NO_DATA;
	parameters.data12 = PDL_NO_DATA;

	return R_MTU2_ControlChannel(
		6,
		&parameters
	);

}

/******************************************************************************
* ID           : 
*
* Include      : 
*
* Declaration  : bool R_PG_Timer_SetTGR_C_MTU_U1_C6(uint16_t tgr_c_val)
*
* Function Name: R_PG_Timer_SetTGR_C_MTU_U1_C6
*
* Description  : ジェネラルレジスタの値の設定
*
* Arguments    : uint16_t tgr_c_val : ジェネラルレジスタに設定する値
*
* Return Value : true  : 設定が正しく行われた場合
*              : false : 設定に失敗した場合
*
* Calling Functions : R_MTU2_ControlChannel
*
* Details      : 詳細についてはリファレンスマニュアルを参照してください。
******************************************************************************/
bool R_PG_Timer_SetTGR_C_MTU_U1_C6(uint16_t tgr_c_val)
{
	R_MTU2_ControlChannel_structure parameters;

	parameters.data2 = PDL_NO_DATA;
	parameters.data3 = PDL_MTU2_REGISTER_TGRC;
	parameters.data4 = PDL_NO_DATA;
	parameters.data5 = PDL_NO_DATA;
	parameters.data6 = PDL_NO_DATA;
	parameters.data7 = tgr_c_val;
	parameters.data8 = PDL_NO_DATA;
	parameters.data9 = PDL_NO_DATA;
	parameters.data10 = PDL_NO_DATA;
	parameters.data11 = PDL_NO_DATA;
	parameters.data12 = PDL_NO_DATA;

	return R_MTU2_ControlChannel(
		6,
		&parameters
	);

}

/******************************************************************************
* ID           : 
*
* Include      : 
*
* Declaration  : bool R_PG_Timer_SetTGR_D_MTU_U1_C6(uint16_t tgr_d_val)
*
* Function Name: R_PG_Timer_SetTGR_D_MTU_U1_C6
*
* Description  : ジェネラルレジスタの値の設定
*
* Arguments    : uint16_t tgr_d_val : ジェネラルレジスタに設定する値
*
* Return Value : true  : 設定が正しく行われた場合
*              : false : 設定に失敗した場合
*
* Calling Functions : R_MTU2_ControlChannel
*
* Details      : 詳細についてはリファレンスマニュアルを参照してください。
******************************************************************************/
bool R_PG_Timer_SetTGR_D_MTU_U1_C6(uint16_t tgr_d_val)
{
	R_MTU2_ControlChannel_structure parameters;

	parameters.data2 = PDL_NO_DATA;
	parameters.data3 = PDL_MTU2_REGISTER_TGRD;
	parameters.data4 = PDL_NO_DATA;
	parameters.data5 = PDL_NO_DATA;
	parameters.data6 = PDL_NO_DATA;
	parameters.data7 = PDL_NO_DATA;
	parameters.data8 = tgr_d_val;
	parameters.data9 = PDL_NO_DATA;
	parameters.data10 = PDL_NO_DATA;
	parameters.data11 = PDL_NO_DATA;
	parameters.data12 = PDL_NO_DATA;

	return R_MTU2_ControlChannel(
		6,
		&parameters
	);

}

/******************************************************************************
* ID           : 
*
* Include      : 
*
* Declaration  : bool R_PG_Timer_SetTGR_E_MTU_U1_C6(uint16_t tgr_e_val)
*
* Function Name: R_PG_Timer_SetTGR_E_MTU_U1_C6
*
* Description  : ジェネラルレジスタの値の設定
*
* Arguments    : uint16_t tgr_e_val : ジェネラルレジスタに設定する値
*
* Return Value : true  : 設定が正しく行われた場合
*              : false : 設定に失敗した場合
*
* Calling Functions : R_MTU2_ControlChannel
*
* Details      : 詳細についてはリファレンスマニュアルを参照してください。
******************************************************************************/
bool R_PG_Timer_SetTGR_E_MTU_U1_C6(uint16_t tgr_e_val)
{
	R_MTU2_ControlChannel_structure parameters;

	parameters.data2 = PDL_NO_DATA;
	parameters.data3 = PDL_MTU2_REGISTER_TGRE;
	parameters.data4 = PDL_NO_DATA;
	parameters.data5 = PDL_NO_DATA;
	parameters.data6 = PDL_NO_DATA;
	parameters.data7 = PDL_NO_DATA;
	parameters.data8 = PDL_NO_DATA;
	parameters.data9 = tgr_e_val;
	parameters.data10 = PDL_NO_DATA;
	parameters.data11 = PDL_NO_DATA;
	parameters.data12 = PDL_NO_DATA;

	return R_MTU2_ControlChannel(
		6,
		&parameters
	);

}

/******************************************************************************
* ID           : 
*
* Include      : 
*
* Declaration  : bool R_PG_Timer_SetTGR_F_MTU_U1_C6(uint16_t tgr_f_val)
*
* Function Name: R_PG_Timer_SetTGR_F_MTU_U1_C6
*
* Description  : ジェネラルレジスタの値の設定
*
* Arguments    : uint16_t tgr_f_val : ジェネラルレジスタに設定する値
*
* Return Value : true  : 設定が正しく行われた場合
*              : false : 設定に失敗した場合
*
* Calling Functions : R_MTU2_ControlChannel
*
* Details      : 詳細についてはリファレンスマニュアルを参照してください。
******************************************************************************/
bool R_PG_Timer_SetTGR_F_MTU_U1_C6(uint16_t tgr_f_val)
{
	R_MTU2_ControlChannel_structure parameters;

	parameters.data2 = PDL_NO_DATA;
	parameters.data3 = PDL_MTU2_REGISTER_TGRF;
	parameters.data4 = PDL_NO_DATA;
	parameters.data5 = PDL_NO_DATA;
	parameters.data6 = PDL_NO_DATA;
	parameters.data7 = PDL_NO_DATA;
	parameters.data8 = PDL_NO_DATA;
	parameters.data9 = PDL_NO_DATA;
	parameters.data10 = tgr_f_val;
	parameters.data11 = PDL_NO_DATA;
	parameters.data12 = PDL_NO_DATA;

	return R_MTU2_ControlChannel(
		6,
		&parameters
	);

}

/******************************************************************************
* ID           : 
*
* Include      : 
*
* Declaration  : bool R_PG_Timer_GetRequestFlag_MTU_U1_C6(bool * cm_ic_a, bool * cm_ic_b, bool * cm_ic_c, bool * cm_ic_d, bool * cm_e, bool * cm_f, bool * ov, bool * un)
*
* Function Name: R_PG_Timer_GetRequestFlag_MTU_U1_C6
*
* Description  : MTUの割り込み要求フラグの取得とクリア
*
* Arguments    : bool * cm_ic_a : コンペアマッチ/インプットキャプチャAフラグの格納先
*              : bool * cm_ic_b : コンペアマッチ/インプットキャプチャBフラグの格納先
*              : bool * cm_ic_c : コンペアマッチ/インプットキャプチャCフラグの格納先
*              : bool * cm_ic_d : コンペアマッチ/インプットキャプチャDフラグの格納先
*              : bool * cm_e : コンペアマッチEフラグの格納先
*              : bool * cm_f : コンペアマッチFフラグの格納先
*              : bool * ov : オーバフローフラグの格納先
*              : bool * un : アンダフローフラグの格納先
*
* Return Value : true  : 取得に成功した場合
*              : false : 取得に失敗した場合
*
* Calling Functions : R_MTU2_ReadChannel
*
* Details      : 詳細についてはリファレンスマニュアルを参照してください。
******************************************************************************/
bool R_PG_Timer_GetRequestFlag_MTU_U1_C6(bool * cm_ic_a, bool * cm_ic_b, bool * cm_ic_c, bool * cm_ic_d, bool * cm_e, bool * cm_f, bool * ov, bool * un)
{
	uint8_t data;
	bool res;

	res = R_MTU2_ReadChannel(
		6,
		&data,
		PDL_NO_PTR,
		PDL_NO_PTR,
		PDL_NO_PTR,
		PDL_NO_PTR,
		PDL_NO_PTR,
		PDL_NO_PTR,
		PDL_NO_PTR
	);

	if( cm_ic_a ){
		*cm_ic_a = (data >> 1) & 0x01;
	}
	if( cm_ic_b ){
		*cm_ic_b = (data >> 2) & 0x01;
	}
	if( cm_ic_c ){
		*cm_ic_c = (data >> 3) & 0x01;
	}
	if( cm_ic_d ){
		*cm_ic_d = (data >> 4) & 0x01;
	}
	if( cm_e ){
		*cm_e = (data >> 5) & 0x01;
	}
	if( cm_f ){
		*cm_f = (data >> 6) & 0x01;
	}
	if( ov ){
		*ov = (data >> 7) & 0x01;
	}
	if( un ){
		*un = 0x00;
	}

	return res;
}



