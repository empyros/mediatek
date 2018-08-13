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

//#define LSA0_GPIO_PIN (GPIO_DISP_LSA0_PIN)
#define LSCE_GPIO_PIN (GPIO_DISP_LSCE_PIN)
#define LSCK_GPIO_PIN (GPIO_DISP_LSCK_PIN)
#define LSDA_GPIO_PIN (GPIO_DISP_LSDA_PIN)
#define LSCE1_GPIO_PIN (GPIO_DISP_LSCE1_PIN)
//#ifdef GPIO_DISP_LSDO_PIN
//#define LSDO_GPIO_PIN (GPIO_DISP_LSDA_PIN)
//#endif

#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (800)

#define LCD_ID 0x87
// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))
#define SET_GPIO_OUT(n, v)  (lcm_util.set_gpio_out((n), (v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

#define SET_GPIO_INPUT(n)  (lcm_util.set_gpio_dir((n), (0)))
#define SET_GPIO_OUTPUT(n)  (lcm_util.set_gpio_dir((n), (1)))

//extern bool slid__lcm_flag;
bool slid__lcm_flag=false;
int mhall_time_lcm=0;

static unsigned int lcm_bl_setlevel1=0;//main bl set to bl ic level
static unsigned int lcm_bl_setlevel2=0;//sub bl set to bl ic level
static unsigned int lcm_bl_needlevel1=0;//UI brightness set main bl level
static unsigned int lcm_bl_needlevel2=0;//UI brightness set sub bl level
static unsigned int level_now=0; //current level for switch mhall

bool lcm_suspend_sub_flag=false;
bool lcm_suspend_main_flag=false;
// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define SET_LSCE_LOW   SET_GPIO_OUT(LSCE_GPIO_PIN, 0)
#define SET_LSCE_HIGH  SET_GPIO_OUT(LSCE_GPIO_PIN, 1)
#define SET_LSCK_LOW   SET_GPIO_OUT(LSCK_GPIO_PIN, 0)
#define SET_LSCK_HIGH  SET_GPIO_OUT(LSCK_GPIO_PIN, 1)
#define SET_LSDA_LOW   SET_GPIO_OUT(LSDA_GPIO_PIN, 0)
#define SET_LSDA_HIGH  SET_GPIO_OUT(LSDA_GPIO_PIN, 1)
#define SET_LSCE1_LOW   SET_GPIO_OUT(LSCE1_GPIO_PIN, 0)
#define SET_LSCE1_HIGH  SET_GPIO_OUT(LSCE1_GPIO_PIN, 1)
//#ifdef GPIO_DISP_LSDO_PIN
#define GET_LSDO_DATA  mt_get_gpio_in(LSDO_GPIO_PIN)
//#endif

#define SET_LSDA_INPUT  SET_GPIO_INPUT(LSDA_GPIO_PIN)
#define SET_LSDA_OUTPUT  SET_GPIO_OUTPUT(LSDA_GPIO_PIN)


// sub lcm code--start----
static __inline void send_ctrl_cmd_sub(unsigned char cmd)
{
    unsigned char i;

    SET_LSCE1_HIGH;
    UDELAY(1);
    SET_LSCK_HIGH;
	UDELAY(1);
    SET_LSDA_HIGH;
    UDELAY(1);
	
    SET_LSCE1_LOW;
    UDELAY(1);

	SET_LSCK_LOW;
	UDELAY(1);
    SET_LSDA_LOW;//A0=0
    UDELAY(1);
    SET_LSCK_HIGH;
    UDELAY(1);

    for (i = 0; i < 8; ++ i)
    {
        SET_LSCK_LOW;
        if (cmd & (1 << 7)) {
            SET_LSDA_HIGH;
        } else {
            SET_LSDA_LOW;
        }
        UDELAY(1);
        SET_LSCK_HIGH;
        UDELAY(1);
        cmd <<= 1;
    }

    SET_LSDA_HIGH;
    SET_LSCE1_HIGH;
}

