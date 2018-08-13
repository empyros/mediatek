#ifndef __CUST_KEY_H__
#define __CUST_KEY_H__

#include<cust_kpd.h>

#define MT65XX_META_KEY		42	/* KEY_2 */
#define MT65XX_PMIC_RST_KEY	1000	/* Invalid Key */
#define MT_CAMERA_KEY 		10
#if defined(ACER_C11)||defined(PHILIPS_STAMFORD)
#define MT65XX_BOOT_MENU_KEY       2   /* KEY_VOLUMEUP */
#else
#define MT65XX_BOOT_MENU_KEY       1   /* KEY_VOLUMEUP */
#endif
#define MT65XX_MENU_SELECT_KEY     MT65XX_BOOT_MENU_KEY   
#if defined(SIMCOM_FOR1T)||defined(PHILIPS_W6500)
#define MT65XX_MENU_OK_KEY         2    /* KEY_VOLUMEDOWN */
#elif defined(PHILIPS_STAMFORD)
#define MT65XX_MENU_OK_KEY         1    /* KEY_VOLUMEDOWN */
#else
#define MT65XX_MENU_OK_KEY         0    /* KEY_VOLUMEDOWN */
#endif

#endif /* __CUST_KEY_H__ */
