#ifndef SCHEDULEDROOM_H
#define SCHEDULEDROOM_H


#include <stdexcept>

#include <vector>
#include <iostream>

/**
 * @brief The ScheduledRoom class
 */
class ScheduledRoom {

public:
    // Constructors
    /**
     * @brief ScheduledRoom
     */
    inline ScheduledRoom();
    /**
     * @brief ScheduledRoom
     * @param _id
     * @param _numPeriods
     */
//    inline ScheduledRoom(int _id, int _numPeriods);

//    bool operator=(const ScheduledRoom &r) {
//        std::cout << "operator=" << std::endl;
//        std::cin.get();
//    }

    /**
     * @brief getId
     * @return
     */
    inline int getId() const;

    /**
     * @brief setId
     * @param _id
     */
    inline void setId(int _id);

    /**
     * @brief getNumExamsScheduled
     * @param _period
     * @return
     */
    inline int getNumExamsScheduled(int _period) const;
    /**
     * @brief setNumExamsScheduled
     * @param _period
     * @param _value
     */
    inline void setNumExamsScheduled(int _period, int _value);
    /**
     * @brief getNumOccupiedSeats
     * @param _period
     * @return
     */
    inline int getNumOccupiedSeats(int _period) const;
    /**
     * @brief setNumOccupiedSeats
     * @param _period
     * @param _value
     */
    inline void setNumOccupiedSeats(int _period, int _value);

    /**
     * @brief setNumPeriods
     * @param _numPeriods
     */
    inline void setNumPeriods(int _numPeriods);


private:
    // Private fields
    /**
     * @brief id Room id
     */
    int id;

    /**
     * @brief The PeriodRoom class Register room info for each period
     */
    struct PeriodRoom {
        /**
         * @brief numExamsScheduled Number of exams scheduled in this room
         */
        int numExamsScheduled;
        /**
         * @brief numOccupiedSeats Number of occupied seats
         */
        int numOccupiedSeats;
    };
    /**
     * @brief periods Periods vector for this room
     */
    std::vector<PeriodRoom> periods;
};


// Constructors
/**
 * @brief ScheduledExam
 */
ScheduledRoom::ScheduledRoom()
    : id(-1), periods(0)
{ }


/**
 * @brief getId
 * @return
 */
int ScheduledRoom::getId() const {
    return id;
}

/**
 * @brief setId
 * @param _id
 */
void ScheduledRoom::setId(int _id) {
    id = _id;
}


/**
 * @brief getNumExamsScheduled
 * @param _period
 * @return
 */
int ScheduledRoom::getNumExamsScheduled(int _period) const {
    return periods[_period].numExamsScheduled;
}

/**
 * @brief setNumExamsScheduled
 * @param _period
 * @param _value
 */
void ScheduledRoom::setNumExamsScheduled(int _period, int _value) {
    periods[_period].numExamsScheduled = _value;
}

/**
 * @brief getNumOccupiedSeats
 * @param _period
 * @return
 */
int ScheduledRoom::getNumOccupiedSeats(int _period) const {
    return periods[_period].numOccupiedSeats;
}

/**
 * @brief setNumOccupiedSeats
 * @param _period
 * @param _value
 */
void ScheduledRoom::setNumOccupiedSeats(int _period, int _value) {
    periods[_period].numOccupiedSeats = _value;
}

/**
 * @brief ScheduledRoom::setNumPeriods
 * @param _numPeriods
 */
void ScheduledRoom::setNumPeriods(int _numPeriods) {
    // Resize periods vector
    periods.resize(_numPeriods);
}


#endif // SCHEDULEDROOM_H








