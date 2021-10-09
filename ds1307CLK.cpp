/*
ds1307CLK.cpp

 * version  V1.0
 * date  2021-09
 * George Ellis, Swakopmund, Namibia
 */

#include "ds1307CLK.h"

ds1307CLK::ds1307CLK()
{
    this->isInitialized = 0;
}


ds1307CLK::~ds1307CLK()
{
}


void ds1307CLK::begin(int tmeZne)
{
    if(tmeZne > 14 || tmeZne < -12)
    {
        if(Serial)
            Serial.println("Invalid timezone");
        return;
    }
    else
    {
        Wire.begin();
        this->THIS_TIME_ZONE = tmeZne * SECONDS_IN_HOUR;
        this->isInitialized = 1;
    }
}




int ds1307CLK::daysInMonth(int monthNo, int yr)
{
    int days = 30;

    if(monthNo == 1 ||
       monthNo == 3 ||
       monthNo == 5 ||
       monthNo == 7 ||
       monthNo == 8 ||
       monthNo == 10 ||
       monthNo == 11)
        ++days;

    if(monthNo == 2)
    {
        if(isItLeapYear(yr))
            days = 29;
        else
            days = 28;
    }

    if(monthNo < 1 || monthNo > 12)
        days = 0;

    return(days);
}




int ds1307CLK::hasLeapSeconds(int thisYear)
{
    return(0);
    /* GNU gcc does not handle leapSeconds

    if(thisYear == 1972)
        return(2);
    if(thisYear < 1972 ||
       thisYear == 1980 || thisYear == 1984 || thisYear == 1986 || thisYear == 1988 || thisYear == 1991 ||
       thisYear == 1996 || thisYear == 1999 || thisYear == 2000 || thisYear == 2001 || thisYear == 2002 ||
       thisYear == 2003 || thisYear == 2004 || thisYear == 2006 || thisYear == 2007 || thisYear == 2009 ||
       thisYear == 2010 || thisYear == 2011 || thisYear == 2013 || thisYear == 2014 || thisYear == 2017 ||
       thisYear == 2018 || thisYear == 2019 || thisYear == 2020 || thisYear == 2021)
            return(0);
    return(1);
    */
}





int ds1307CLK::isItLeapYear(int yr)
{
	int leap1;
	int leap2;
	int leap3;
	int isLeapYear = 0;

	/*A leapyear is any year divisible by 4, unless the year is divisible by 100 but not 400.*/
	leap1 = yr % 4;
	leap2 = yr % 100;
	leap3 = yr % 400;
	if (leap3 == 0 || (leap2 != 0 && leap1 == 0))
		isLeapYear = 1;

	return(isLeapYear);
}






int ds1307CLK::epoch2Date(unsigned long thisEpoch, int *mday, int *mnth, int *yr, int *hr, int *min, int *sec)
{
    int yrs, monthNo, dayNo, hours, minutes, seconds;
    long daysInSeconds, daysAsDays, daysDiff, restDays, d;
    int leapYearsCount, i;
    unsigned long tmpEpoch, restSeconds;
    long preEpoch;


    if(this->THIS_TIME_ZONE < 0)
    {
        preEpoch = this->THIS_TIME_ZONE * -1;
        if (preEpoch < thisEpoch)
            preEpoch = 0;
    }
    else
        preEpoch = 0;

    if(preEpoch > 0)
    {
        preEpoch -= thisEpoch;

        *yr = 1969;
        *mday = 31;
        *mnth = 12;

        tmpEpoch = SECONDS_IN_DAY - preEpoch;
        restSeconds = tmpEpoch % SECONDS_IN_HOUR;
        *hr  = tmpEpoch / SECONDS_IN_HOUR;
        *sec = restSeconds % 60;
        *min =(restSeconds / 60);
    }
    else
    {
         // add or subtract time-zone difference (in seconds) in order to get GMT/UTC epoch
        thisEpoch += this->THIS_TIME_ZONE;

        yrs = thisEpoch / SECONDS_IN_YEAR;
        yrs += 1970;
        for(i = 1970, leapYearsCount = 0; i <= yrs; i++)
        {
            if(isItLeapYear(i))
                ++leapYearsCount;
        }
        daysInSeconds = thisEpoch % SECONDS_IN_YEAR;
        daysAsDays = daysInSeconds / SECONDS_IN_DAY;
        restSeconds = thisEpoch % SECONDS_IN_DAY;

        daysDiff = daysAsDays-(long)leapYearsCount;

        if(daysDiff < 0)
        {
            daysDiff *= -1;

            tmpEpoch = thisEpoch - (daysDiff * SECONDS_IN_DAY);
            yrs = tmpEpoch / SECONDS_IN_YEAR;
            yrs += 1970;
            for(i = 1970, leapYearsCount = 0; i <= yrs; i++)
            {
                if(isItLeapYear(i))
                    ++leapYearsCount;
            }

            daysInSeconds = tmpEpoch % SECONDS_IN_YEAR;
            daysAsDays = daysInSeconds / SECONDS_IN_DAY;
            daysAsDays -= leapYearsCount;

        }
        else
        {
            daysAsDays -= leapYearsCount;
        }

        restDays = daysAsDays;

        for(i = 1, monthNo = 1; i < 13; i++)
        {
            d = (long) daysInMonth(i, yrs);
            if(d < restDays)
            {
                restDays -= d;
                ++monthNo;
            }
            else
            {
                if(d == restDays)
                {
                    ++monthNo;
        break;
                }
                else
        break;
            }
        }

        hours = (int) (restSeconds / SECONDS_IN_HOUR);
        minutes = (int) ((restSeconds - (hours * SECONDS_IN_HOUR)) / 60);
        seconds = (int) (restSeconds - ( (hours * SECONDS_IN_HOUR) + (minutes * 60)));
        restDays += 1; // add 1 to restdays/monthDay because, although the day is not yet over, it counts as another day of the month

        if(isItLeapYear(yrs))
            ++restDays;

        *mday = (int) restDays;
        *mnth = monthNo;
        *yr   = yrs;
        *hr   = hours;
        *min  = minutes;
        *sec  = seconds;
    }
    return(0);
}