static __inline void send_data_cmd_sub(unsigned char data)
{
    unsigned char i;

    SET_LSCE1_HIGH;
    UDELAY(1);
    SET_LSCK_HIGH;
	UDELAY(1);
    SET_LSDA_HIGH;
    UDELAY(1);
	
    SET_LSCE1_LOW;
    UDELAY(1);

	SET_LSCK_LOW;
	UDELAY(1);
    SET_LSDA_HIGH;//A0=1
    UDELAY(1);
    SET_LSCK_HIGH;
    UDELAY(1);

    for (i = 0; i < 8; ++ i)
    {
        SET_LSCK_LOW;
        if (data & (1 << 7)) {
            SET_LSDA_HIGH;
        } else {
            SET_LSDA_LOW;
        }
        UDELAY(1);
        SET_LSCK_HIGH;
        UDELAY(1);
        data <<= 1;
    }

    SET_LSDA_HIGH;
    SET_LSCE1_HIGH;
}

static  __inline void backlight_ctrl_sub(unsigned char level)
{
    volatile unsigned char temp,temp1,i,j;

        #ifdef BUILD_LK
	printf("[LED] sub need level=%d\n", level);
        #else
	printk("[LED] sub need level=%d\n", level);
        #endif
        lcm_bl_needlevel2=level;
        level=32-(level/8);

      #ifdef BUILD_LK
	printf("[LED] sub set level=%d\n", level);
        #else
	printk("[LED] sub set level=%d\n", level);
        #endif

    if(level==lcm_bl_setlevel2)
    {
	return;
    }
    else
    {
	if(level<32)
	{    
	    mt_set_gpio_mode(GPIO_DISP_BL_EN2_PIN, GPIO_MODE_GPIO);
            mt_set_gpio_dir(GPIO_DISP_BL_EN2_PIN, GPIO_DIR_OUT);
            mt_set_gpio_out(GPIO_DISP_BL_EN2_PIN, GPIO_OUT_ZERO);
	    temp=level;
             
            temp1=lcm_bl_setlevel2;
	    
	    if(temp1<temp)
		temp=temp-temp1;
	    else
		temp=32+temp-temp1;
               for(j=0;j<30;j++); 

	    for(j=0;j<30;j++);
	    mt_set_gpio_out(GPIO_DISP_BL_EN2_PIN, GPIO_OUT_ONE);
	    UDELAY(35); //Ton>30us
	    for(i=1;i<temp;i++)
	    {
		mt_set_gpio_out(GPIO_DISP_BL_EN2_PIN, GPIO_OUT_ZERO);
		for(j=0;j<30;j++); //0.5us~500us	
		mt_set_gpio_out(GPIO_DISP_BL_EN2_PIN, GPIO_OUT_ONE);
		for(j=0;j<30;j++); //>0.5us			
	    }

	    mt_set_gpio_out(GPIO_DISP_BL_EN2_PIN, GPIO_OUT_ONE);
	    for(j=0;j<5;j++); 
             
	 
	}
	else
	{
	    mt_set_gpio_mode(GPIO_DISP_BL_EN2_PIN, GPIO_MODE_GPIO);
            mt_set_gpio_dir(GPIO_DISP_BL_EN2_PIN, GPIO_DIR_OUT);
            mt_set_gpio_out(GPIO_DISP_BL_EN2_PIN, GPIO_OUT_ZERO);
	    MDELAY(5); //>3ms
	}
	
	lcm_bl_setlevel2=level;
    }


}

