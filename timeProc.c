#include "timeProc.h"
#include "common.h"
#include <string.h>
#include <stdlib.h>

#ifdef TEST_WORD == 1
void RtcReadTime(void)
{/*convert the BCD time read from RTC registers to decimal values
    Hour is in 12H mode without AM/PM
  */
    static unsigned int HourTmp;

    if(++Min > 59)
    {
        Min=0;
        if(++HourTmp > 11)
        {
            HourTmp=0;
        }
    }
    Hour = HourTmp;
    TimeSync = 0;
}
#else
void RtcReadTime(void)
{/*convert the BCD time read from RTC registers to decimal values
    Hour is in 12H mode without AM/PM
  */
    unsigned char Units;
    unsigned char Tens;

    Units=I2c[RTC_PTR].RxBuff[0]&0b00001111;
    Tens=((I2c[RTC_PTR].RxBuff[0]&0b01110000)>>4)*10;
    Sec=Units + Tens;

    Units=I2c[RTC_PTR].RxBuff[1]&0b00001111;
    Tens=((I2c[RTC_PTR].RxBuff[1]&0b01110000)>>4)*10;
    Min=Units + Tens;

    Units=I2c[RTC_PTR].RxBuff[2]&0b00001111;
    Tens=((I2c[RTC_PTR].RxBuff[2]&0b00010000)>>4)*10;
    Hour=Units + Tens;
}
#endif



void RtcWriteTime(void)
{
    unsigned char Units;
    unsigned char Tens;

    unsigned int BcdSec;
    unsigned int BcdMin;
    unsigned int BcdHour;

    Tens = NtpSec / 10;
    Units = NtpSec - (Tens * 10);
    BcdSec = (Tens << 4) | Units;


    Tens = NtpMin / 10;
    Units = NtpMin - (Tens * 10);
    BcdMin = (Tens << 4) | Units;

    Tens = NtpHour / 10;
    Units = NtpHour - (Tens * 10);
    BcdHour = (Tens<<4) | Units;

    if(I2cBuffChk(RTC_PTR))
    {// if previous operations are over, start a new one
        I2cData(RTC_PTR, 4, 0, BcdSec, BcdMin, BcdHour, 0);
    }
}

char TimeDecode(void)
{// decode the current time from http://www.inrim.it
 //Example: *OPEN*15 OCT 2012 22:44:39 CEST\n*CLOS*

    int TimePoint;

    TimePoint = strcspn(RXbuff, ":" );

    NtpHour = atoi(RXbuff+TimePoint-2);
    NtpMin  = atoi(RXbuff+TimePoint+1);
    NtpSec  = atoi(RXbuff+TimePoint+4);
    return '0';
}

