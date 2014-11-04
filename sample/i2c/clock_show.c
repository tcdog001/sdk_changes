/*****************************************
* this sample base on hisi demo board test e2prom.
*user board notice:
* e2prom mode/ i2c device address/i2c use which channel
*
*****************************************/
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hi_unf_i2c.h"

//#include "hi_adp_boardcfg.h"

#ifdef CONFIG_SUPPORT_CA_RELEASE
 #define HI_INFO_I2C(format, arg...)
#else
 #define HI_INFO_I2C(format, arg...) printf( format, ## arg)
#endif

HI_S32 main(HI_S32 argc, HI_CHAR **argv)
{
    HI_S32 s32Ret = HI_FAILURE;

    HI_U8 *pData = HI_NULL;
	HI_U8 sec_data = 0, sec = 0, min_data = 0, min = 0, hour_data = 0, hour = 0;
	HI_U8 day = 0, date_data = 0, date = 0, month_data = 0, month = 0, year_data = 0, year = 0;
	HI_U8 mode, am_pm = 0;
	HI_U8 *week[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	HI_U32 u32DeviceAddress = 0xd0;
    HI_U32 u32I2cNum  = 0;
    HI_U32 u32RegAddr = 0x0;
    HI_U32 u32RegAddrCount = 1;
    HI_U32 u32ReadNumber = 7;

    s32Ret = HI_UNF_I2C_Init();
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_I2C("%s: %d ErrorCode=0x%x\n", __FILE__, __LINE__, s32Ret);
        return s32Ret;
    }

    pData = (HI_U8 *)malloc(u32ReadNumber);
    if (HI_NULL == pData)
    {
        HI_INFO_I2C("\n pReadData malloc() error!\n");
        HI_UNF_I2C_DeInit();

        return HI_FAILURE;
    }

    /* Read data from Device */
    s32Ret = HI_UNF_I2C_Read(u32I2cNum, u32DeviceAddress,
                             u32RegAddr, u32RegAddrCount, pData, u32ReadNumber);
    if (s32Ret != HI_SUCCESS)
    {
        HI_INFO_I2C("call HI_I2C_Read failed.\n");
        HI_INFO_I2C("error code: %x\n", s32Ret);
		free(pData);
        HI_UNF_I2C_DeInit();

        return s32Ret;
    }
	
	sec_data = pData[0];
	min_data = pData[1];
	hour_data = pData[2];
	day = pData[3];
	date_data = pData[4];
	month_data = pData[5];
	year_data = pData[6];


	sec = ((sec_data >> 4) & 0x7) * 10 + (sec_data & 0xf);
	min = ((min_data >> 4) & 0x7) * 10 + (min_data & 0xf);
	
	mode = (hour_data >> 6) & 0x1;
	
	if (mode) {//12-hour mode 
		hour = ((hour_data >> 4) & 0x01) * 10 + (hour_data & 0xf); 
		am_pm = (hour_data >> 5) & 0x1;
	}
	else {//24-hour mode
		hour = ((hour_data >> 4) & 0x03) * 10 + (hour_data & 0xf); 
	}
	date = ((date_data >> 4) & 0x3) * 10 + (date_data & 0xf);
	month = ((month_data >> 4) & 0x01) * 10 + (month_data & 0xf);
	year = ((year_data >> 4) & 0xf) * 10 + (year_data & 0xf);

	if (mode) 
		HI_INFO_I2C("%04d-%02d-%02d %s, %02d:%02d:%02d %s\n",
					2000+year, month, date, week[day%7], 
					((0 == am_pm) && (12 == hour) ? 0 : hour), 
					min, sec, 
					(am_pm == 1) ? "PM" : "AM");
	else 
		HI_INFO_I2C("%04d-%02d-%02d %s, %02d:%02d:%02d\n",
					2000+year, month, date, week[day%7], hour, min, sec);

    free(pData);

    HI_UNF_I2C_DeInit();

    return HI_SUCCESS;
}
