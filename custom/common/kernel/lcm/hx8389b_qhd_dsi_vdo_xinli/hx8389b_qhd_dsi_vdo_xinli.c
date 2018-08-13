/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
	#include <mach/mt_gpio.h>
#endif

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(540)
#define FRAME_HEIGHT 										(960)

#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xFF   // END OF REGISTERS MARKER

#define LCM_DSI_CMD_MODE									0

#define LCM_ID_HX8389B 0x89

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)   			lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)    
       

static struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] = {
	
	/*
	Note :

	Data ID will depends on the following rule.
	
		count of parameters > 1	=> Data ID = 0x39
		count of parameters = 1	=> Data ID = 0x15
		count of parameters = 0	=> Data ID = 0x05

	Structure Format :

	{DCS command, count of parameters, {parameter list}}
	{REGFLAG_DELAY, milliseconds of time, {}},

	...

	Setting ending by predefined flag
	
	{REGFLAG_END_OF_TABLE, 0x00, {}}
	*/


	//must use 0x39 for init setting for all register.

	{REGFLAG_DELAY, 10, {}},

	{REGFLAG_DELAY, 10, {}},

	//{0XCC, 1,   {0X02}},
	//{REGFLAG_DELAY, 10, {}},


	//{0X35, 1,   {0X00}},//TE on
	//{REGFLAG_DELAY, 10, {}},

	// Note
	// Strongly recommend not to set Sleep out / Display On here. That will cause messed frame to be shown as later the backlight is on.

	// Setting ending by predefined flag
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_set_window[] = {
	{0x2A,	4,	{0x00, 0x00, (FRAME_WIDTH>>8), (FRAME_WIDTH&0xFF)}},
	{0x2B,	4,	{0x00, 0x00, (FRAME_HEIGHT>>8), (FRAME_HEIGHT&0xFF)}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
	{0x11, 0, {0x00}},
    {REGFLAG_DELAY, 120, {}},

    // Display ON
	{0x29, 0, {0x00}},
	{REGFLAG_DELAY, 10, {}},
	
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_in_setting[] = {
	// Display off sequence
//	{0x28, 0, {0x00}},
//	{REGFLAG_DELAY, 10, {}},
    // Sleep Mode On
	{0x10, 0, {0x00}},
	{REGFLAG_DELAY, 120, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

    for(i = 0; i < count; i++) {
		
        unsigned cmd;
        cmd = table[i].cmd;
		
        switch (cmd) {
			
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
				
            case REGFLAG_END_OF_TABLE :
                break;
				
            default:
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
       	}
    }
	
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		// enable tearing-free
	#if defined(LCM_DSI_CMD_MODE)
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
	 #else
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_DISABLED;
	#endif
	params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

#if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
#else
		params->dsi.mode   = SYNC_PULSE_VDO_MODE;
#endif
	
		// DSI
		/* Command mode setting */
		params->dsi.LANE_NUM				= LCM_TWO_LANE;
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Highly depends on LCD driver capability.
		// Not support in MT6573
		params->dsi.packet_size=256;

		// Video mode setting		
		params->dsi.intermediat_buffer_num = 0;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

		params->dsi.vertical_sync_active				= 5;
		params->dsi.vertical_backporch					= 11;//5;
		params->dsi.vertical_frontporch					= 5;
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 20;// 10
		params->dsi.horizontal_backporch				= 46;// 60
		params->dsi.horizontal_frontporch				= 21;// 30
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

		// Bit rate calculation
		//params->dsi.pll_div1=34;		// fref=26MHz, fvco=fref*(div1+1)	(div1=0~63, fvco=500MHZ~1GHz)
		//params->dsi.pll_div2=1; 		// div2=0~15: fout=fvo/(2*div2)

		params->dsi.pll_select=0;	//0: MIPI_PLL; 1: LVDS_PLL
		// Bit rate calculation
		// 1 Every lane speed
		params->dsi.pll_div1=0;		// div1=0,1,2,3;div1_real=1,2,4,4 ----0: 546Mbps  1:273Mbps
		params->dsi.pll_div2=1;		// div2=0,1,2,3;div1_real=1,2,4,4	
		params->dsi.fbk_div =17;//30;    // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)	
}
static unsigned int lcm_compare_id(void)
{
	unsigned int id=0;
	unsigned char buffer[2];
	unsigned int array[16];  

    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(1);
    SET_RESET_PIN(1);
    MDELAY(50);//Must over 6 ms

	array[0]=0x00043902;
	array[1]=0x8983FFB9;// page enable
	dsi_set_cmdq(&array, 2, 1);
	MDELAY(10);

	array[0]= 0x00083902;
	array[1]= 0x009341BA;
	array[2]= 0x1800a416;
	dsi_set_cmdq(&array,3, 1);
	MDELAY(10);

	array[0] = 0x00013700;// return byte number
	dsi_set_cmdq(&array, 1, 1);
	MDELAY(10);

	read_reg_v2(0xDB, buffer, 1);
	id = buffer[0]; 
	
	#ifdef BUILD_LK
		printf("tengdeqiang333 uboot %s\n", __func__);
		printf("teng%s, id = 0x%08x\n", __func__, id);//should be 0x00
	#else
		printk("tengdeqiang333 kernel %s\n", __func__);
		printk("teng%s, id= 0x%08x\n", __func__, id);//should be 0x00
	#endif

	return (LCM_ID_HX8389B == id)?1:0;

}


static void lcm_init(void)
{
	unsigned char buffer[2];
	unsigned int data_array[16];  

    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(10);
    SET_RESET_PIN(1);
    MDELAY(50);//Must over 6 ms,SPEC request

//	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);

	data_array[0]= 0x00043902;
	data_array[1]= 0x8983FFB9;
	dsi_set_cmdq(&data_array, 2, 1);
	MDELAY(10);

	//{0XBA, 7, {0x41,0x93,0x00,0x16,0xA4,0x10,0x18}},
	//{REGFLAG_DELAY, 10, {}},
	data_array[0]= 0x00083902;
	data_array[1]= 0x009341BA;
	data_array[2]= 0x1810a416;
	dsi_set_cmdq(&data_array,3, 1);
	MDELAY(10);

	data_array[0]= 0x02CC1500;
	dsi_set_cmdq(&data_array, 1, 1);
//	MDELAY(10);

	//{0XC6, 1, {0X08}},
	//{REGFLAG_DELAY, 10, {}},
    data_array[0]= 0x08C61500;
	dsi_set_cmdq(&data_array, 1, 1);
//	MDELAY(10);

	//{0XB1, 19, {0x00,0x00,0x04,0xEB,0x50,0x10,0x11,0x90,0xF0,0x2F,0x37,0x26,
                        //0x26,0x42,0x01,0x3A,0xFC,0x00,0xE6}},
	//{REGFLAG_DELAY, 10, {}},
	data_array[0]= 0x00143902;
	data_array[1]= 0x070000B1;
	data_array[2]= 0x111058EB;
	data_array[3]= 0x362FF0d4;
	data_array[4]= 0x01422626;
	data_array[5]= 0xE600FC3A;
	dsi_set_cmdq(&data_array, 6, 1);
//	MDELAY(10);
    data_array[0]= 0x00043902;
	data_array[1]= 0x500000B7;
	dsi_set_cmdq(&data_array, 2, 1);
	//MDELAY(10);
	//{0XB2, 7,   {0x00,0x00,0x78,0x0C,0x07,0x3F,0x80}},
	//{REGFLAG_DELAY, 10, {}},
	data_array[0]= 0x00083902;
	data_array[1]= 0x780000B2;
	data_array[2]= 0x403F070c;
	dsi_set_cmdq(&data_array, 3, 1);
//	MDELAY(10);
	
	//{0XB4, 23, {0x80,0x08,0x00,0x32,0x10,0x04,0x32,0x10,0x00,0x32,0x10,0x00,
                //0x37,0x0A,0x40,0x08,0x37,0x0A,0x40,0x14,0x46,0x50,0x0A}}, 
	//{REGFLAG_DELAY, 10, {}},
	data_array[0]= 0x00183902;
	data_array[1]= 0x000892B4;
	data_array[2]= 0x32041032;
	data_array[3]= 0x10320010;
	data_array[4]= 0x400A3700;
	data_array[5]= 0x46023708;
	data_array[6]= 0x0c584c16;
	dsi_set_cmdq(&data_array, 7, 1);
//	MDELAY(10);
	
	//{0XD5, 56, {0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x60,0x00,0x88,0x88,0x88,0x88,0x88,0x23,0x88,
                      //0x01,0x88,0x67,0x88,0x45,0x01,0x23,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,
                      //0x54,0x88,0x76,0x88,0x10,0x88,0x32,0x32,0x10,0x88,0x88,0x88,0x88,0x88,0x00,0x00,0x00,
                      //0x00,0x00,0x00,0x00,0x00}}, 
	//{REGFLAG_DELAY, 10, {}},
	data_array[0]= 0x00393902;
	data_array[1]= 0x000000D5;
	data_array[2]= 0x00000100;
	data_array[3]= 0x88006000;
	data_array[4]= 0x88888888;
	data_array[5]= 0x88018823;
	data_array[6]= 0x01458867;
	data_array[7]= 0x88888823;
	data_array[8]= 0x88888888;
	data_array[9]= 0x54888888;
	data_array[10]= 0x10887688;
	data_array[11]= 0x10323288;
	data_array[12]= 0x88888888;
	data_array[13]= 0x00000088;
	data_array[14]= 0x00000000;
	data_array[15]= 0x00000000;
	dsi_set_cmdq(&data_array, 16, 1);
//	MDELAY(10);

	//{0XDE, 2, {0x05,0x58}},
	//{REGFLAG_DELAY, 10, {}},
	data_array[0]= 0x00033902;
	data_array[1]= 0x005805DE;
	dsi_set_cmdq(&data_array, 2, 1);
//	MDELAY(10);

	//{0XB6, 4, {0x00,0xA4,0x00,0xA4}},
	//{REGFLAG_DELAY, 10, {}},
	data_array[0]= 0x00053902;
	data_array[1]= 0x00A000B6;
   	data_array[2]= 0x000000A0;
	dsi_set_cmdq(&data_array, 3, 1);
//	MDELAY(10);
////////////
/*
	//{0XCB, 2, {0x07,0x07}},
	//{REGFLAG_DELAY, 10, {}},
	data_array[0]= 0x00033902;
	data_array[1]= 0x000707CB;
	dsi_set_cmdq(&data_array, 2, 1);
	MDELAY(10);

	//{0XBB, 4, {0x00,0x00,0xFF,0x80}},
	//{REGFLAG_DELAY, 10, {}},
	data_array[0]= 0x00053902;
	data_array[1]= 0xFF0000BB;
    data_array[2]= 0x00000080;
	dsi_set_cmdq(&data_array, 3, 1);
	MDELAY(10);

	//{0XCC, 1, {0X02}},
	//{REGFLAG_DELAY, 10, {}},
	data_array[0]= 0x02CC1500;
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(10);
*/

	
	data_array[0]= 0x00233902;
	data_array[1]= 0x0e0e00E0;
	data_array[2]= 0x223e1614;
	data_array[3]= 0x130e0237;
	data_array[4]= 0x17171b18;
	data_array[5]= 0x0e001510;
	data_array[6]= 0x3e16140e;
	data_array[7]= 0x0E023722;
	data_array[8]= 0x171b1813;
	data_array[9]= 0x00151017;
	dsi_set_cmdq(&data_array, 10, 1);
	MDELAY(10);


/*
    data_array[0]= 0x00803902;
	data_array[1]= 0x1C0001C1;
	data_array[2]= 0x4B41352A;
	data_array[3]= 0x6D655D55;
	data_array[4]= 0x89827C75;
	data_array[5]= 0xA19C958F;
	data_array[6]= 0xBAB2ACA7;
	data_array[7]= 0xD2CBC8C2;
	data_array[8]= 0xEEE6E2DA;
	data_array[9]= 0x39FFFAF4;

    data_array[10]= 0x67C7E037;
	data_array[11]= 0xC058C1C8;
	data_array[12]= 0x352A1C00;
	data_array[13]= 0x5D554B41;
	data_array[14]= 0x7C756D65;
	data_array[15]= 0x958F8982;
	data_array[16]= 0xACA7A19C;
	data_array[17]= 0xC8C1BAB2;
	data_array[18]= 0xE2DAD2CB;

    data_array[19]= 0xFAF4EEE6;
	data_array[20]= 0xE03739FF;
	data_array[21]= 0xC1C867C7;
	data_array[22]= 0x1C00C058;
	data_array[23]= 0x4B41352A;
	data_array[24]= 0x6D655D55;
	data_array[25]= 0x89827C75;
	data_array[26]= 0xA19C958F;
	data_array[27]= 0xBAB2ACA7;

    data_array[28]= 0xD2CBC8C1;
	data_array[29]= 0xEEE6E2DA;
	data_array[30]= 0x39FFFAF4;
	data_array[31]= 0x67C7E037;
	data_array[32]= 0xC058C1C8;
	dsi_set_cmdq(&data_array, 33, 1);
	MDELAY(10);
*/
    data_array[0] = 0x00110500;	//exit sleep mode
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(150);

    // Display ON
	//{0x29, 0, {0x00}},
	//{REGFLAG_DELAY, 10, {}},
	data_array[0] = 0x00290500;	//exit sleep mode
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(50);
}


static void lcm_suspend(void)
{


//	push_table(lcm_sleep_in_setting, sizeof(lcm_sleep_in_setting) / sizeof(struct LCM_setting_table), 1);
	
    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(10);
    SET_RESET_PIN(1);
    MDELAY(50);//Must over 6 ms,SPEC request	
	
//	lcm_compare_id();

}


static void lcm_resume(void)
{

	lcm_init();
	//push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);


}


static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	data_array[3]= 0x00053902;
	data_array[4]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[5]= (y1_LSB);
	data_array[6]= 0x002c3909;

	dsi_set_cmdq(&data_array, 7, 0);

}


LCM_DRIVER hx8389b_qhd_dsi_vdo_xinli_drv = 
{
    .name			= "hx8389b_dsi_vdo_xinli",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id,
#if (LCM_DSI_CMD_MODE)
	.set_backlight	= lcm_setbacklight,
    .update         = lcm_update,
#endif
};