static void init_lcm_registers_sub(void)
{
	//Set_EXTC 
	send_ctrl_cmd_sub(0xB9);   
	send_data_cmd_sub(0xFF);	  
	send_data_cmd_sub(0x83);	  
	send_data_cmd_sub(0x63);	       	     	

//Set_POWER  
send_ctrl_cmd_sub(0xB1);  
send_data_cmd_sub(0x81);    
send_data_cmd_sub(0x24);//24   
send_data_cmd_sub(0x04);    
send_data_cmd_sub(0x20);//02    
send_data_cmd_sub(0x00);//02    
send_data_cmd_sub(0x00);//03    
send_data_cmd_sub(0x10);    
send_data_cmd_sub(0x10);    
send_data_cmd_sub(0x34);    
send_data_cmd_sub(0x3C);   
send_data_cmd_sub(0x3F);    
send_data_cmd_sub(0x3F); 
	  
	//Sleep Out
	send_ctrl_cmd_sub(0x11);
	MDELAY(150);

	send_ctrl_cmd_sub(0x20);

	send_ctrl_cmd_sub(0x36);   
	send_data_cmd_sub(0x0A);	

	//Set COLMOD 
	send_ctrl_cmd_sub(0x3A);   
	send_data_cmd_sub(0x60);
	
	//Set_RGBIF 
	send_ctrl_cmd_sub(0xB3);   
	send_data_cmd_sub(0x07);	       	     	

	//Set_CYC 
	send_ctrl_cmd_sub(0xB4);   
	send_data_cmd_sub(0x00);	       	     	
	send_data_cmd_sub(0x08);	       	     	
	send_data_cmd_sub(0x6E);	       	     	
	send_data_cmd_sub(0x07);	       	     	
	send_data_cmd_sub(0x01);	       	     	
	send_data_cmd_sub(0x01);	       	     	
	send_data_cmd_sub(0x62);	       	     	
	send_data_cmd_sub(0x01);	       	     	
	send_data_cmd_sub(0x57);	       	     	

	//Set_PANEL 
	send_ctrl_cmd_sub(0xCC);   
	send_data_cmd_sub(0x02);	       	     	
	MDELAY(5);

	//Set_VCOM 
	send_ctrl_cmd_sub(0xB6);   
	send_data_cmd_sub(0x50);

	//Set Gamma 2.2
	send_ctrl_cmd_sub(0xE0);   
	send_data_cmd_sub(0x01);	        
	send_data_cmd_sub(0x48);	        
	send_data_cmd_sub(0x4D);	        
	send_data_cmd_sub(0x4E);	        
	send_data_cmd_sub(0x58);	        
	send_data_cmd_sub(0xF6);	        
	send_data_cmd_sub(0x0B);	        
	send_data_cmd_sub(0x4E);	        
	send_data_cmd_sub(0x12);	        
	send_data_cmd_sub(0xD5);	        
	send_data_cmd_sub(0x15);	        
	send_data_cmd_sub(0x95);	        
	send_data_cmd_sub(0x55);	        
	send_data_cmd_sub(0x8E);	        
	send_data_cmd_sub(0x11);	        
	send_data_cmd_sub(0x01);	        
	send_data_cmd_sub(0x48);	        
	send_data_cmd_sub(0x4D);	        
	send_data_cmd_sub(0x55);	        
	send_data_cmd_sub(0x5F);	        
	send_data_cmd_sub(0xFD);	        
	send_data_cmd_sub(0x0A);	        
	send_data_cmd_sub(0x4E);	        
	send_data_cmd_sub(0x51);	        
	send_data_cmd_sub(0xD3);	        
	send_data_cmd_sub(0x17);	        
	send_data_cmd_sub(0x95);	        
	send_data_cmd_sub(0x96);	        
	send_data_cmd_sub(0x4E);	        
	send_data_cmd_sub(0x11);	       	
	MDELAY(5);
	
	//Display On
	send_ctrl_cmd_sub(0x29);

	MDELAY(200);
	send_ctrl_cmd_sub(0x2C);
	MDELAY(20);



#ifdef BUILD_LK
	printf("init_lcm_registers_sub\r\n");
#else
	printk("init_lcm_registers_sub\r\n");
#endif
}
static void lcm_suspend_sub(void)
{
    send_ctrl_cmd_sub(0x28);
    MDELAY(10);
    send_ctrl_cmd_sub(0x10);
    MDELAY(120);
    lcm_suspend_sub_flag=true;
}


