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
#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
#else
#include <linux/string.h>
#if defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
	#include <mach/mt_gpio.h>
#endif
#endif
#include "lcm_drv.h"


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(540)
#define FRAME_HEIGHT 										(960)
#define LCM_ID       (0x88)

#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xFF   // END OF REGISTERS MARKER

#define LCM_DSI_CMD_MODE									0

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

#define LCM_TABLE_V3

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

#define dsi_set_cmdq_V3(ppara, size, force_update)	        	lcm_util.dsi_set_cmdq_V3(ppara, size, force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)         

static struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};

#if defined(LCM_TABLE_V3)

static LCM_setting_table_V3 lcm_initialization_setting_V3[] = {
	
	/*
	Note :

	Structure Format :

	{Data_ID, DCS command, count of parameters, {parameter list}},

	{REGFLAG_ESCAPE_ID, REGFLAG_DELAY_MS_V3,	milliseconds of time,		{}},

	...

	*/
{0x39,0xB9,0x03,{0xFF,0x83,0x89}},

{0x39,0xBA,0x07,{0x41,0x93,0x00,0x16,0xA4,0x10,0x18}},// Set MIPI

{0x15, 0xCC,0x01,{0x02}},//SS_PANEL[3] GS_PANEL[2] REV_PANEL[1] BGR_PANEL[0]

{0x15, 0xC6,0x01,{0x08}},

{0x39,0xB1,0x13,{ // Set Power                                       
						0x00,  									                                 
						0x00,  									                                 
						0x07,  //04 120804									                       
						0xEB,  // VGH=15.6V							                         
						0x50,  // VGL=-10V						                             
						0x10,  									                                 
						0x11,  									                                 
						0xB3,  // VSP=5.4V	0xb0 120809					                   
						0xF3,  // VSN=5.4V						                             
						0x2F,  // VSPR=4.697V 					                           
						0x37,  // VSNR=-4.697V					                           
						0x27,  // VGSP= 4.697-4.669=0.028V		                     
						0x27,  // VGSN= -4.669+4.697=-0.028V		                   
						0x43,  // HX5186 Mode						                         
						0x01,  //01 120809									                       
						0x58,  									                                 
						0xF2,  									                                 
						0x00,  									                                 
						0xE6                                                     
						}},//Internal Charge Pump
//{0x39, 0, 0, 0, 0,0x14,{0xB1,0x00,0x00,0x07,0xEB,0x58,0x10,0x11,0xD4,0xF0,0x2F,0x37,0x26,0x26,0x42,0x01,0x3A,0xFC,0x00,0xE6}},//Internal Charge Pump
 	
 	
{0x39,0xB2,0x07,{   // bit7:0=DCS Command
												0x00,   // Parameter1        
												0x00,   // Parameter2        
												0x78,   // Parameter3        
												0x0C,   // Parameter4        
												0x07,   // Parameter5        
												0x3F,   // Parameter6        
												0x80   // Parameter7  f0    
												}},

{0x39,0xB4,0x17,{    // SET CYC             
												0x92,  //0X80 120804	           
												0x08,  	                       
												0x00,  	                       
												0x32,  	                       
												0x10,  	                       
												0x04,  	                       
												0x32,  	                       
												0x10,  	                       
												0x00,  	                       
												0x32,  	                       
												0x10,  	                       
												0x00,  	                       
												0x37,  	                       
												0x0A,   //new 0A 120806         
												0x40,   //new 40 120806         
												0x08,  	                       
												0x37,  	                       
												0x0A,   //new  0X0A 120806      
												0x40,   //new  0X40 120806      
												0x14,  	                       
												0x46,  	                       
												0x50,  	                       
												0x0A	                       
}},

{0x39,0xD5,0x38,{0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x60,0x00,0x88,0x88,0x88,0x88,0x88,0x23,0x88,0x01,0x88,0x67,0x88,0x45,0x01,
0x23,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x88,0x54,0x88,0x76,0x88,0x10,0x88,0x32,0x32,0x10,0x88,0x88,0x88,0x88,
0x88,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
						
{0x39,0xDE,0x02,{0x05,0x58}},

{0x39,0xB6,0x04,{0x00,0xA4,0x00,0xA4}},
						
{0x39,0xE0,0x22,{ // Set Gamma 
                  // positive 
                0x05,   //V0 
                0x11,   //V1 
                0x14,   //V2 
                0x17,   //V14 
                0x18,   //V15 
                0x3C,   //V16 
                0x22,   //V5 
                0x3A,   //V11 
                0x08,     
                0x0E,   
                0x13,   
                0x18,   
                0x1B,   
                0x17,   
                0x17,   
                0x17,   //0x07->0x17 
                0x16,   
       //negative 
                0x05,   //V0 
                0x11,   //V1 
                0x14,   //V2 
                0x17,   //V14 
                0x18,   //V15 
                0x3C,   //V16 
                0x22,   //V5 
                0x3A,   //V11 
                0x08,     
                0x0E,   
                0x13,   
                0x18,   
                0x1B,   
                0x17,   
                0x17,   
                0x17,   //0x07->0x17 
                0x16}}
/*
						
{0x39,0xC1,0x7f,{0x01,0x00,0x1C,0x2A,0x35,0x41,0x4B,0x55,0x5D,0x65,0x6D,0x75,0x7C,0x82,0x89,0x8F,0x95,0x9C,0xA1,0xA7,0xAC,
0xB2,0xBA,0xC2,0xC8,0xCB,0xD2,0xDA,0xE2,0xE6,0xEE,0xF4,0xFA,0xFF,0x39,0x37,0xE0,0xC7,0x67,0xC8,0xC1,0x58,0xC0,0x00,0x1C,
0x2A,0x35,0x41,0x4B,0x55,0x5D,0x65,0x6D,0x75,0x7C,0x82,0x89,0x8F,0x95,0x9C,0xA1,0xA7,0xAC,0xB2,0xBA,0xC1,0xC8,0xCB,0xD2,
0xDA,0xE2,0xE6,0xEE,0xF4,0xFA,0xFF,0x39,0x37,0xE0,0xC7,0x67,0xC8,0xC1,0x58,0xC0,0x00,0x1C,0x2A,0x35,0x41,0x4B,0x55,0x5D,
0x65,0x6D,0x75,0x7C,0x82,0x89,0x8F,0x95,0x9C,0xA1,0xA7,0xAC,0xB2,0xBA,0xC1,0xC8,0xCB,0xD2,0xDA,0xE2,0xE6,0xEE,0xF4,0xFA,
0xFF,0x39,0x37,0xE0,0xC7,0x67,0xC8,0xC1,0x58,0xC0}}*/

};

static LCM_setting_table_V3 lcm_sleep_out_setting_V3[] = {
    // Sleep Out
	{0x15,	0x11, 	1, {0x00}},
    {REGFLAG_ESCAPE_ID, REGFLAG_DELAY_MS_V3,	150,		{}},
    // Display ON
	{0x15,	0x29, 	1, {0x00}},
	{REGFLAG_ESCAPE_ID, REGFLAG_DELAY_MS_V3,	40,		{}}

};


static LCM_setting_table_V3 lcm_deep_sleep_mode_in_setting_V3[] = {
	// Display off sequence
	{0x15,	0x28, 	1, {0x00}},
	{REGFLAG_ESCAPE_ID, REGFLAG_DELAY_MS_V3,	20,		{}},
    // Sleep Mode On
	{0x15,	0x10, 	1, {0x00}},
	{REGFLAG_ESCAPE_ID, REGFLAG_DELAY_MS_V3,	200,		{}}
};

static LCM_setting_table_V3 lcm_compare_id_setting_V3[] = {

	{0x39,	0xB9,	3,  {0xFF, 0x83, 0x89}},
	{0x39,  0xBA,   2,  {0x01,0x92}},
	{REGFLAG_ESCAPE_ID, REGFLAG_DELAY_MS_V3,	10,		{}}
};


static LCM_setting_table_V3 lcm_backlight_level_setting_V3[] = {
	{0x15,	0x51, 	1, {0xFF}}
};


#else

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

	
	{0xB9,	3,	{0xFF, 0x83, 0x69}},
	{REGFLAG_DELAY, 5, {}},

	{0xB0,	2,	{0x01, 0x0B}},
	{REGFLAG_DELAY, 5, {}},

	{0xB2,	15,	{0x00, 0x20, 0x05, 0x05,
				 0x70, 0x00, 0xFF, 0x00,
				 0x00, 0x00, 0x00, 0x03,
				 0x03, 0x00, 0x01}},
	{REGFLAG_DELAY, 5, {}},

	{0xB4, 	5,	{0x00, 0x18, 0x80, 0x06,
				 0x02}},
	{REGFLAG_DELAY, 5, {}},

	{0xD5,	26, {0x00, 0x04, 0x03, 0x00,
				 0x01, 0x05, 0x28, 0x70,
				 0x01, 0x03, 0x00, 0x00,
				 0x40, 0x06, 0x51, 0x07,
				 0x00, 0x00, 0x41, 0x06,
				 0x50, 0x07, 0x07, 0x0F,
				 0x04, 0x00}},
	{REGFLAG_DELAY, 5, {}},

	{0xB1,	19,	{0x85, 0x00, 0x34, 0x07,
				 0x00, 0x0f, 0x0f, 0x2A,
				 0x32, 0x3F, 0x3F, 0x01,
				 0x3A, 0x01, 0xE6, 0xE6,
				 0xE6, 0xE6, 0xE6}},
	{REGFLAG_DELAY, 5, {}},


	{0x3A,	1,	{0x07}},
	{0xCC,	1,	{0x02}},

	{0xB6,	2,	{0x42, 0x42}},
	{REGFLAG_DELAY, 5, {}},

	// SET GAMMA
	{0xE0,	34,	{0x00, 0x31, 0x19, 0x38,
				 0x3D, 0x3F, 0x28, 0x46,
				 0x07, 0x0D, 0x0E, 0x12,
				 0x15, 0x12, 0x14, 0x0F,
				 0x17, 0x00, 0x13, 0x19,
				 0x38, 0x3D, 0x3F, 0x28,
				 0x46, 0x07, 0x0D, 0x0E,
				 0x12, 0x15, 0x12, 0x14,
				 0x0F, 0x17}},
	{REGFLAG_DELAY, 5, {}}, 

	{0xBA,	13,	{0x00, 0xA0, 0xC6, 0x00,
				 0x0A, 0x02, 0x10, 0x30,
				 0x6F, 0x02, 0x11, 0x18,
				 0x40}},

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
	{0x11, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},

    // Display ON
	{0x29, 1, {0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	// Display off sequence
	{0x28, 1, {0x00}},

    // Sleep Mode On
	{0x10, 1, {0x00}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_compare_id_setting[] = {

	{0xB9,	3,	{0xFF, 0x83, 0x69}},
	{REGFLAG_DELAY, 10, {}}, 	

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_backlight_level_setting[] = {
	{0x51, 1, {0xFF}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

#endif

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
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_DISABLED;
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
		params->dsi.fbk_div =16;//30;    // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)	
}
static unsigned int lcm_compare_id(void)
{
	unsigned int id = 0;
	unsigned char buffer[2];
	unsigned int array[16];
        SET_RESET_PIN(1);  //NOTE:should reset LCM firstly
    	SET_RESET_PIN(0);
    	MDELAY(1);
    	SET_RESET_PIN(1);
    	MDELAY(50);

#if defined(LCM_TABLE_V3)
	dsi_set_cmdq_V3(lcm_compare_id_setting_V3, sizeof(lcm_compare_id_setting_V3) / sizeof(LCM_setting_table_V3), 1);
#else
	push_table(lcm_compare_id_setting, sizeof(lcm_compare_id_setting) / sizeof(struct LCM_setting_table), 1);
#endif

	array[0] = 0x00013700;// read id return two byte,version and id
	dsi_set_cmdq(&array, 1, 1);
	MDELAY(10);
	read_reg_v2(0xDB, buffer, 1);
	id = buffer[0]; //we only need ID
#if defined(BUILD_LK)
		printf("tengdeqiang444%s, id = 0x%08x\n", __func__, id);
#else
		printk("tengdeqiang444 %s, id= 0x%08x\n", __func__, id);
#endif
    return (LCM_ID == id)?1:0;
}
static void lcm_init(void)
{
    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(1);
    SET_RESET_PIN(1);
    MDELAY(10);

#if defined(LCM_TABLE_V3)
	dsi_set_cmdq_V3(lcm_initialization_setting_V3, sizeof(lcm_initialization_setting_V3) / sizeof(LCM_setting_table_V3), 1);
#else
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
#endif
}


static void lcm_suspend(void)
{


//	push_table(lcm_sleep_in_setting, sizeof(lcm_sleep_in_setting) / sizeof(struct LCM_setting_table), 1);
	
    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(10);
    SET_RESET_PIN(1);
    MDELAY(50);//Must over 6 ms,SPEC request	
//  lcm_compare_id();

}


static void lcm_resume(void)
{
	lcm_init();

#if defined(LCM_TABLE_V3)
	dsi_set_cmdq_V3(lcm_sleep_out_setting_V3, sizeof(lcm_sleep_out_setting_V3) / sizeof(LCM_setting_table_V3), 1);
#else
	push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
#endif

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


// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER hx8389b_qhd_dsi_vdo_xinli_new_drv = 
{
    .name			= "hx8389b_dsi_vdo_xinli_new",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id    = lcm_compare_id,
#if (LCM_DSI_CMD_MODE)
    .update         = lcm_update,
#endif
};

