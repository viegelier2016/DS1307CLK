/*
ds1307CLK.h

 * version  V1.0
 * date  2021-09
 * George Ellis, Swakopmund, Namibia
 */

#ifndef ds1307CLK_h
#define ds1307CLK_h

#include "Arduino.h"

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "Wire.h"

#define DS1307_ADDRESS 0x68


#define SECONDS_IN_HOUR (unsigned long) 3600
#define SECONDS_IN_DAY  (unsigned long) (SECONDS_IN_HOUR * (unsigned long) 24)
#define SECONDS_IN_YEAR (unsigned long) (SECONDS_IN_DAY * (unsigned long) 365)


// You might have a look at the file tmZn.txt for an overview of different time zones
// To make things slightly more complicated, some timezones have Daylight Saving Times
// (differences between summer and winter time)



typedef struct DATTIM
{
    byte second, minute, hour, weekDay, monthDay, month, year;
} DATTIM;



class ds1307CLK
{
public:

    ds1307CLK();
    ~ds1307CLK();

    // starts Wire (I2C) and sets the required time-zone (hours plus or minus UTC)
    void begin(int tmeZne);

     // executes a while-loop during msec2Sleep milliseconds
    void milliBreak(unsigned long msec2Sleep);

    // sets Date and Time on DS1307+
    void setDateTime(byte second, byte minute, byte hour, byte weekDay, byte monthDay, byte month, byte year);

    // same as above except that it uses date and time setting from passed struct DATTIM
    void setDateTime(DATTIM dtInput);

    // puts the current date and time values (from DS1307+) into passed variables
    void getDateTime(int *second, int *minute, int *hour, int *weekDay, int *monthDay, int *month, int *year);

    // in case calling Arduino script previously called Serial.begin() (thus Serial != NULL) printDateTime() prints
    // current date and time values to serial-monitor
    void printDateTime();


    // Seconds passed since 0:0.0 1 January 1970 (thisEpoch) converted into date and time values
    int epoch2Date(unsigned long thisEpoch, int *mday, int *mnth, int *yr, int *hr, int *min, int *sec);

    // Returns seconds passed since 0:0.0 1 January 1970 as derived from given date and time values
    unsigned long getEpoch(int sec, int min, int hr, int mday, int mnth, int yr);

    // Returns seconds passed since 0:0.0 1 January 1970, derived from present date and time values in DS1307+
    unsigned long ds1307Now();

    // the very first second of todays day-begin as seconds since 0:0.0 1 January 1970
    unsigned long epochAtDayBegin;


private:

    byte decToBcd(byte val);
    byte bcdToDec(byte val);
    int daysInMonth(int monthNo, int yr);
    int hasLeapSeconds(int thisYear);
    int isItLeapYear(int yr);

    long THIS_TIME_ZONE = 0; // not an unsigned long because it can be a negative value
    int isInitialized;
    char *days[7] = {"Mon","Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
    char *months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
};

#endif
