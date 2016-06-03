#ifndef ITC2007PERIOD_H
#define ITC2007PERIOD_H



#include "utils/DateTime.h"


class ITC2007Period {
    int id;   // Zero based period index
    Date date;    // Period date
    Time time;    // Period time in 24 hour format
    int duration; // Period duration in minutes
    int penalty;  // Associated integer penalty. Zero means no penalty is incurred

public:
    // Constructor
    ITC2007Period(int _id, Date const& _date, Time const& _time, int _duration, int _penalty)
        : id(_id), date(_date), time(_time), duration(_duration), penalty(_penalty) { }

    int getId() const           { return id;       }
    Date const& getDate() const { return date;     }
    Time const& getTime() const { return time;     }
    int getDuration() const     { return duration; }
    int getPenalty() const      { return penalty;  }
};


#endif // ITC2007PERIOD_H