// getEpoch() returns the number of seconds since 1 January 1970 at time 0:0:0 as measured from
// GMT/UTC + 0
unsigned long ds1307CLK::getEpoch(int sec, int min, int hr, int mday, int mnth, int yr)
{
    int lpy = 0;
    int lps;
    int i, d;
    int thisYr;
    unsigned long secondsThisYear = 0, totEpoch;

    int tmp;

    if((sec > 59) || (sec < 0) || (min > 59) || (min < 0) || (hr > 23) || (hr < 0))
        return((unsigned long) 0);

    if((mnth <= 0) || (mnth > 12))
        return((unsigned long) 0);

    if(yr < 1970)
        yr += 2000;

    tmp = daysInMonth(mnth, yr);
    if(mday < 1 || mday > tmp)
        return((unsigned long) 0);

    if(mnth >= 3)
    {
        lpy = isItLeapYear(yr);
        secondsThisYear = SECONDS_IN_DAY;
    }

    lps = 0;
    //if(yr == 1972)   GNU gcc does not handle leapSeconds
    //    lps = 1;

    if(mnth >= 7)
        lps += hasLeapSeconds(yr); // for now always returns 0

    secondsThisYear += (unsigned long)lps;

    secondsThisYear = (unsigned long) sec;
    secondsThisYear += (unsigned long) ((unsigned long)min * (unsigned long)60);
    secondsThisYear += (unsigned long) ((unsigned long)hr * (unsigned long)3600);

    if(mday >= 2)
        secondsThisYear += (unsigned long) ((unsigned long)(mday-1) * SECONDS_IN_DAY);

    for(i = 1; i < mnth; i++) // < mnth because thisMonth has to be excluded
    {
        d = daysInMonth(i, yr);
        secondsThisYear += (unsigned long) ((unsigned long)d * SECONDS_IN_DAY);
    }
    // Above the numbers of seconds passed since the beginning of THIS year


    // Below the number of seconds passed from 1-1-1970 0:0.0

    for (totEpoch = 0, i = 1970; i < yr; i++)
    {
        totEpoch += SECONDS_IN_YEAR;

        if(isItLeapYear(i))
            totEpoch += SECONDS_IN_DAY;

        totEpoch += (unsigned long)hasLeapSeconds(i);
    }

    totEpoch += secondsThisYear;
    totEpoch -= this->THIS_TIME_ZONE;  // add or subtract time-zone difference (in seconds) in order to get GMT/UTC epoch


    return(totEpoch);
}