static void lcm_resume_sub(void)
{

    send_ctrl_cmd_sub(0x11);
    MDELAY(150);
    send_ctrl_cmd_sub(0x29);
    send_ctrl_cmd_sub(0x2c);
    MDELAY(50);
    lcm_suspend_sub_flag=false;

}
// sub lcm code ---end----


//main lcm code ----start----


#if 1
static __inline void send_ctrl_cmd(unsigned char cmd)
{
    unsigned char i;

    /*SET_LSCE_LOW;
    UDELAY(1);
    
    SET_LSCK_LOW;
    UDELAY(1);
    SET_LSDA_LOW;//A0=0
    UDELAY(1);
    SET_LSCK_HIGH;
    UDELAY(1);

    for(i=0;i<8;i++)
    {
    SET_LSCK_LOW;
    if((0x80 & cmd)!=0) 
    SET_LSDA_HIGH;
    else 
    SET_LSDA_LOW;
    cmd <<= 1;
    UDELAY(1);
    SET_LSCK_HIGH;
    UDELAY(1);
    }
    SET_LSCE_HIGH;*/
     SET_LSCE_HIGH;
    UDELAY(1);
    SET_LSCK_HIGH;
	UDELAY(1);
    SET_LSDA_HIGH;
    UDELAY(1);
	
    SET_LSCE_LOW;
    UDELAY(1);

	SET_LSCK_LOW;
	UDELAY(1);
    SET_LSDA_LOW;//A0=0
    UDELAY(1);
    SET_LSCK_HIGH;
    UDELAY(1);

    for (i = 0; i < 8; ++ i)
    {
        SET_LSCK_LOW;
        if (cmd & (1 << 7)) {
            SET_LSDA_HIGH;
        } else {
            SET_LSDA_LOW;
        }
        UDELAY(1);
        SET_LSCK_HIGH;
        UDELAY(1);
        cmd <<= 1;
    }

    SET_LSDA_HIGH;
    SET_LSCE_HIGH;
}
#endif

#if 1
static __inline void send_data_cmd(unsigned char data)
{
    unsigned char i;

    /*SET_LSCE_LOW;
    UDELAY(1);

    SET_LSCK_LOW;
    UDELAY(1);
    SET_LSDA_HIGH;//A0=1
    UDELAY(1);
    SET_LSCK_HIGH;
    UDELAY(1);

  for(i=0;i<8;i++)
  {
    SET_LSCK_LOW;
    if((0x80 & data)!=0) 
    SET_LSDA_HIGH;
    else 
    SET_LSDA_LOW;
    UDELAY(1);
    data <<= 1;
    SET_LSCK_HIGH;
    UDELAY(1);
  }
  SET_LSCE_HIGH;*/
   SET_LSCE_HIGH;
    UDELAY(1);
    SET_LSCK_HIGH;
	UDELAY(1);
    SET_LSDA_HIGH;
    UDELAY(1);
	
    SET_LSCE_LOW;
    UDELAY(1);

	SET_LSCK_LOW;
	UDELAY(1);
    SET_LSDA_HIGH;//A0=1
    UDELAY(1);
    SET_LSCK_HIGH;
    UDELAY(1);

    for (i = 0; i < 8; ++ i)
    {
        SET_LSCK_LOW;
        if (data & (1 << 7)) {
            SET_LSDA_HIGH;
        } else {
            SET_LSDA_LOW;
        }
        UDELAY(1);
        SET_LSCK_HIGH;
        UDELAY(1);
        data <<= 1;
    }

    SET_LSDA_HIGH;
    SET_LSCE_HIGH;
}
#endif

