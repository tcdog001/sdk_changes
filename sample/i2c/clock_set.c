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

#ifdef CONFIG_SUPPORT_CA_RELEASE
 #define HI_INFO_I2C(format, arg...)
#else
 #define HI_INFO_I2C(format, arg...) printf( format, ## arg)
#endif

void 
save_auto(HI_S8 *year_data, HI_S8 *month_data, HI_S8 *date_data, 
						HI_S8 *day, HI_S8 *hour_data, HI_S8 *minute_data, 
						HI_S8 *second_data)
{

	time_t timep = 0;    
	struct tm *p;

	time(&timep);
	p = localtime(&timep);

	p->tm_mon += 1;
	*second_data= (((p->tm_sec / 10) & 0x07) << 4) | (p->tm_sec - (p->tm_sec / 10) * 10); 

	*minute_data= (((p->tm_min / 10) & 0x07) << 4) | (p->tm_min - (p->tm_min / 10) * 10);  
	if ((p->tm_hour/10) >= 2) {        
		*hour_data = 0x20 | ((p->tm_hour - 20) & 0x0f);
	} else if ((p->tm_hour/10) >= 1) {        
		*hour_data = 0x10 | ((p->tm_hour - 10) & 0x0f);
	} else    {       
		*hour_data = 0x00 | ((p->tm_hour) & 0x0f);
	}    
	*hour_data = *hour_data & 0xBF;  
	*day = p->tm_wday;    
	*date_data = (((p->tm_mday / 10) & 0x03) << 4) | (p->tm_mday - (p->tm_mday / 10) * 10);   
	*month_data= (((p->tm_mon / 10) & 0x01) << 4) | (p->tm_mon - (p->tm_mon / 10) * 10);    
	*year_data= (1900 + p->tm_year) - 2000;    
	*year_data = (((*year_data / 10) & 0x0F) << 4) | (*year_data - (*year_data / 10) * 10);
}

void save_manual(HI_S8 *year_data, HI_S8 *month_data, HI_S8 *date_data, 
						HI_S8 *day, HI_S8 *hour_data, HI_S8 *minute_data, 
						HI_S8 *second_data)
{

	HI_S16	year = 0;
	HI_S8 	month = 0,date = 0, mode = 0, am_pm = 0,
			hour = 0,  minute = 0, second = 0;

	HI_S32 ret;

	while (1) {
		HI_INFO_I2C("Please input mode(12/24):");
		scanf("%hhd", &mode);
		if (12 != mode && 24 != mode) {
			HI_INFO_I2C("Wrong mode!\n");
			continue;
		}
		break;
	}
	
	while (1) {
		HI_INFO_I2C("Please input date(xxxx-xx-xx):");
		ret = scanf("%hd-%hhd-%hhd", &year, &month, &date);

		if (ret < 3 || (month < 0 || month > 12) || (date < 0 || date > 31)) {
			HI_INFO_I2C("Wrong format\n");
			continue ;
		}
		break;
	}

	while(1) {
		HI_INFO_I2C("Please input day(1-7):");
		scanf("%hhd", day);

		if (*day < 1 || *day > 7) {
			HI_INFO_I2C("Wrong format\n");
			continue;
		}
		break;
	}

	while(1) {
		HI_INFO_I2C("Please input time(xx:xx:xx):");
		ret = scanf("%hhd:%hhd:%hhd", &hour, &minute, &second);
		if (ret < 3 || (minute < 0 || minute > 59) || (second < 0 || second > 59)) {
			HI_INFO_I2C("Wrong format\n");
			continue;
		}
		if (24 == mode) {
			if ((hour < 0 || hour > 24) ) {
				HI_INFO_I2C("Wrong format\n");
				continue;
			}
		 } else { 
			if ((hour < 0 || hour > 12) ) {
				HI_INFO_I2C("Wrong format\n");
				continue;
			}
		 }
		 break;
	}
		
	if (12 == mode) {
		while (1) {
			HI_INFO_I2C("Choose 0(AM)/1(PM)):");
			scanf("%hhd", &am_pm);
			if (0 != am_pm && 1 != am_pm) {
				HI_INFO_I2C("Wrong format\n");
				continue;
			}
			break;
		}
	}

	*year_data = (year % 10) + ((year % 100) / 10 ) * 0x10; 
	*month_data = (month % 10) + (month / 10) * 0x10;
	*date_data = (date % 10) + (date / 10) * 0x10;
	*hour_data = (hour % 10) + (hour / 10) * 0x10;
	if (12 == mode)
		*hour_data = *hour_data | (1 << 6) | (am_pm << 5);
	*minute_data = (minute % 10) + (minute / 10) * 0x10;
	*second_data = (second % 10) + (second / 10) * 0x10;
}

