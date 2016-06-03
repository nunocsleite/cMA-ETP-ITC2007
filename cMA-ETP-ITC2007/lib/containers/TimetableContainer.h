#ifndef TIMETABLECONTAINER_H
#define TIMETABLECONTAINER_H


#include <boost/unordered_set.hpp>
#include "data/ScheduledExam.h"
#include "data/ScheduledRoom.h"
#include "data/TimetableProblemData.hpp"
#include <tuple>

// Exam-Room tuple definition
//typedef std::tuple<int, int> ExamRoomTuple;


// Forward declaration
class TimetableProblemData;


/**
 * @brief The TimetableContainer class Abstract timetable container class
 */
class TimetableContainer {


public:

    // Exam-Room tuple definition
    typedef std::tuple<int, int> ExamRoomTuple;

    /**
     * @brief getNumPeriods Get # periods
     * @return
     */
    virtual int getNumPeriods() const = 0;
    /**
     * @brief getNumRooms Get # rooms
     * @return
     */
    virtual int getNumRooms() const = 0;
    /**
     * @brief getPeriodExams Return _ti period exams vector
     * @param _ti
     * @return
     */
    virtual const std::vector<ExamRoomTuple> &getPeriodExams(int _ti) const = 0;

    /**
     * @brief getPeriodExams Return _ti period exams vector
     * @param _ti
     * @return
     */
    virtual const std::vector<int> &getCompletePeriod(int _ti) const = 0;

    /**
     * @brief getPeriodSize Return period size
     * @param _period
     * @return
     */
    virtual int getPeriodSize(int _ti) const = 0;
    /**
     * @brief getScheduledExamsVector
     * @return The scheduled exams vector
     */
    virtual std::vector<ScheduledExam> const &getScheduledExamsVector() const = 0;
    /**
     * @brief getScheduledRoomsVector
     * @return
     */
    virtual const std::vector<ScheduledRoom> &getScheduledRoomsVector() const = 0;
    /**
     * @brief getRoom Return room where exam _ei is allocated in period _tj
     * @param _ei
     * @param _tj
     * @return
     */
    virtual int getRoom(int _ei, int _tj) const = 0;

    /**
     * @brief insertExam Insert exam _ei into period _tj and room _rk
     * @param _ei
     * @param _tj
     * @param _rk
     */
    virtual void insertExam(int _ei, int _tj, int _rk) = 0;
    /**
     * @brief isExamScheduled
     * @param _ei
     * @param _tj
     * @return true if exam _ei is scheduled in time slot _tj
     */
    virtual bool isExamScheduled(int _ei, int _tj) const = 0;
    /**
     * @brief removeAllPeriodExams Remove all period exams
     * @param _ti
     */
    virtual void removeAllPeriodExams(int _ti) = 0;
    /**
     * @brief removeExam Remove exam _ei from period _tj
     * @param _ei
     * @param _tj
     */
    virtual void removeExam(int _ei, int _tj) = 0;
    /**
     * @brief replacePeriod Copy period _tj to period _ti
     * @param _ti
     * @param _tj
     */
    virtual void replacePeriod(int _ti, int _tj) = 0;

    /**
     * @brief replacePeriod Copy external _periodExams vector, with _size exams, to period _ti
     * @param _ti
     * @param _periodExams
     * @param _size
     */
    virtual void replacePeriod(int _ti, const std::vector<int> &_periodExams, int _size) = 0;
    /**
     * @brief scheduleExam
     * @param _ei
     * @param _tj
     * @param _rk
     */
    virtual void scheduleExam(int _ei, int _tj, int _rk) = 0;

    /**
     * @brief addExamToRoom
     * @param _ei
     * @param _tj
     * @param _rk
     */
    virtual void addExamToRoom(int _ei, int _tj, int _rk) = 0;

    /**
     * @brief setTimetableProblemData
     * @param _value
     */
    virtual void setTimetableProblemData(const TimetableProblemData *_value) = 0;

    /**
     * @brief unscheduleExam
     * @param _ei
     * @param _tj
     */
    virtual void unscheduleExam(int _ei, int _tj) = 0;

    /**
     * @brief removeExamFromRoom
     * @param _ei
     * @param _tj
     * @param _rk
     */
    virtual void removeExamFromRoom(int _ei, int _tj, int _rk)  = 0;

};

#endif // TIMETABLECONTAINER_H