//#ifdef GPIO_DISP_LSDO_PIN
#if 1
static unsigned char spi_get_data(unsigned char reg) 
{
    unsigned int i;
	int k;
	unsigned char reg_value=0;

    SET_LSCE_HIGH;
    UDELAY(1);
    SET_LSCK_HIGH;
	UDELAY(1);
    SET_LSDA_HIGH;
    UDELAY(1);
	
    SET_LSCE_LOW;
    UDELAY(1);

    SET_LSCK_LOW;
	UDELAY(1);
    SET_LSDA_LOW;//A0=0
    UDELAY(1);
    SET_LSCK_HIGH;
    UDELAY(1);
	
    for (i = 0; i < 8; ++ i)
    {
        SET_LSCK_LOW;
        if (reg & (1 << 7)) {
            SET_LSDA_HIGH;
        } else {
            SET_LSDA_LOW;
        }
        UDELAY(1);
        SET_LSCK_HIGH;
        UDELAY(1);
        reg <<= 1;
    }

	for(k = 7; k >= 0; k--)
	{
	   SET_LSCK_LOW;
       UDELAY(5);
	   
	  // if(GET_LSDO_DATA)
	   //{
	       //reg_value |= (1 << k);
	   //}
       SET_LSCK_HIGH;
	   UDELAY(5);
	}

    SET_LSCE_HIGH;
    SET_LSDA_HIGH;

	return reg_value;
}
#endif


static  __inline void backlight_ctrl_main(unsigned char level)
{
       #ifdef BUILD_LK
	printf("[mly][LED]backlight_ctrl_main:level:%d\r\n",level);
       #else
	printk("[mly][LED]backlight_ctrl_main:level:%d\r\n",level);
       #endif

    volatile unsigned char temp,temp1,i,j;

        #ifdef BUILD_LK
	printf("[LED] need level=%d\n", level);
        #else
	printk("[LED] need level=%d\n", level);
        #endif
        lcm_bl_needlevel1=level;
        level=32-(level/8);

      #ifdef BUILD_LK
	printf("[LED] set level=%d\n", level);
        #else
	printk("[LED] set level=%d\n", level);
        #endif

    if(level==lcm_bl_setlevel1)
    {
	return;
    }
    else
    {
	if(level<32)
	{    
	    mt_set_gpio_mode(GPIO_DISP_BL_EN_PIN, GPIO_MODE_GPIO);
            mt_set_gpio_dir(GPIO_DISP_BL_EN_PIN, GPIO_DIR_OUT);
            mt_set_gpio_out(GPIO_DISP_BL_EN_PIN, GPIO_OUT_ZERO);
	    temp=level;
             
            temp1=lcm_bl_setlevel1;
	    
	    if(temp1<temp)
		temp=temp-temp1;
	    else
		temp=32+temp-temp1;
               for(j=0;j<30;j++); 

	    for(j=0;j<30;j++);
	    mt_set_gpio_out(GPIO_DISP_BL_EN_PIN, GPIO_OUT_ONE);
	    UDELAY(35); //Ton>30us
	    for(i=1;i<temp;i++)
	    {
		mt_set_gpio_out(GPIO_DISP_BL_EN_PIN, GPIO_OUT_ZERO);
		for(j=0;j<30;j++); //0.5us~500us	
		mt_set_gpio_out(GPIO_DISP_BL_EN_PIN, GPIO_OUT_ONE);
		for(j=0;j<30;j++); //>0.5us			
	    }

	    mt_set_gpio_out(GPIO_DISP_BL_EN_PIN, GPIO_OUT_ONE);
	    for(j=0;j<5;j++); 
             
	 
	}
	else
	{
	    mt_set_gpio_mode(GPIO_DISP_BL_EN_PIN, GPIO_MODE_GPIO);
            mt_set_gpio_dir(GPIO_DISP_BL_EN_PIN, GPIO_DIR_OUT);
            mt_set_gpio_out(GPIO_DISP_BL_EN_PIN, GPIO_OUT_ZERO);
	    MDELAY(5); //>3ms
	}
	
	lcm_bl_setlevel1=level;
    }
 
}


