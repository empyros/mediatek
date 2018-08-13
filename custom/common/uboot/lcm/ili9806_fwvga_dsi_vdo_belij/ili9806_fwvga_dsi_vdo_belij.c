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

#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (854)

#define LCM_ID_ILI9806 (0x9806)

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0XFD   // END OF REGISTERS MARKER
#define LCM_DSI_CMD_MODE									 0

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update); MDELAY(10) 
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#define dsi_set_cmdq_HQ(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_HQ(cmd, count, ppara, force_update)
extern int IMM_GetOneChannelValue(int dwChannel, int deCount);


struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};
#if 1
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

    {0xFF, 3 ,{0xFF,0x98,0x06}},
    {0xBA, 1 ,{0x60}}, 
    {0xBC, 21,{0x03,0x0E,0x03,0x63,0x01,0x01,0x1B,0x12,0x6F,0x00,0x00,0x00,0x01,0x01,0x03,0x00,0xFF,0xF2,0x01,0x00,0xC0}},
    {0xBD, 8 ,{0x02,0x13,0x45,0x67,0x45,0x67,0x01,0x23}},
    {0xBE, 9 ,{0x01,0x22,0x22,0xDC,0xBA,0x67,0x22,0x22,0x22}},
    {0xC7, 1 ,{0x59}},
    {0xED, 3 ,{0x7F,0x0F,0x00}},
    {0xC0, 3 ,{0x03,0x0B,0x00}},
    {0xFC, 1 ,{0x08}},
    {0xDF, 6 ,{0x00,0x00,0x00,0x00,0x00,0x20}},
    {0xF3, 1 ,{0x74}},
    {0xB4, 3 ,{0x00,0x00,0x00}},
    {0xF7, 1 ,{0x81}},
    {0xB1, 3 ,{0x00,0x13,0x13}},
    {0xF2, 4 ,{0x80,0x02,0x40,0x28}},
    {0xC1, 4 ,{0x17,0x75,0x90,0x20}},
    {0xE0, 16,{0x00,0x06,0x11,0x10,0x12,0x1B,0xC9,0x07,0x03,0x09,0x05,0x0D,0x0D,0x30,0x2A,0x00}},
    {0xE1, 16,{0x00,0x07,0x14,0x10,0x12,0x19,0x79,0x07,0x05,0x08,0x06,0x0B,0x0A,0x27,0x22,0x00}},
    {0x35, 1 ,{0x00}},
    {0x3A, 1 ,{0x55}},    
    {0x36, 1 ,{0x00}},
    
    {0x11, 1 ,{0x00}}, 
    {REGFLAG_DELAY, 120, {}},
    
    {0x29, 1 ,{0x00}}, 
    
    {REGFLAG_DELAY,50, {}},
    
    //{0x2C, 1 ,{0x00}}, 
    	// Note
    	// Strongly recommend not to set Sleep out / Display On here. That will cause messed frame to be shown as later the backlight is on.
    	// Setting ending by predefined flag
    
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};
#else


static struct LCM_setting_table lcm_initialization_setting[] = {
//ic 
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

