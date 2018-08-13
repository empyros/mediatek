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

#define FRAME_WIDTH  (540)
#define FRAME_HEIGHT (960)

#define LCM_ID_OMT9608A (0x40)

#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0XFD   // END OF REGISTERS MARKER

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

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)			lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)							lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)				lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)							lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   			lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer,buffer_size) 


#define   LCM_DSI_CMD_MODE							1


struct LCM_setting_table {
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

    {0x00, 1 ,{0x00}}, 
    {0xFF, 3  ,{0x96,0x08,0x01}},
    {0x00, 1 ,{0x80}}, 
    {0xFF, 2  ,{0x96,0x08}},
    
    {0x00, 1 ,{0x00}}, 
    {0xA0, 1  ,{0x00}},
    
    {0x00, 1 ,{0x80}}, 
    {0xB3, 5  ,{0x00,0x00,0x00,0x21,0x00}},

    {0x00, 1 ,{0x92}}, 
    {0xB3, 1  ,{0x01}},

    {0x00, 1 ,{0xC0}}, 
    {0xB3, 1  ,{0x11}},
    
    {0x00, 1 ,{0x80}}, 
    {0xC0, 9  ,{0x00,0x4A,0x00,0x0A,0x0A,0x00,0x4A,0x0A,0x0A}},
    
    {0x00, 1 ,{0x92}}, 
    {0xC0, 4  ,{0x00,0x0E,0x00,0x11}},

    {0x00, 1 ,{0xA2}}, 
    {0xC0, 3  ,{0x00,0x10,0x00}},
    
    {0x00, 1 ,{0xB3}}, 
    {0xC0, 2  ,{0x00,0x50}},
    
    {0x00, 1 ,{0x81}}, 
    {0xC1, 1  ,{0x55}},
    
    {0x00, 1 ,{0x80}}, 
    {0xC4, 3  ,{0x30,0x84,0xFA}},  //0x00 ,84,fa

    {0x00, 1 ,{0x88}}, 
    {0xC4, 1  ,{0x40}},  //0x00 ,84,fa

    {0x00, 1 ,{0xB4}}, 
    {0xC0, 1  ,{0x05}},  //0x00 ,84,fa
    
    {0x00, 1 ,{0xA0}}, 
    {0xC4, 8  ,{0x33,0x09,0x90,0x2B,0x33,0x09,0x90,0x54}},
    
    {0x00, 1 ,{0x80}}, 
    {0xC5, 4  ,{0x08,0x00,0xA0,0x11}},
    
    {0x00, 1 ,{0x90}}, 
    {0xC5, 7  ,{0x96,0x81,0x06,0x81,0x33,0x33,0x34}},
    
    {0x00, 1 ,{0xA0}}, 
    {0xC5, 7  ,{0x96,0x81,0x06,0x81,0x33,0x33,0x34}},

    {0x00, 1 ,{0xB6}}, 
    {0xF5, 4  ,{0x15,0x00,0x15,0x00}}, 
  
    {0x00, 1 ,{0xB0}}, 
    {0xC5, 2  ,{0x04,0xA8}},
    
    {0x00, 1 ,{0x80}}, 
    {0xC6, 1  ,{0x64}},
    
    {0x00, 1 ,{0xB0}},
    {0xC6, 5  ,{0x03,0x10,0x00,0x1F,0x12}},

    {0x00, 1 ,{0xE1}}, 
    {0xC0, 1  ,{0x96}},

    {0x00, 1 ,{0x00}}, 
    {0xD0, 1  ,{0x02}},
   
    {0x00, 1 ,{0x00}}, 
    {0xD1, 2  ,{0x01,0x03}},

    {0x00, 1 ,{0xB7}}, 
    {0xB0, 1  ,{0x10}},

    {0x00, 1 ,{0xC0}}, 
    {0xB0, 1  ,{0x55}},
    
    {0x00, 1 ,{0xB1}}, 
    {0xB0, 1  ,{0x03}},
    
    {0x00, 1 ,{0x81}}, 
    {0xD6, 1  ,{0x00}},

    {0x00, 1 ,{0x80}}, 
    {0xCB, 10 ,{0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA}},
    
    {0x00, 1 ,{0x90}}, 
    {0xCB, 15 ,{0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA}},
    
    {0x00, 1 ,{0xA0}}, 
    {0xCB, 15 ,{0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA}},
    
    {0x00, 1 ,{0xB0}}, 
    {0xCB, 10 ,{0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA}},
    
    {0x00, 1 ,{0xC0}}, 
    {0xCB, 15 ,{0xA6,0xA6,0xA6,0xA6,0xAA,0xAA,0xA6,0xA6,0xAA,0xAA,0xA6,0xA6,0xA6,0xA6,0xAA}},