unsigned long ds1307CLK::ds1307Now()
{
    int sec, min, hr, weekDay, mDay, mnth, yr;
    unsigned long epoch;

    // Reset the register pointer
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.write(0);
    Wire.endTransmission();
    Wire.requestFrom(DS1307_ADDRESS, 7);

    sec = bcdToDec(Wire.read());
    min = bcdToDec(Wire.read());
    hr = bcdToDec(Wire.read() & 0b111111); //24 hour time
    weekDay = bcdToDec(Wire.read()); //0-6 -> sunday - Saturday  N O T   U S E D  H E R E
    mDay = bcdToDec(Wire.read());
    mnth = bcdToDec(Wire.read());
    yr = bcdToDec(Wire.read());
    yr += 2000;

    epoch = getEpoch(sec, min, hr, mDay, mnth, yr); // mday=day 1..31   mnth=months 1..12

    hr  = (int) 0;
    min = (int) 0;
    sec = (int) 0;
    this->epochAtDayBegin = getEpoch(sec, min, hr, mDay, mnth, yr);

    return(epoch);
}





void ds1307CLK::setDateTime(byte second, byte minute, byte hour,
                            byte weekDay, byte monthDay, byte month, byte year)
{
/*
  second   0-59
  minute   0-59
  hour     0-23
  weekDay  Monday = 1 -- Sunday = 7
  monthDay 1-31
  month    1-12
  year     0-99
*/

  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(0); //stop Oscillator

  Wire.write(decToBcd(second));
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));
  Wire.write(decToBcd(weekDay));
  Wire.write(decToBcd(monthDay));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));

  Wire.write(0); //start

  Wire.endTransmission();
}


void ds1307CLK::setDateTime(DATTIM dtInput)
{
/*
  dtInput.second   0-59
  .minute   0-59
  .hour     0-23
  .weekDay  Monday = 1 -- Sunday = 7
  .monthDay 1-31
  .month    1-12
  .year     0-99
*/

  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(0); //stop Oscillator

  Wire.write(decToBcd(dtInput.second));
  Wire.write(decToBcd(dtInput.minute));
  Wire.write(decToBcd(dtInput.hour));
  Wire.write(decToBcd(dtInput.weekDay));
  Wire.write(decToBcd(dtInput.monthDay));
  Wire.write(decToBcd(dtInput.month));
  Wire.write(decToBcd(dtInput.year));

  Wire.write(0); //start

  Wire.endTransmission();
}





void ds1307CLK::getDateTime(int *second, int *minute, int *hour, int *weekDay, int *monthDay, int *month, int *year)
{
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.write(0);
    Wire.endTransmission();

    Wire.requestFrom(DS1307_ADDRESS, 7);

    *second   = (int) bcdToDec(Wire.read());
    *minute   = (int) bcdToDec(Wire.read());
    *hour     = (int) bcdToDec(Wire.read() & 0b111111); //24 hour time
    *weekDay  = (int) bcdToDec(Wire.read()); //0-6 -> sunday - Saturday
    *monthDay = (int) bcdToDec(Wire.read());
    *month    = (int) bcdToDec(Wire.read());
    *year     = (int) bcdToDec(Wire.read());
}


void ds1307CLK::printDateTime()
{
    int second,  minute,  hour,  weekDay,  monthDay,  month,  year;

    if(Serial)
    {
        getDateTime(&second, &minute, &hour, &weekDay, &monthDay, &month, &year);
        // print the date and time like:  DD-MM-YYYY hh:mm:ss
        // Za 19 Do 2021  19:09:52

        Serial.print(this->days[weekDay-1]);
        Serial.print(" ");

        if (monthDay < 10)
          Serial.print("0");
        Serial.print(monthDay,DEC);
        Serial.print(" ");

        //if (month < 10)
        //  Serial.print("0");
        Serial.print(this->months[month-1]);

        Serial.print(" 20");
        Serial.print(year, DEC);

        Serial.print("  ");
        Serial.print(hour, DEC);
        Serial.print(":");
        if (minute < 10)
          Serial.print("0");
        Serial.print(minute, DEC);


        Serial.print(".");
        if (second < 10)
          Serial.print("0");
        Serial.println(second,DEC);
    }
}



byte ds1307CLK::decToBcd(byte val)
{
  return ( (val/10*16) + (val%10) );
}


byte ds1307CLK::bcdToDec(byte val)
{
  return ( (val/16*10) + (val%16) );
}



void ds1307CLK::milliBreak(unsigned long msec2Sleep)
{
    unsigned long nu, eind;

    if(! this->isInitialized)
    {
        if(Serial)
            Serial.println("ds1307::milliBreak() failed.  Call ds1307CLK::begin() method first");
        return;
    }

    nu = millis();
    eind = nu + msec2Sleep;

    while(millis() < eind);
}