void 
read_rtc_and_config_date(HI_U8 *pData)
{   

	HI_U8 sec_data = 0, sec = 0, min_data = 0, min = 0, hour_data = 0, hour = 0;
	HI_U8 day = 0, date_data = 0, date = 0, month_data = 0, month = 0, year_data = 0, year = 0;
	char Cmd[128] = {0};
	char fen = '"';
	
	sec_data = pData[0];
	min_data = pData[1];
	hour_data = pData[2];
	day = pData[3];
	date_data = pData[4];
	month_data = pData[5];
	year_data = pData[6];

	sec = ((sec_data >> 4) & 0x7) * 10 + (sec_data & 0xf);
	min = ((min_data >> 4) & 0x7) * 10 + (min_data & 0xf);
	hour = ((hour_data >> 4) & 0x03) * 10 + (hour_data & 0xf); 
	date = ((date_data >> 4) & 0x3) * 10 + (date_data & 0xf);
	month = ((month_data >> 4) & 0x01) * 10 + (month_data & 0xf);
	year = ((year_data >> 4) & 0xf) * 10 + (year_data & 0xf);

	memset(Cmd, 0, 128);
    sprintf(Cmd, "date %c%04d-%02d-%02d %02d:%02d:%02d%c", fen, year+2000, month, date,
        	hour, min, sec, fen);

	system(Cmd);
}

HI_S32 main(HI_S32 argc, HI_CHAR **argv)
{
    HI_S32 s32Ret = HI_FAILURE;
    HI_U8 *pData = HI_NULL;
    HI_U32 u32DeviceAddress = 0xd0;
    HI_U32 u32I2cNum  = 0;
    HI_U32 u32RegAddr = 0x0;
    HI_U32 u32RegAddrCount = 1;
    HI_U32 u32WriteNumber = 7;
	HI_S8 	year_data = 0, month_data = 0, date_data = 0, 
			day = 0, hour_data = 0, minute_data = 0, second_data = 0;

	s32Ret = HI_UNF_I2C_Init();
    if (HI_SUCCESS != s32Ret)
    {
        HI_INFO_I2C("%s: %d ErrorCode=0x%x\n", __FILE__, __LINE__, s32Ret);
        return s32Ret;
    }

    pData = (HI_U8 *)malloc(u32WriteNumber);
    if (HI_NULL == pData)
    {
        HI_INFO_I2C("\n malloc() error!\n");
        HI_UNF_I2C_DeInit();

        return HI_FAILURE;
    }
	
	if (2 == argc) {
		if (0 == strncmp(argv[1], "auto", strlen("auto"))) {
			save_auto(&year_data, &month_data, &date_data, &day, &hour_data, &minute_data, &second_data);
		} else if (0 == strncmp(argv[1], "manual", strlen("manual"))) {
			save_manual(&year_data, &month_data, &date_data, &day, &hour_data, &minute_data, &second_data);
		}
		pData[0] = second_data;
		pData[1] = minute_data;
		pData[2] = hour_data;
		pData[3] = day;
		pData[4] = date_data;
		pData[5] = month_data;
		pData[6] = year_data;


	    s32Ret = HI_UNF_I2C_Write(u32I2cNum, u32DeviceAddress,
	                             u32RegAddr, u32RegAddrCount, pData, u32WriteNumber);
	    if (s32Ret != HI_SUCCESS)
	    {
	        HI_INFO_I2C("i2c write failed!\n");
	    }
	    else
	    {
	        HI_INFO_I2C("i2c write success!\n");
	    }
 
	}else {
		s32Ret = HI_UNF_I2C_Read(u32I2cNum, u32DeviceAddress,
                             	 u32RegAddr, u32RegAddrCount, pData, u32WriteNumber);
		read_rtc_and_config_date(pData);
	}

	free(pData);
	HI_UNF_I2C_DeInit();
	
    return s32Ret;
}
