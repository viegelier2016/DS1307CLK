# DS1307CLK
Arduino Library for the DS1307+ Real Time Clock (RTC).Makes NO use of Arduino's Time library.
Also contains functions to get Linux Epoch  derived from Date and Time on the DS1307.

Functions:

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