	Setting ending by predefined flag*/
//LCD_ILI9806_CMD(0xFF);             
//LCD_ILI9806_INDEX(0xFF);							
//LCD_ILI9806_INDEX(0x98);							
//LCD_ILI9806_INDEX(0x06);   							

{0xff,3,{0xff,0x98,0x06}},
							
//LCD_ILI9806_CMD(0xBA);              // SPI Interface Setting							
//LCD_ILI9806_INDEX(0x60); 							
{0xba,1,{0x60}},	
{0x3a,1,{0x77}},						
///////GIP Timing Setting for HSD 4);3inch module////////							
//LCD_ILI9806_CMD(0xBC);                  // GIP 1							
//LCD_ILI9806_INDEX(0x03);							
//LCD_ILI9806_INDEX(0x0E);							
//LCD_ILI9806_INDEX(0x03);							
//LCD_ILI9806_INDEX(0x63);							
//LCD_ILI9806_INDEX(0x01);							
//LCD_ILI9806_INDEX(0x01);							
//LCD_ILI9806_INDEX(0x1B);							
//LCD_ILI9806_INDEX(0x12);							
//LCD_ILI9806_INDEX(0x6F);							
//LCD_ILI9806_INDEX(0x00);							
//LCD_ILI9806_INDEX(0x00);							
//LCD_ILI9806_INDEX(0x00);							
//LCD_ILI9806_INDEX(0x01);							
//LCD_ILI9806_INDEX(0x01);							
//LCD_ILI9806_INDEX(0x04);							
//LCD_ILI9806_INDEX(0x03);							
//LCD_ILI9806_INDEX(0xFF);							
//LCD_ILI9806_INDEX(0xF2);							
//LCD_ILI9806_INDEX(0x01);							
//LCD_ILI9806_INDEX(0x00);							
//LCD_ILI9806_INDEX(0xC0);							
{0xbc,21,{0x03,0x0e,0x03,0x63,0x01,0x01,0x1b,0x12,0x6f,0x00,0x00,0x00,0x01,0x01,0x04,0x03,0xff,0xf2,0x01,0x00,0xc0}},
							
//LCD_ILI9806_CMD(0xBD);              // GIP 2							
//LCD_ILI9806_INDEX(0x02);							
//LCD_ILI9806_INDEX(0x13);							
//LCD_ILI9806_INDEX(0x45);							
//LCD_ILI9806_INDEX(0x67);							
//LCD_ILI9806_INDEX(0x45);							
//LCD_ILI9806_INDEX(0x67);							
//LCD_ILI9806_INDEX(0x01);							
//LCD_ILI9806_INDEX(0x23);							
{0xbd,8,{0x02,0x13,0x45,0x67,0x45,0x67,0x01,0x23}},
							
//LCD_ILI9806_CMD(0xBE);              // GIP 3							
//LCD_ILI9806_INDEX(0x01);							
//LCD_ILI9806_INDEX(0x22);							
//LCD_ILI9806_INDEX(0x22);							
//LCD_ILI9806_INDEX(0xDC);							
//LCD_ILI9806_INDEX(0xBA);							
//LCD_ILI9806_INDEX(0x67);							
//LCD_ILI9806_INDEX(0x22);							
//LCD_ILI9806_INDEX(0x22);							
//LCD_ILI9806_INDEX(0x22);							
{0xbe,9,{0x01,0x22,0x22,0xdc,0xba,0x67,0x22,0x22,0x22}},
							
//LCD_ILI9806_CMD(0xC7);             // VCOM Control							
//LCD_ILI9806_INDEX(0x5D);							
//{0xc7,1,{0x5d}},//gama 2.2
{0xc7,1,{0x91}},
							
//LCD_ILI9806_CMD(0xED);							
//LCD_ILI9806_INDEX(0x7F);							
//LCD_ILI9806_INDEX(0x0F);							
//LCD_ILI9806_INDEX(0x00);							
{0xed,3,{0x7f,0x0f,0x00}},
							
//LCD_ILI9806_CMD(0xC0);            // Power Control 1							
//LCD_ILI9806_INDEX(0x03);							
//LCD_ILI9806_INDEX(0x0B);							
//LCD_ILI9806_INDEX(0x00);  
 							
{0xc0,3,{0x03,0x0b,0x00}},	
						
//LCD_ILI9806_CMD(0xF5);            // LVGL							
//LCD_ILI9806_INDEX(0x20);							
//LCD_ILI9806_INDEX(0x43);							
//LCD_ILI9806_INDEX(0x00);							

{0xf5,3,{0x20,0x43,0x00}},
							
//LCD_ILI9806_CMD(0xEE);            // LVGL							
//LCD_ILI9806_INDEX(0x0A);							
//LCD_ILI9806_INDEX(0x1B);							
//LCD_ILI9806_INDEX(0x5F);							
//LCD_ILI9806_INDEX(0x40);							
//LCD_ILI9806_INDEX(0x28);							
//LCD_ILI9806_INDEX(0x38);							
//LCD_ILI9806_INDEX(0x02);							
//LCD_ILI9806_INDEX(0x2B);							
//LCD_ILI9806_INDEX(0x50);							
//LCD_ILI9806_INDEX(0x00);							
//LCD_ILI9806_INDEX(0x80);							

{0xee,11,{0x0a,0x1b,0x5f,0x40,0x28,0x38,0x02,0x2b,0x50,0x00,0x80}},
							
//LCD_ILI9806_CMD(0xDF);							
//LCD_ILI9806_INDEX(0x00);							
//LCD_ILI9806_INDEX(0x00);							
//LCD_ILI9806_INDEX(0x00);							
//LCD_ILI9806_INDEX(0x00);							
//LCD_ILI9806_INDEX(0x00);							
//LCD_ILI9806_INDEX(0x20);							

{0xdf,6,{0x00,0x00,0x00,0x00,0x00,0x20}},
							
//LCD_ILI9806_CMD(0xF3);							
//LCD_ILI9806_INDEX(0x74);							

{0xf3,1,{0x74}},
							
//LCD_ILI9806_CMD(0xFC);           // LVGL							
//LCD_ILI9806_INDEX(0x08);							

{0xfc,1,{0x08}},							
//Inversion							
//LCD_ILI9806_CMD(0xB4);          // Display Inversion Control							
//LCD_ILI9806_INDEX(0x02);							
//LCD_ILI9806_INDEX(0x02);							
//LCD_ILI9806_INDEX(0x02);							
	
{0xb4,3,{0x00,0x00,0x00}},
						
//LCD_ILI9806_CMD(0xF7);          // 480x854							
//LCD_ILI9806_INDEX(0x82);
							
{0xf7,1,{0x82}},			       				
/////////HSD module display quality tune code//////////////////							
//LCD_ILI9806_CMD(0xB1);         // Frame Rate Control							
//LCD_ILI9806_INDEX(0x00);							
//LCD_ILI9806_INDEX(0x13);							
//LCD_ILI9806_INDEX(0x13);							

{0xb1,3,{0x00,0x11,0x13}},
							
//LCD_ILI9806_CMD(0xF2);         //Panel Timing Control							
//LCD_ILI9806_INDEX(0x41);							
//LCD_ILI9806_INDEX(0x04);							
//LCD_ILI9806_INDEX(0x41);							
//LCD_ILI9806_INDEX(0x28);							
							
{0xf2,4,{0x41,0x04,0x41,0x28}},							
							
//LCD_ILI9806_CMD(0xC1);        // Power Control 2							
//LCD_ILI9806_INDEX(0x17);							
//LCD_ILI9806_INDEX(0x78);							
//LCD_ILI9806_INDEX(0x7B);							
//LCD_ILI9806_INDEX(0x20);
							
{0xc1,4,{0x17,0x78,0x7b,0x20}},	
							
//LCD_ILI9806_CMD(0xE0);							
//LCD_ILI9806_INDEX(0x00);//P1							
//LCD_ILI9806_INDEX(0x08);//P2							
//LCD_ILI9806_INDEX(0x13);//P3							
//LCD_ILI9806_INDEX(0x10);//P4							
//LCD_ILI9806_INDEX(0x10);//P5							
//LCD_ILI9806_INDEX(0x17);//P6							
//LCD_ILI9806_INDEX(0xCA);//P7							
//LCD_ILI9806_INDEX(0x08);//P8							
//LCD_ILI9806_INDEX(0x03);//P9							
//LCD_ILI9806_INDEX(0x08);//P10							
//LCD_ILI9806_INDEX(0x06);//P11							
//LCD_ILI9806_INDEX(0x0D);//P12							
//LCD_ILI9806_INDEX(0x0F);//P13							
//LCD_ILI9806_INDEX(0x2E);//P14							
//LCD_ILI9806_INDEX(0x29);//P15							
//LCD_ILI9806_INDEX(0x00);//P16							
	
//{0xe0,16,{0x00,0x08,0x13,0x10,0x10,0x17,0xca,0x08,0x03,0x08,0x06,0x0d,0x0f,0x2e,0x29,0x00}},	//gama2.2
{0xe0,16,{0x00,0x24,0x2a,0x0a,0x0f,0x17,0xc9,0x08,0x03,0x07,0x01,0x00,0x00,0x1e,0x14,0x00}},	
						
//LCD_ILI9806_CMD(0xE1);							
//LCD_ILI9806_INDEX(0x00);//P1							
//LCD_ILI9806_INDEX(0x04);//P2							
//LCD_ILI9806_INDEX(0x0C);//P3							
//LCD_ILI9806_INDEX(0x0F);//P4							
//LCD_ILI9806_INDEX(0x11);//P5							
//LCD_ILI9806_INDEX(0x15);//P6							
//LCD_ILI9806_INDEX(0x79);//P7							
//LCD_ILI9806_INDEX(0x07);//P8							
//LCD_ILI9806_INDEX(0x05);//P9							
//LCD_ILI9806_INDEX(0x09);//P10							
//LCD_ILI9806_INDEX(0x07);//P11							
//LCD_ILI9806_INDEX(0x0F);//P12							
//LCD_ILI9806_INDEX(0x0D);//P13							
//LCD_ILI9806_INDEX(0x2C);//P14							
//LCD_ILI9806_INDEX(0x27);//P15							
//LCD_ILI9806_INDEX(0x00);//P16							

//{0xe1,16,{0x00,0x04,0x0c,0x0f,0x11,0x15,0x79,0x07,0x05,0x09,0x07,0x0f,0x0d,0x2c,0x27,0x00}}, //gama2.2

