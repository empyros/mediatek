#ifndef TOUCHPANEL_H__
#define TOUCHPANEL_H__

//lenovo_sw liaohj merged from putaoya 2012-09-12

/* Pre-defined definition */
#define TPD_TYPE_CAPACITIVE
//#define TPD_TYPE_RESISTIVE
#define TPD_POWER_SOURCE         MT65XX_POWER_LDO_VGP4   
#define TPD_I2C_NUMBER           0
#define TPD_WAKEUP_TRIAL         60
#define TPD_WAKEUP_DELAY         100

#define TPD_VELOCITY_CUSTOM_X 15
#define TPD_VELOCITY_CUSTOM_Y 20


#define TPD_DELAY                (2*HZ/100)
#define TPD_CALIBRATION_MATRIX  {962,0,0,0,1600,0,0,0};

#define TPD_HAVE_CALIBRATION
#define TPD_HAVE_TREMBLE_ELIMINATION
#define TPD_NO_GPIO

/////////////////////////////////////////////////////////
#define TPD_X_RES 540
#define TPD_Y_RES 960
////////////////////////////////////////////////////////

#define TPD_WARP_Y(y) y 
#define TPD_WARP_X(x) x 

///////////////////////////////////////////////////////////


#define TPD_HAVE_BUTTON
#define TPD_KEY_COUNT 3
#define TPD_KEYS { KEY_BACK, KEY_HOMEPAGE, KEY_MENU}
#define TPD_KEYS_DIM {{120,1020,50,80},\
			{240,1020,50,80},\
			{400,1020,50,80}}

#endif /* TOUCHPANEL_H__ */

