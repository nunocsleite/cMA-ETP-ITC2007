#ifndef DATETIME_H
#define DATETIME_H


class Date {
    int day;
    int month;
    int year;

public:
    // Constructor
    Date(int _day, int _month, int _year)
        : day(_day), month(_month), year(_year) { }

    int getDay() const       { return day;    }
    void setDay(int value)   { day = value;   }
    int getMonth() const     { return month;  }
    void setMonth(int value) { month = value; }
    int getYear() const      { return year;   }
    void setYear(int value)  { year = value;  }
};


class Time {
    int hour;
    int minutes;
    int seconds;

public:
    // Constructor
    Time(int _hour, int _minutes, int _seconds)
        : hour(_hour), minutes(_minutes), seconds(_seconds) { }

    int getHour() const        { return hour;     }
    void setHour(int value)    { hour = value;    }
    int getMinutes() const     { return minutes;  }
    void setMinutes(int value) { minutes = value; }
    int getSeconds() const     { return seconds;  }
    void setSeconds(int value) { seconds = value; }
};


#endif // DATETIME_H
