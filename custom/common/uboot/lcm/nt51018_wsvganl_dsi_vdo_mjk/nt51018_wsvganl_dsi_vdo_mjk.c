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
	#include <platform/mt_pmic.h>
    #include <debug.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
	#include <mach/mt_gpio.h>
	#include <mach/mt_pm_ldo.h>
#endif
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (1024)
#define FRAME_HEIGHT (600)

#define GPIO_LCD_RST         GPIO175
#define GPIO_LCD_STBTYB      GPIO174
#define GPIO_LCD_PMOS_EN     GPIO74


#define LCM_ID_OMT9608A (0x40)

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------
static LCM_UTIL_FUNCS lcm_util = {0};

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


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

       //1 SSD2075 has no TE Pin 
		// enable tearing-free
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_DISABLED;//;LCM_DBI_TE_MODE_VSYNC_ONLY
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
		params->dsi.LANE_NUM				= LCM_FOUR_LANE;
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;//
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Highly depends on LCD driver capability.
		// Not support in MT6573
		params->dsi.packet_size=256;

		// Video mode setting		
		params->dsi.intermediat_buffer_num = 0;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		//params->dsi.word_count=720*3;	

		
		params->dsi.vertical_sync_active				= 4;  //---10
		params->dsi.vertical_backporch					= 20; //---30
		params->dsi.vertical_frontporch					= 11;  //----30
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 32;  //----10
		params->dsi.horizontal_backporch				= 158; //----50
		params->dsi.horizontal_frontporch				= 130; //----50
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;


        //	params->dsi.HS_PRPR=6;
	    //params->dsi.LPX=8; 
		//params->dsi.HS_PRPR=5;
		//params->dsi.HS_TRAIL=13;

		// Bit rate calculation
		//1 Every lane speed
		params->dsi.pll_div1=1;		// div1=0,1,2,3;div1_real=1,2,4,4 ----0: 546Mbps  1:273Mbps
		params->dsi.pll_div2=1;		// div2=0,1,2,3;div1_real=1,2,4,4	
		params->dsi.fbk_div =15;    // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)	
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
		
}

static void lcm_init(void)
{
#ifdef BUILD_LK

    mt_set_gpio_mode(GPIO_LCD_PMOS_EN, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_PMOS_EN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_PMOS_EN, GPIO_OUT_ONE);
    MDELAY(20);


    //VGP6 3.3V
    pmic_config_interface(DIGLDO_CON12, 0x1, PMIC_RG_VGP6_EN_MASK, PMIC_RG_VGP6_EN_SHIFT); 
    pmic_config_interface(DIGLDO_CON33, 0x07, PMIC_RG_VGP6_VOSEL_MASK, PMIC_RG_VGP6_VOSEL_SHIFT);
    MDELAY(20);
  
#if 1
    mt_set_gpio_mode(GPIO_LCD_STBTYB, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_STBTYB, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_STBTYB, GPIO_OUT_ZERO);
    MDELAY(20);
    mt_set_gpio_mode(GPIO_LCD_STBTYB, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_STBTYB, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_STBTYB, GPIO_OUT_ONE);
    MDELAY(35);

#endif	

    mt_set_gpio_mode(GPIO_LCD_RST, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_RST, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ZERO);
    MDELAY(20);
    mt_set_gpio_mode(GPIO_LCD_RST, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_RST, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ONE);
    MDELAY(35);
	
   

#else

    mt_set_gpio_mode(GPIO_LCD_PMOS_EN, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_PMOS_EN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_PMOS_EN, GPIO_OUT_ONE);
    MDELAY(20);


//VGP6 3.3V
    hwPowerOn(MT65XX_POWER_LDO_VGP6, VOL_3300, "LCM");
    MDELAY(20);

	//hwPowerOn(MT65XX_POWER_LDO_VAST,VOL_1200, "LCM");
    //MDELAY(20);
	
#if 1
    mt_set_gpio_mode(GPIO_LCD_STBTYB, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_STBTYB, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_STBTYB, GPIO_OUT_ZERO);
    MDELAY(20);
    mt_set_gpio_mode(GPIO_LCD_STBTYB, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_STBTYB, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_STBTYB, GPIO_OUT_ONE);
    MDELAY(35);

#endif    

    mt_set_gpio_mode(GPIO_LCD_RST, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_RST, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ZERO);
    MDELAY(20);
    mt_set_gpio_mode(GPIO_LCD_RST, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_RST, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ONE);
   
#endif    
}

static void lcm_suspend(void)
{
    mt_set_gpio_mode(GPIO_LCD_RST, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_RST, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ZERO);
 #ifdef BUILD_LK
    pmic_config_interface(DIGLDO_CON12, 0x0, PMIC_RG_VGP6_EN_MASK, PMIC_RG_VGP6_EN_SHIFT); 
    pmic_config_interface(DIGLDO_CON33, 0x0, PMIC_RG_VGP6_VOSEL_MASK, PMIC_RG_VGP6_VOSEL_SHIFT);
 #else
    hwPowerDown(MT65XX_POWER_LDO_VGP6, "LCM");
 #endif   
}


static void lcm_resume(void)
{
#ifdef BUILD_LK
    //VGP6 3.3V
    pmic_config_interface(DIGLDO_CON12, 0x1, PMIC_RG_VGP6_EN_MASK, PMIC_RG_VGP6_EN_SHIFT); 
    pmic_config_interface(DIGLDO_CON33, 0x07, PMIC_RG_VGP6_VOSEL_MASK, PMIC_RG_VGP6_VOSEL_SHIFT);
    MDELAY(20);
    

    mt_set_gpio_mode(GPIO_LCD_RST, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_RST, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ZERO);
    MDELAY(1000);
    mt_set_gpio_mode(GPIO_LCD_RST, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_RST, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ONE);
#else
   //VGP6 3.3V
    hwPowerOn(MT65XX_POWER_LDO_VGP6, VOL_3300, "LCM");
    MDELAY(20);
	//hwPowerOn(MT65XX_POWER_LDO_VAST,VOL_1200, "LCM");
    //MDELAY(20);
	
   

    mt_set_gpio_mode(GPIO_LCD_RST, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_RST, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ZERO);
    MDELAY(1000);
    mt_set_gpio_mode(GPIO_LCD_RST, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_RST, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ONE);
    MDELAY(25);

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

	if(buffer[0]==0x01 && buffer[1] == 0x8B)
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

LCM_DRIVER nt51018_wsvganl_dsi_vdo_mjk_lcm_drv = 
{
    .name			= "nt51018_wsvganl_dsi_vdo_mjk",
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
