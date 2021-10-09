/*
 * ds1307CLK.ino
 * example code showing ds1307CLK-library with the DS1307+ Real Time Clock (RTC)
 *
 * This example assumes we are in the UTC +2 timezone.
 * In case you want to use a different time zone, just change the first parameter in the begin().
 *
 */


/*
ds1307CLK.ino

 * version  V1.0
 * date  2021-09
 * George Ellis, Swakopmund, Namibia
 */

#include <ds1307CLK.h>

ds1307CLK clkObject;


void setup()
{
    Serial.begin(115200);
    while(!Serial);

    clkObject.begin(+2); // always call begin() method for initialisation, the +2 assumes we are in timezone UTC+2
                         // in case you are in a different timezone, just change this parameter accordingly.

    /* if necessary, in case your DS1307 is not yet set at your local time, you can call setDateTime()
     setDateTime(byte second, byte minute, byte hour, byte weekDay, byte monthDay, byte month, byte year);
     where: second   0-59
            minute   0-59
            hour     0-23
            weekDay  1-7  Monday = 1 -- Sunday = 7
            monthDay 1-31
            month    1-12
            year     0-99

     For example: clkObject.setDateTime(0, 5, 15, 7, 19, 10, 25);
                  will set your DS1307 to: 15:05.0 on Sunday, 19th October 2025 */
}


int cntr = 0;



void loop()
{
    int second, minute, hour, weekDay, monthDay, month, year;
    unsigned long secondsPassed;
    char aMsg[168];

    // executes a while-loop during 2000 milliseconds
    clkObject.milliBreak(2000);

    if(cntr < 1)
    {
        ++cntr;

        // printDateTime() only works if you previously, in setup(), made a call to Seria.begin().
        // It prints the current date and time values to the serial-monitor
        clkObject.printDateTime();

        Serial.print("Seconds passed since 0:0.0 01-01-1970 UTC: ");
        Serial.println(clkObject.ds1307Now());

        // puts the current date and time values (from DS1307+) into passed variables
        // - Note: the year returned has a value between 0 and 99. So if year is 25 it actually is 2025
        clkObject.getDateTime(&second, &minute, &hour, &weekDay, &monthDay, &month, &year);
        sprintf(aMsg,"Current time is %d:%d.%d and the year is %d", hour, minute, second, year + 2000);
        Serial.println(aMsg);

        // idle during 200 mSec in a while-loop
        clkObject.milliBreak(200);

        // Returns seconds passed since 0:0.0 1 January 1970 as derived from given date and time values
        secondsPassed = clkObject.getEpoch(second, minute, hour, monthDay, month, year);
        Serial.print("At this time, the number of seconds passend since January 1 1970 at 0:0.0 is:");
        Serial.println(secondsPassed);

        // Seconds passed since 0:0.0 1 January 1970 converted into date and time values (in your timezone)
        //secondsPassed = 3665UL;
        clkObject.epoch2Date(secondsPassed, &monthDay, &month, &year, &hour, &minute, &second);
        sprintf(aMsg,"%lu seconds after 0:0.0 1 January 1970 the time and date in your timezone is %d:%d.%d  %d-%d-%d",
                   secondsPassed, hour, minute, second, monthDay, month, year);
        Serial.println(aMsg);
    }
}