    {0x00, 1 ,{0xD0}}, 
    {0xCB, 15 ,{0xAA,0xAA,0xAA,0xAA,0xAA,0xA6,0xA6,0xA6,0xA6,0xA6,0xA6,0xAA,0xAA,0xA6,0xA6}},
   
    {0x00, 1 ,{0xE0}}, 
    {0xCB, 10 ,{0xAA,0xAA,0xAA,0xAA,0xA6,0xA6,0xAA,0xAA,0xAA,0xAA}},

    {0x00, 1 ,{0xF0}}, 
    {0xCB, 10 ,{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}},

    {0x00, 1 ,{0x80}}, 
    {0xCC, 10 ,{0x00,0x00,0x02,0x04,0x00,0x00,0x0A,0x0E,0x00,0x00}},
 
    {0x00, 1 ,{0x90}}, 
    {0xCC, 15 ,{0x0C,0x10,0x08,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x09}},
    
    {0x00, 1 ,{0xA0}}, 
    {0xCC, 15 ,{0x0D,0x00,0x00,0x0B,0x0F,0x00,0x00,0x00,0x00,0x07,0x05,0x00,0x00,0x00,0x00}},
    
    {0x00, 1 ,{0xB0}}, 
    {0xCC, 10 ,{0x00,0x00,0x07,0x05,0x00,0x00,0x0F,0x0B,0x00,0x00}},
    
    {0x00, 1 ,{0xC0}}, 
    {0xCC, 15 ,{0x0D,0x09,0x01,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x06,0x10}},

    {0x00, 1 ,{0xD0}}, 
    {0xCC, 15 ,{0x0C,0x00,0x00,0x0E,0x0A,0x00,0x00,0x00,0x00,0x02,0x04,0x00,0x00,0x00,0x00}},

    {0x00, 1 ,{0x80}}, 
    {0xCE, 12 ,{0x86,0x03,0x18,0x85,0x03,0x18,0x84,0x03,0x18,0x83,0x03,0x18}},
    
    {0x00, 1 ,{0x90}}, 
    {0xCE, 14 ,{0x33,0xBD,0x18,0x33,0xBE,0x18,0x33,0xBF,0x18,0x33,0xC0,0x18,0x00,0x00}},
    
    {0x00, 1 ,{0xA0}}, 
    {0xCE, 14 ,{0x38,0x02,0x83,0xC1,0x89,0x18,0x00,0x38,0x01,0x83,0xC2,0x89,0x18,0x00}},
   
    {0x00, 1 ,{0xB0}}, 
    {0xCE, 14 ,{0x38,0x00,0x83,0xC3,0x88,0x18,0x00,0x30,0x00,0x83,0xC4,0x87,0x18,0x00}},
    
    {0x00, 1 ,{0xC0}}, 
    {0xCE, 14 ,{0x30,0x01,0x83,0xC5,0x86,0x18,0x00,0x30,0x02,0x83,0xC6,0x85,0x18,0x00}},
    
    {0x00, 1 ,{0xD0}}, 
    {0xCE, 14 ,{0x30,0x03,0x83,0xC7,0x84,0x18,0x00,0x30,0x04,0x83,0xC8,0x83,0x18,0x00}},
    
    {0x00, 1 ,{0x80}}, 
    {0xCF, 14 ,{0xF0,0x00,0x00,0x10,0x00,0x00,0x00,0xF0,0x00,0x00,0x10,0x00,0x00,0x00}},
    
    {0x00, 1 ,{0x90}}, 
    {0xCF, 14 ,{0xF0,0x00,0x00,0x10,0x00,0x00,0x00,0xF0,0x00,0x00,0x10,0x00,0x00,0x00}},
    
    {0x00, 1 ,{0xA0}},
    {0xCF, 14 ,{0xF0,0x00,0x00,0x10,0x00,0x00,0x00,0xF0,0x00,0x00,0x10,0x00,0x00,0x00}},
    
    {0x00, 1 ,{0xB0}}, 
    {0xCF, 14 ,{0xF0,0x00,0x00,0x10,0x00,0x00,0x00,0xF0,0x00,0x00,0x10,0x00,0x00,0x00}},
    
    {0x00, 1 ,{0xC0}}, 
    {0xCF, 10 ,{0x01,0x01,0x00,0x00,0x00,0x00,0x02,0x81,0x20,0x09}},

    {0x00, 1 ,{0x80}}, 
    {0xD6, 1  ,{0x00}},
    
    {0x00, 1 ,{0x00}}, 
    {0xD7, 1  ,{0x00}},

    {0x00, 1 ,{0x00}}, 
    {0xD8, 2  ,{0x1F,0x1F}},

    {0x00, 1 ,{0x00}}, 
    {0xD9, 1  ,{0x1A}},