  {0xe1,16,{0x00,0x01,0x01,0x06,0x06,0x07,0x7a,0x09,0x03,0x08,0x0f,0x11,0x12,0x3f,0x3f,0x00}}, 

{0x35, 1, {0x00}},

	{REGFLAG_DELAY, 10, {}},

	{0x11, 1, {0x00}},
        {0x29, 1, {0x00}},

	{0x2c,1,{0x00}},	
	// Note
	// Strongly recommend not to set Sleep out / Display On here. That will cause messed frame to be shown as later the backlight is on.


	// Setting ending by predefined flag 
	{REGFLAG_END_OF_TABLE, 0x00, {}}	

};

#endif
static struct LCM_setting_table lcm_sleep_out_setting[] = {
#if 1	
    // Sleep Out
	{0x11, 1, {0x00}},
    {REGFLAG_DELAY, 150, {}},
    // Display ON
	{0x29, 1, {0x00}},
	{REGFLAG_DELAY, 50, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
#else
	// Sleep Out
	{0x11, 0, {}},
	{REGFLAG_DELAY, 120, {}},

	// Display ON
	{0x29, 0, {}},
	{REGFLAG_DELAY, 50, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
#endif
};


static struct LCM_setting_table lcm_sleep_mode_in_setting[] = {
#if 1
	// Display off sequence
	{0x28, 1, {0x00}},
	{REGFLAG_DELAY, 50, {}},

    // Sleep Mode On

	{0x10, 1, {0x00}},
	{REGFLAG_DELAY, 150, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
#else
	// Display off sequence
	{0x28, 0, {}},
	{REGFLAG_DELAY, 150, {}},

	// Sleep Mode On
	{0x10, 0, {}},
	{REGFLAG_DELAY, 100, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
#endif
};


static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)

{
	unsigned int i;

    for(i = 0; i < count; i++){ 
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
				MDELAY(2);
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

#if 1
static void lcm_get_params(LCM_PARAMS *params)
{
		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

       //1 SSD2075 has no TE Pin 
		// enable tearing-free
        #if (LCM_DSI_CMD_MODE)
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
	 #else
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_DISABLED;
	#endif
		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

        #if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
        #else
		params->dsi.mode   = SYNC_PULSE_VDO_MODE;
		//params->dsi.mode   = BURST_VDO_MODE;
		//params->dsi.mode   = SYNC_EVENT_VDO_MODE; 
		
        #endif
	
		// DSI
		/* Command mode setting */
		//1 Three lane or Four lane
		params->dsi.LANE_NUM				= LCM_TWO_LANE;
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Highly depends on LCD driver capability.
		// Not support in MT6573
		params->dsi.packet_size=256;

        #if (LCM_DSI_CMD_MODE)
		// Video mode setting		
		params->dsi.intermediat_buffer_num = 2;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage
	#else
		params->dsi.intermediat_buffer_num = 2/*0*/;		// ??? FANG Zhuo
	#endif

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		//params->dsi.word_count=720*3;	

		
		params->dsi.vertical_sync_active				= 1;  //---3
		params->dsi.vertical_backporch					= 16; //---14
		params->dsi.vertical_frontporch					= 15;  //----8
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 6;  //----2
		params->dsi.horizontal_backporch				= 40; //----28
		params->dsi.horizontal_frontporch				= 40; //----50
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;


        //	params->dsi.HS_PRPR=6;
	    //params->dsi.LPX=8; 
		//params->dsi.HS_PRPR=5;
		//params->dsi.HS_TRAIL=13;
		params->dsi.pll_select=0;	//0: MIPI_PLL; 1: LVDS_PLL

		 #if (LCM_DSI_CMD_MODE)
		params->dsi.pll_div1=38;		// div1=0,1,2,3;div1_real=1,2,4,4 ----0: 546Mbps  1:273Mbps
		params->dsi.pll_div2=1;		// div2=0,1,2,3;div1_real=1,2,4,4			 
		 #else
		// Bit rate calculation
		//1 Every lane speed
		params->dsi.pll_div1=0;		// div1=0,1,2,3;div1_real=1,2,4,4 ----0: 546Mbps  1:273Mbps
		params->dsi.pll_div2=1;		// div2=0,1,2,3;div1_real=1,2,4,4	
		params->dsi.fbk_div =16;//30;    // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)	
		#endif

		
#if 0
params->dsi.HS_TRAIL    = 10;//10;//min max(n*8*UI, 60ns+n*4UI)
params->dsi.HS_ZERO    = 10;//8;//min 105ns+6*UI
params->dsi.HS_PRPR    = 6;//min 40ns+4*UI; max 85ns+6UI
params->dsi.LPX    = 12;//min 50ns

params->dsi.TA_SACK=1;
params->dsi.TA_GET=60;
params->dsi.TA_SURE=18;  
params->dsi.TA_GO    = 12;//12;//4*LPX
//
params->dsi.CLK_TRAIL   = 5;//5;//min 60ns
params->dsi.CLK_ZERO    = 18;//18;//min 300ns-38ns
params->dsi.LPX_WAIT    = 10;
params->dsi.CONT_DET    = 0;

params->dsi.CLK_HS_PRPR = 4;//min 38ns; max 95ns 
#endif
		//params->dsi.compatibility_for_nvk = 1;
}
#else

static void lcm_get_params(LCM_PARAMS *params)
{

        memset(params, 0, sizeof(LCM_PARAMS));    
        params->type   = LCM_TYPE_DSI;
        params->width  = FRAME_WIDTH;
        params->height = FRAME_HEIGHT; 

	//enable tearing-free

	//params->dbi.te_mode                 =LCM_DBI_TE_MODE_VSYNC_ONLY;//LCM_DBI_TE_MODE_VSYNC_ONLY; //LCM_DBI_TE_MODE_DISABLED;

	//params->dbi.te_edge_polarity        = LCM_POLARITY_RISING;

 

	#if(LCM_DSI_CMD_MODE)
        params->dsi.mode   = CMD_MODE;
	#else
        params->dsi.mode   = SYNC_PULSE_VDO_MODE;
	#endif

    

        // DSI

        /* Command mode setting */
        params->dsi.LANE_NUM                = LCM_TWO_LANE;
        //The following defined the fomat for data coming from LCD engine.
        params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
        params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
        params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
        params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;
		
        //Highly depends on LCD driver capability.
        // Not support in MT6573
        params->dsi.packet_size=256;

        //Video mode setting
        params->dsi.intermediat_buffer_num = 2;
		
        params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888; 

        #if 0
        params->dsi.vertical_sync_active                = 2;//4;
        params->dsi.vertical_backporch                  = 4;//8;
        params->dsi.vertical_frontporch                 = 4;//8;
        #else
        params->dsi.word_count=480*3;
        params->dsi.vertical_sync_active                = 4;//2;//4;
        params->dsi.vertical_backporch                  = 16;//4;//8;
        params->dsi.vertical_frontporch                 = 8;//2;
        #endif
		
        params->dsi.vertical_active_line                = FRAME_HEIGHT; 
        params->dsi.horizontal_sync_active              = 6;
        params->dsi.horizontal_backporch                = 37;
        params->dsi.horizontal_frontporch               = 37;
        params->dsi.horizontal_active_pixel             = FRAME_WIDTH;
		
        #if 0
        // Bit rate calculation
        params->dsi.pll_div1=33;//29;       // fref=26MHz, fvco=fref*(div1+1)   (div1=0~63, fvco=500MHZ~1GHz)
        params->dsi.pll_div2=1;         // div2=0~15: fout=fvo/(2*div2)
        #else
        // Bit rate calculation
	//        params->dsi.pll_div1=30;        // fref=26MHz, fvco=fref*(div1+1)   (div1=0~63, fvco=500MHZ~1GHz)

	//        params->dsi.pll_div2=1;         // div2=0~15: fout=fvo/(2*div2)
        #endif


	params->dsi.PLL_CLOCK = LCM_DSI_6589_PLL_CLOCK_201_5; //��Ҫ����clock̫\u017eߣ���ο\u0152FAQ\u0152��?
 
        /* ESD or noise interference recovery For video mode LCM only. */ // Send TE packet to LCM in a period of n frames and check the response. 
        //params->dsi.lcm_int_te_monitor = FALSE; 
        //params->dsi.lcm_int_te_period = 0; // Unit : frames 

        // Need longer FP for more opportunity to do int. TE monitor applicably. 
        //if(params->dsi.lcm_int_te_monitor) 
        //  params->dsi.vertical_frontporch *= 2; 

         // Monitor external TE (or named VSYNC) from LCM once per 2 sec. (LCM VSYNC must be wired to baseband TE pin.) 
       // params->dsi.lcm_ext_te_monitor = 1; 
        // Non-continuous clock 
        //params->dsi.noncont_clock = TRUE; 
        //params->dsi.noncont_clock_period = 2; // Unit : frames

}

 


#endif

static unsigned int lcm_compare_id(void);

static void lcm_init(void)
{
	lcm_compare_id();

	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(50/*10*/);
	
	SET_RESET_PIN(1);
	MDELAY(150/*200*/);      
	
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{
#if 0
	unsigned int data_array[2];
	data_array[0] = 0x00280500; // Display Off
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(50); 
	data_array[0] = 0x00100500; // Sleep In
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(150);
#else

    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(6);//Must > 5ms
    SET_RESET_PIN(1);
    MDELAY(50);//Must > 50ms

    push_table(lcm_sleep_mode_in_setting, sizeof(lcm_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
#endif
}


static void lcm_resume(void)
{
#if 1
    lcm_init();
    //push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
#else	
	//push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);

	unsigned int data_array[2];

	data_array[0] = 0x00110500; // Sleep In
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(150); 

	data_array[0] = 0x00290500; // Display Off
	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(50);
#endif

}
         
#if (LCM_DSI_CMD_MODE)
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

#if 0
	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	dsi_set_cmdq(&data_array, 3, 1);

	data_array[0]= 0x00053902;
	data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[2]= (y1_LSB);
	dsi_set_cmdq(&data_array, 3, 1);
	
	data_array[0] = 0x00290508;
	dsi_set_cmdq(&data_array, 1, 1);

	data_array[0]= 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);
#endif

#if 0

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	dsi_set_cmdq(data_array, 3, 1);
	
	data_array[0]= 0x00053902;
	data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[2]= (y1_LSB);
	dsi_set_cmdq(data_array, 3, 1);
	
	data_array[0]= 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);
#endif	


#if 1

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	data_array[3]= 0x00053902;
	data_array[4]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[5]= (y1_LSB);
	data_array[6]= 0x002c3909;

	dsi_set_cmdq(&data_array, 7, 0);

#endif
}
#endif

static unsigned int lcm_compare_id(void)
{

    int   array[4];
    int   id=0;
    char  buffer[4];
    char  id0=0;
    char  id1=0;
    char  id2=0;
    char  id3=0;

    SET_RESET_PIN(0);
    MDELAY(200);
    SET_RESET_PIN(1);
    MDELAY(200);

#if 0
	array[0] = 0x00043700;// read id return two byte,version and id
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0xD3,buffer, 4);
	
	//array[0] = 0x00030000;// read id return two byte,version and id
	//dsi_set_cmdq(array, 1, 1);
	//read_reg_v2(0xA1,buffer+1, 1);
	
	//array[0] = 0x00030000;// read id return two byte,version and id
	//dsi_set_cmdq(array, 1, 1);
	//read_reg_v2(0xA1,buffer+2, 1);
	
	id0 = buffer[0];//should be dummy
	id1 = buffer[1];//should be 0x00
	id2 = buffer[2];//should be 0x98
	id3 = buffer[3];//should be 0x06
	id=id2<<8 | id3;

	#ifdef BUILD_LK
		printf("FANG Zhuo uboot %s\n", __func__);
		printf("%s, id1 = 0x%08x\n", __func__, id2);//should be 0xaa
		printf("%s, id2 = 0x%08x\n", __func__, id3);//should be 0x55
	#else
		printk("FANG Zhuo kernel %s\n", __func__);
		printk("%s, id2 = 0x%08x\n", __func__, id2);//should be 0x55
		printk("%s, id3 = 0x%08x\n", __func__, id3);//should be 0x55
	#endif
	
#else
        array[0] = 0x00033700;// read id return two byte,version and id
        dsi_set_cmdq(array, 1, 1);
        read_reg_v2(0xD3, buffer, 4/*3*/);
        id = (buffer[1]<<8) | buffer[2];

		id0 = buffer[0];//should be dummy
		id1 = buffer[1];//should be 0x00
		id2 = buffer[2];//should be 0x98
		id3 = buffer[3];//should be 0x06

    	#ifdef BUILD_LK
		printf("FANG Zhuo LK %s\n", __func__);
		printf("%s, id1 = 0x%08x\n", __func__, id0);
		printf("%s, id2 = 0x%08x\n", __func__, id1);
		printf("%s, id3 = 0x%08x\n", __func__, id2);
		printf("%s, id4 = 0x%08x\n", __func__, id3);		
		printf("%s, id = 0x%08x\n", __func__,id); 
        #else
		printk("FANG Zhuo kernel %s\n", __func__);
		printk("%s, id0 = 0x%08x\n", __func__, id0);
		printk("%s, id1 = 0x%08x\n", __func__, id1);
		printk("%s, id2 = 0x%08x\n", __func__, id2);
		printk("%s, id3 = 0x%08x\n", __func__, id3);		
		printk("%s, id = 0x%08x\n", __func__,id);   
        #endif
		
#endif


#if 0
	return 1;
#else
		return (id == LCM_ID_ILI9806)? 1:0;
#endif
}

#if 0
static unsigned int lcm_esd_check(void)
{
#ifndef BUILD_LK
	char  buffer[3];
	int   array[4];

	/// please notice: the max return packet size is 1
	/// if you want to change it, you can refer to the following marked code
	/// but read_reg currently only support read no more than 4 bytes....
	/// if you need to read more, please let BinHan knows.
	/*
			unsigned int data_array[16];
			unsigned int max_return_size = 1;
			
			data_array[0]= 0x00003700 | (max_return_size << 16);	
			
			dsi_set_cmdq(&data_array, 1, 1);
	*/
	array[0] = 0x00023700;// read id return two byte,version and id
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0xA1, buffer, 2);
	if(buffer[0]==0x20 && buffer[1] == 0x75)
	{
		return FALSE;
	}
	else
	{			 
		return TRUE;
	}
#endif

}

static unsigned int lcm_esd_recover(void)
{
	lcm_init();
	lcm_resume();

	return TRUE;
}
#endif

LCM_DRIVER ili9806_fwvga_dsi_vdo_belij_lcm_drv = 
{
    .name			= "ili9806_fwvga_dsi_vdo_belij",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id    = lcm_compare_id,
//	.esd_check = lcm_esd_check,
//	.esd_recover = lcm_esd_recover,
    #if (LCM_DSI_CMD_MODE)
    .update         = lcm_update,
    #endif
    };
