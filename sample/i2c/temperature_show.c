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
#define SHIFT(_a, _b) (1.0 * (((_a) >> (_b)) & 0x01))

	HI_S32 s32Ret = HI_FAILURE;

    HI_U8 *pData = HI_NULL;

    HI_U32 u32DeviceAddress = 0x90;
    HI_U32 u32I2cNum  = 0;
    HI_U32 u32RegAddr = 0;
    HI_U32 u32RegAddrCount = 1;
    HI_U32 u32ReadNumber = 2;

	HI_S16 data;
	float temp;

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

	data = pData[0];
	data = (data << 8) + pData[1];
	temp =  SHIFT(data, 4) / 16 +
			SHIFT(data, 5) / 8 +
			SHIFT(data, 6) / 4 +
			SHIFT(data, 7) / 2 +
			SHIFT(data, 8) + 
			SHIFT(data, 9) * 2 +
			SHIFT(data, 10) * 4 +
			SHIFT(data, 11) * 8 +
			SHIFT(data, 12) * 16 +
			SHIFT(data, 13) * 32 +
			SHIFT(data, 14) * 64;
	
	if (data & 0x8000) 
		temp = temp - 128;
	
	HI_INFO_I2C("\nBoard temperature: %3.4f\n\n", temp);

    free(pData);

    HI_UNF_I2C_DeInit();
	
    return HI_SUCCESS;

#undef SHIFT
}