static void init_lcm_registers_main(void)
{

	//Set_EXTC 
	send_ctrl_cmd(0xB9);   
	send_data_cmd(0xFF);	  
	send_data_cmd(0x83);	  
	send_data_cmd(0x63);	       	     	

//Set_POWER  
send_ctrl_cmd(0xB1);  
send_data_cmd(0x81);    
send_data_cmd(0x24);//24   
send_data_cmd(0x04);    
send_data_cmd(0x20);//02    
send_data_cmd(0x00);//02    
send_data_cmd(0x00);//03    
send_data_cmd(0x10);    
send_data_cmd(0x10);    
send_data_cmd(0x34);    
send_data_cmd(0x3C);   
send_data_cmd(0x3F);    
send_data_cmd(0x3F);	
  
	//Sleep Out
	send_ctrl_cmd(0x11);
	MDELAY(150);

	send_ctrl_cmd(0x20);

	send_ctrl_cmd(0x36);   
	send_data_cmd(0x0A);	

	//Set COLMOD 
	send_ctrl_cmd(0x3A);   
	send_data_cmd(0x60);
	
	//Set_RGBIF 
	send_ctrl_cmd(0xB3);   
	send_data_cmd(0x07);	       	     	

	//Set_CYC 
	send_ctrl_cmd(0xB4);   
	send_data_cmd(0x00);	       	     	
	send_data_cmd(0x08);	       	     	
	send_data_cmd(0x6E);	       	     	
	send_data_cmd(0x07);	       	     	
	send_data_cmd(0x01);	       	     	
	send_data_cmd(0x01);	       	     	
	send_data_cmd(0x62);	       	     	
	send_data_cmd(0x01);	       	     	
	send_data_cmd(0x57);	       	     	

	//Set_PANEL 
	send_ctrl_cmd(0xCC);   
	send_data_cmd(0x02);	       	     	
	MDELAY(5);

	//Set_VCOM 
	send_ctrl_cmd(0xB6);   
	send_data_cmd(0x50);

	//Set Gamma 2.2
	send_ctrl_cmd(0xE0);   
	send_data_cmd(0x01);	        
	send_data_cmd(0x48);	        
	send_data_cmd(0x4D);	        
	send_data_cmd(0x4E);	        
	send_data_cmd(0x58);	        
	send_data_cmd(0xF6);	        
	send_data_cmd(0x0B);	        
	send_data_cmd(0x4E);	        
	send_data_cmd(0x12);	        
	send_data_cmd(0xD5);	        
	send_data_cmd(0x15);	        
	send_data_cmd(0x95);	        
	send_data_cmd(0x55);	        
	send_data_cmd(0x8E);	        
	send_data_cmd(0x11);	        
	send_data_cmd(0x01);	        
	send_data_cmd(0x48);	        
	send_data_cmd(0x4D);	        
	send_data_cmd(0x55);	        
	send_data_cmd(0x5F);	        
	send_data_cmd(0xFD);	        
	send_data_cmd(0x0A);	        
	send_data_cmd(0x4E);	        
	send_data_cmd(0x51);	        
	send_data_cmd(0xD3);	        
	send_data_cmd(0x17);	        
	send_data_cmd(0x95);	        
	send_data_cmd(0x96);	        
	send_data_cmd(0x4E);	        
	send_data_cmd(0x11);	       	
	MDELAY(5);
	
	//Display On
	send_ctrl_cmd(0x29);

	MDELAY(200);
	send_ctrl_cmd(0x2C);
	MDELAY(20);



#ifdef BUILD_LK
	printf("init_lcm_registers_main\r\n");
#else
	printk("init_lcm_registers_main\r\n");
#endif

}
static void lcm_suspend_main(void)
{

    send_ctrl_cmd(0x28);
    MDELAY(10);
    send_ctrl_cmd(0x10);
    MDELAY(120);
    lcm_suspend_main_flag=true;
}


static void lcm_resume_main(void)
{
    send_ctrl_cmd(0x11);
    MDELAY(150);
    send_ctrl_cmd(0x29);
    send_ctrl_cmd(0x2c);
    MDELAY(50);
    lcm_suspend_main_flag=false;
}