    {0x00, 1 ,{0x00}}, 
    {0xE1, 16 ,{0x09,0x11,0x17,0x0D,0x06,0x0E,0x0A,0x08,0x05,0x09,0x0D,0x07,0x0E,0x0E,0x0A,0x08}},   

    {0x00, 1 ,{0x00}}, 
    {0xE2, 16 ,{0x09,0x11,0x17,0x0D,0x06,0x0E,0x0A,0x08,0x05,0x09,0x0D,0x07,0x0E,0x0E,0x0A,0x08}},   

    {0x00, 1 ,{0x00}}, 
    {0xEC, 34 ,{0x40,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,
		0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,
		0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,
		0x44,0x44,0x44,0x00}},

    {0x00, 1 ,{0x00}}, 
    {0xED, 34 ,{0x40,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,
		0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,
		0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,
		0x44,0x44,0x44,0x00}},

    {0x00, 1 ,{0x00}}, 
    {0xEE, 34 ,{0x40,0x44,0x44,0x44,0x44,0x44,0x54,0x44,0x44,0x44,
		0x45,0x44,0x44,0x45,0x44,0x44,0x44,0x44,0x54,0x44,
		0x44,0x44,0x44,0x44,0x44,0x34,0x44,0x44,0x34,0x44,
		0x34,0x34,0x44,0x00}},

    {0x00, 1 ,{0x00}}, 
    {0xFF, 3  ,{0xFF,0xFF,0xFF}},
    
    {0x3A, 1 ,{0x77}},
    
    {0x35, 1 ,{0x00}}, //open te

    {0x44, 2 ,{50,0x00}}, //set te N line

    {0x36, 1 ,{0xd0}}, //180  //0xco
    
    {0x11, 1 ,{0x00}}, 
    {REGFLAG_DELAY, 200, {}},
    
    {0x29, 1 ,{0x00}}, 
    
    {REGFLAG_DELAY, 10, {}},
    
    //{0x2C, 1 ,{0x00}}, 
    	// Note
    	// Strongly recommend not to set Sleep out / Display On here. That will cause messed frame to be shown as later the backlight is on.
    	// Setting ending by predefined flag
    
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
	{0x11, 1, {0x00}},
	{REGFLAG_DELAY, 20, {}},
	// Display ON
	{0x29, 1, {0x00}},
	{REGFLAG_DELAY, 120, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_in_setting[] = {

	// Display off sequence
	{0x28, 1, {0x00}},
	{REGFLAG_DELAY, 120, {}},

    // Sleep Mode On

	{0x10, 1, {0x00}},
	{REGFLAG_DELAY, 120, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}

};


void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)

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

static void lcm_get_params(LCM_PARAMS *params)
{
		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		// enable tearing-free
		params->dbi.te_mode 			= LCM_DBI_TE_MODE_VSYNC_ONLY;//LCM_DBI_TE_MODE_DISABLED;
		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

        #if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
        #else
		params->dsi.mode   = SYNC_PULSE_VDO_MODE;	
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

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		params->dsi.vertical_active_line				= FRAME_HEIGHT;

		params->dsi.intermediat_buffer_num = 0;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage

		params->dsi.pll_div1=1;		// div1=0,1,2,3;div1_real=1,2,4,4
		params->dsi.pll_div2=1;		// div2=0,1,2,3;div1_real=1,2,4,4
		params->dsi.fbk_div =20;		// fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)		

}

static void lcm_init(void)
{
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(10);
	
	SET_RESET_PIN(1);
	MDELAY(200);      
	
    push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{
	
//push_table(lcm_sleep_in_setting, sizeof(lcm_sleep_in_setting) / sizeof(struct LCM_setting_table), 1);

	unsigned int data_array[2];
	data_array[0] = 0x00280500; // Display Off
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(120); 
	data_array[0] = 0x00100500; // Sleep In
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(120);
}


static void lcm_resume(void)
{
    unsigned int data_array[2];

	data_array[0] = 0x00110500; // Sleep In
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(120); 

    data_array[0] = 0x00290500; // Display Off
    dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(20);

	
//	push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
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
	dsi_set_cmdq(&data_array, 3, 1);

	data_array[0]= 0x00053902;
	data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[2]= (y1_LSB);
	dsi_set_cmdq(data_array, 3, 1);
	
	data_array[0]= 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);	

}


LCM_DRIVER otm9608a_qhd_dsi_cmd_truly_lcm_drv = 
{
   	 .name		= "otm9608a_qhd_dsi_cmd_truly",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	//.compare_id    = lcm_compare_id,
	//.esd_check = lcm_esd_check,
	//.esd_recover = lcm_esd_recover,
    #if (LCM_DSI_CMD_MODE)
    	.update         = lcm_update,
    #endif
    };
