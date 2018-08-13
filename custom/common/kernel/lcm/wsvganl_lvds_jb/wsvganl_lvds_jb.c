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
#ifdef BUILD_LK
#include <platform/mt_gpio.h>
#include <platform/mt_pmic.h>
#include <debug.h>
#else
#include <mach/mt_gpio.h>
#include <mach/mt_pm_ldo.h>
#endif

#include "lcm_drv.h"


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (1024)
#define FRAME_HEIGHT (600)

#define GPIO_DISP_PWR_EN     GPIO133  //boost ic
#define GPIO_LCD_RST         GPIO175
#define GPIO_LCD_STBTYB      GPIO174
#define GPIO_LCD_SHUTDOWN      GPIO132
#define GPIO_MHL_RST   GPIO30

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (mt_set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

static __inline void send_ctrl_cmd(unsigned int cmd)
{

}

static __inline void send_data_cmd(unsigned int data)
{

}

static __inline void set_lcm_register(unsigned int regIndex,
                                      unsigned int regData)
{

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

    params->type   = LCM_TYPE_DPI;
    params->ctrl   = LCM_CTRL_SERIAL_DBI;
    params->width  = FRAME_WIDTH;
    params->height = FRAME_HEIGHT;
    params->io_select_mode = 0;	

    /* RGB interface configurations */    
    params->dpi.mipi_pll_clk_ref  = 0;
    params->dpi.mipi_pll_clk_div1 = 0x80000081;  //lvds pll 204.1M
    params->dpi.mipi_pll_clk_div2 = 0x800fb333;
    params->dpi.dpi_clk_div       = 4;           //{4,2}, pll/4=51.025M
    params->dpi.dpi_clk_duty      = 2;

    params->dpi.clk_pol           = LCM_POLARITY_FALLING;
    params->dpi.de_pol            = LCM_POLARITY_RISING;
    params->dpi.vsync_pol         = LCM_POLARITY_FALLING;
    params->dpi.hsync_pol         = LCM_POLARITY_FALLING;

    params->dpi.hsync_pulse_width = 128;  //HT=1344
    params->dpi.hsync_back_porch  = 152;
    params->dpi.hsync_front_porch = 40;
    params->dpi.vsync_pulse_width = 5;    //VT=635
    params->dpi.vsync_back_porch  = 15;
    params->dpi.vsync_front_porch = 15;
    
    params->dpi.i2x_en = 0;
    
    params->dpi.format            = LCM_DPI_FORMAT_RGB888;   // format is 24 bit
    params->dpi.rgb_order         = LCM_COLOR_ORDER_RGB;
    params->dpi.is_serial_output  = 0;

    params->dpi.intermediat_buffer_num = 0;

    params->dpi.io_driving_current = LCM_DRIVING_CURRENT_6575_8MA;
}


static void lcm_init(void)
{
#ifdef BUILD_LK
    //VGP6 3.3V
    pmic_config_interface(DIGLDO_CON12, 0x1, PMIC_RG_VGP6_EN_MASK, PMIC_RG_VGP6_EN_SHIFT); 
    pmic_config_interface(DIGLDO_CON33, 0x07, PMIC_RG_VGP6_VOSEL_MASK, PMIC_RG_VGP6_VOSEL_SHIFT);
    MDELAY(20);
    mt_set_gpio_mode(GPIO_LCD_STBTYB, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_STBTYB, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_STBTYB, GPIO_OUT_ONE);
   // mt_set_gpio_out(GPIO_LCD_STBTYB, GPIO_OUT_ZERO);
    MDELAY(2);
		
	mt_set_gpio_mode(GPIO_MHL_RST, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_MHL_RST, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_MHL_RST, GPIO_OUT_ONE);
    MDELAY(3);
	
    mt_set_gpio_mode(GPIO_LCD_RST, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_RST, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ONE);
    MDELAY(20);
    mt_set_gpio_mode(GPIO_DISP_PWR_EN, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_DISP_PWR_EN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_DISP_PWR_EN, GPIO_OUT_ONE);
    MDELAY(35);
 	
    mt_set_gpio_mode(GPIO_LCD_RST, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_RST, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ZERO);
    MDELAY(2);
    mt_set_gpio_mode(GPIO_LCD_RST, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_RST, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ONE);
    MDELAY(35);
	
	mt_set_gpio_mode(GPIO_LCD_SHUTDOWN, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_SHUTDOWN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_SHUTDOWN, GPIO_OUT_ONE);//chailu for mhl 
    MDELAY(3);

#else
//VGP6 3.3V
    hwPowerOn(MT65XX_POWER_LDO_VGP6, VOL_3300, "LCM");
    MDELAY(20);

	hwPowerOn(MT65XX_POWER_LDO_VAST,VOL_1200, "LCM");
    MDELAY(20);
	
    mt_set_gpio_mode(GPIO_LCD_STBTYB, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_STBTYB, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_STBTYB, GPIO_OUT_ONE);
//	mt_set_gpio_out(GPIO_LCD_STBTYB, GPIO_OUT_ZERO);
    MDELAY(20);


    mt_set_gpio_mode(GPIO_LCD_RST, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_RST, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ONE);
    MDELAY(20);
    mt_set_gpio_mode(GPIO_DISP_PWR_EN, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_DISP_PWR_EN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_DISP_PWR_EN, GPIO_OUT_ONE);
    MDELAY(35);

    mt_set_gpio_mode(GPIO_LCD_RST, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_RST, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ZERO);
    MDELAY(2);
    mt_set_gpio_mode(GPIO_LCD_RST, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_RST, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ONE);

    MDELAY(25);
	mt_set_gpio_mode(GPIO_LCD_SHUTDOWN, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_SHUTDOWN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_SHUTDOWN, GPIO_OUT_ONE);//chailu for mhl
    MDELAY(3);
#endif    
}


static void lcm_suspend(void)
{
	mt_set_gpio_mode(GPIO_LCD_SHUTDOWN, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_SHUTDOWN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_SHUTDOWN, GPIO_OUT_ZERO);
    MDELAY(3);

    mt_set_gpio_mode(GPIO_LCD_STBTYB, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_STBTYB, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_STBTYB, GPIO_OUT_ZERO);
   // mt_set_gpio_out(GPIO_LCD_STBTYB, GPIO_OUT_ONE);
    MDELAY(50);	
    mt_set_gpio_mode(GPIO_DISP_PWR_EN, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_DISP_PWR_EN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_DISP_PWR_EN, GPIO_OUT_ZERO);
    MDELAY(50);  
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
    mt_set_gpio_mode(GPIO_LCD_STBTYB, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_STBTYB, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_STBTYB, GPIO_OUT_ONE);
   // mt_set_gpio_out(GPIO_LCD_STBTYB, GPIO_OUT_ZERO);
    MDELAY(20);

    mt_set_gpio_mode(GPIO_LCD_RST, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_RST, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ONE);
    MDELAY(20);
    mt_set_gpio_mode(GPIO_DISP_PWR_EN, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_DISP_PWR_EN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_DISP_PWR_EN, GPIO_OUT_ONE);
    MDELAY(35);

    mt_set_gpio_mode(GPIO_LCD_RST, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_RST, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ZERO);
    MDELAY(2);
    mt_set_gpio_mode(GPIO_LCD_RST, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_RST, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ONE);

	mt_set_gpio_mode(GPIO_LCD_SHUTDOWN, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_SHUTDOWN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_SHUTDOWN, GPIO_OUT_ONE);//chailu for mhl
    MDELAY(3);
#else
   //VGP6 3.3V
    hwPowerOn(MT65XX_POWER_LDO_VGP6, VOL_3300, "LCM");
    MDELAY(20);
	hwPowerOn(MT65XX_POWER_LDO_VAST,VOL_1200, "LCM");
    MDELAY(20);
	
    mt_set_gpio_mode(GPIO_LCD_STBTYB, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_STBTYB, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_STBTYB, GPIO_OUT_ONE);
   // mt_set_gpio_out(GPIO_LCD_STBTYB, GPIO_OUT_ZERO);
    MDELAY(20);
    mt_set_gpio_mode(GPIO_LCD_RST, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_RST, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ONE);
    MDELAY(20);
    mt_set_gpio_mode(GPIO_DISP_PWR_EN, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_DISP_PWR_EN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_DISP_PWR_EN, GPIO_OUT_ONE);
    MDELAY(35);

    mt_set_gpio_mode(GPIO_LCD_RST, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_RST, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ZERO);
    MDELAY(2);
    mt_set_gpio_mode(GPIO_LCD_RST, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_RST, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ONE);
    MDELAY(25);

	mt_set_gpio_mode(GPIO_LCD_SHUTDOWN, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_SHUTDOWN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_SHUTDOWN, GPIO_OUT_ONE);//chailu for mhl
    MDELAY(3);

#endif    
}


LCM_DRIVER wsvganl_lvds_jb_lcm_drv = 
{
    .name		    = "wsvganl_lvds_jb",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
};