//main lcm code ---end----

static void config_gpio(void)
{
    const unsigned int USED_GPIOS[] = 
    {
        LSCE_GPIO_PIN,
        LSCK_GPIO_PIN,
        LSDA_GPIO_PIN,
        LSCE1_GPIO_PIN
    };

    unsigned int i;

    //lcm_util.set_gpio_mode(LSA0_GPIO_PIN, GPIO_DISP_LSA0_PIN_M_GPIO);
    lcm_util.set_gpio_mode(LSCE_GPIO_PIN, GPIO_DISP_LSCE_PIN_M_GPIO);
    lcm_util.set_gpio_mode(LSCK_GPIO_PIN, GPIO_DISP_LSCK_PIN_M_GPIO);
    lcm_util.set_gpio_mode(LSDA_GPIO_PIN, GPIO_DISP_LSDA_PIN_M_GPIO);
    lcm_util.set_gpio_mode(LSCE1_GPIO_PIN, GPIO_DISP_LSCE1_PIN_M_GPIO);

    for (i = 0; i < ARY_SIZE(USED_GPIOS); ++ i)
    {
        lcm_util.set_gpio_dir(USED_GPIOS[i], 1);               // GPIO out
        lcm_util.set_gpio_pull_enable(USED_GPIOS[i], 0);
    }

	//#ifdef GPIO_DISP_LSDO_PIN
	//lcm_util.set_gpio_mode(LSDO_GPIO_PIN, GPIO_DISP_LSDA_PIN_M_GPIO);
    //lcm_util.set_gpio_dir(LSDO_GPIO_PIN, 0);               // GPIO out
    //lcm_util.set_gpio_pull_enable(LSDO_GPIO_PIN, 0);	
	//#endif

    // Swithc LSA0 pin to GPIO mode to avoid data contention,
    // since A0 is connected to LCM's SPI SDO pin
    //
    //lcm_util.set_gpio_dir(LSA0_GPIO_PIN, 0);                   // GPIO in
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
#if 1
    params->type   = LCM_TYPE_DPI;
    params->ctrl   = LCM_CTRL_GPIO;
    params->width  = FRAME_WIDTH;
    params->height = FRAME_HEIGHT;	


    params->dpi.mipi_pll_clk_ref = 0;
    params->dpi.mipi_pll_clk_div1 = 0x80000081; //lvds pll 208M
    params->dpi.mipi_pll_clk_div2 = 0x80100000;
    params->dpi.dpi_clk_div = 8; //{8,4}, pll/8=26M
    params->dpi.dpi_clk_duty = 4;

    params->dpi.clk_pol           = LCM_POLARITY_FALLING;
    params->dpi.de_pol            = LCM_POLARITY_RISING;//LCM_POLARITY_RISING;
    params->dpi.vsync_pol         = LCM_POLARITY_FALLING;//LCM_POLARITY_FALLING;
    params->dpi.hsync_pol         = LCM_POLARITY_FALLING;

    params->dpi.hsync_pulse_width = 10;
    params->dpi.hsync_back_porch  = 10;
    params->dpi.hsync_front_porch = 10;
    params->dpi.vsync_pulse_width = 4;
    params->dpi.vsync_back_porch  = 3;
    params->dpi.vsync_front_porch = 3;

    params->dpi.format            = LCM_DPI_FORMAT_RGB666;
    params->dpi.rgb_order         = LCM_COLOR_ORDER_RGB;
    params->dpi.is_serial_output  = 0;

    params->dpi.intermediat_buffer_num = 2;

    params->dpi.io_driving_current = LCM_DRIVING_CURRENT_6575_4MA;
#endif


}

//#ifdef GPIO_DISP_LSDO_PIN
static bool lcm_compare_id(void)
{
    unsigned int hx8363a_id=0;
	
    config_gpio();

	SET_RESET_PIN(1);
    MDELAY(50);
    SET_RESET_PIN(0);
    MDELAY(150);
    SET_RESET_PIN(1);
    MDELAY(150);

	send_ctrl_cmd(0xB9); //Set_EXTC
	send_data_cmd(0xFF);
	send_data_cmd(0x83);
	send_data_cmd(0x63);

	send_ctrl_cmd(0xfe); 
	send_data_cmd(0xF4);
	MDELAY(20);
	hx8363a_id=spi_get_data(0xFF);
	
#ifdef BUILD_LK
	printf("hx8363a_id=%x\r\n",hx8363a_id);
#else
	printk("hx8363a_id=%x\r\n",hx8363a_id);
#endif

	return (LCD_ID == hx8363a_id)?1:0;
}
//#endif

static void lcm_init(void)
{
    config_gpio();

	//SET_RESET_PIN(1);
    //MDELAY(50);
    SET_RESET_PIN(0);

    MDELAY(200);
    SET_RESET_PIN(1);
    MDELAY(200);

    init_lcm_registers_sub();
    //lcm_suspend_sub();
    init_lcm_registers_main();
    //lcm_suspend_main();

}


static void lcm_suspend(void)
{
//if(slid__lcm_flag)//close
lcm_suspend_sub();
//else //open
lcm_suspend_main();
}


static void lcm_resume(void)
{
    if(slid__lcm_flag)//close
      lcm_resume_sub();
    else //open
      lcm_resume_main();

#ifdef BUILD_LK
	printf("lcm_resume\r\n");
#else
	printk("lcm_resume\r\n");
#endif
}


static void lcm_setbacklight(unsigned int level)
{
//if (level>0)
{
#ifdef BUILD_LK
    if(level==255)
    {
       backlight_ctrl_sub(level);
       backlight_ctrl_main(level);
    }
    else
#endif
    {
       if(slid__lcm_flag)//close
        {
          //backlight_ctrl_main(0);
          //MDELAY(1);
          backlight_ctrl_sub(level);
        }
       else //open
        {  
          //backlight_ctrl_sub(0);
          //MDELAY(1); 
          backlight_ctrl_main(level);
        }
    }
}
/*
else if (level==0)
{
       backlight_ctrl_sub(0);
       backlight_ctrl_main(0);
}*/

/*
if(slid__lcm_flag)//close
backlight_ctrl_sub(level);
else //open
backlight_ctrl_main(level);*/

}

void lcm_backlight_switch(void)
{
#ifdef BUILD_LK
	printf("[mly][LED]lcm_backlight_switch:slid__lcm_flag=%d,level_now=%d\r\n",slid__lcm_flag,level_now);
#else
	printk("[mly][LED]lcm_backlight_switch:slid__lcm_flag=%d,level_now=%d\r\n",slid__lcm_flag,level_now);
#endif

if(lcm_suspend_main_flag&&lcm_suspend_sub_flag)
   return 0;

if(slid__lcm_flag)//close
{
  if ((mhall_time_lcm==0)||(mhall_time_lcm==1))
     level_now=255;
  else
     level_now=lcm_bl_needlevel1;
  backlight_ctrl_main(0);
  MDELAY(1);
  lcm_suspend_main();
  MDELAY(1);
  lcm_resume_sub();
  MDELAY(1);
  backlight_ctrl_sub(level_now);
}
else //open
{
  if ((mhall_time_lcm==0)||(mhall_time_lcm==1))
     level_now=255;
  else
     level_now=lcm_bl_needlevel2;
  backlight_ctrl_sub(0);
  MDELAY(1);
  lcm_suspend_sub();
  MDELAY(1);
  lcm_resume_main();
  MDELAY(1);
  backlight_ctrl_main(level_now);
}
}
// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER hx8363a_dpi_wvga_lcm_drv = 
{
    .name			= "hx8363a_dpi_wvga",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.set_backlight = lcm_setbacklight,
//#ifdef GPIO_DISP_LSDO_PIN
    .compare_id     = lcm_compare_id
//#endif
};
