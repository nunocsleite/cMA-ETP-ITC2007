#ifndef TIMETABLECONTAINERMATRIX_H
#define TIMETABLECONTAINERMATRIX_H

#include "containers/TimetableContainer.h"
#include <boost/shared_ptr.hpp>
#include <boost/unordered_set.hpp>
#include "containers/ColumnMatrix.h"
#include "containers/Matrix.h"
#include <vector>
#include "data/ScheduledExam.h"
#include "data/ScheduledRoom.h"
#include "data/TimetableProblemData.hpp"





//#define SCHED_EXAM  1
#define REMOVE_EXAM -1


//#define TIMETABLECONTAINERMATRIX_DEBUG

//#define TIMETABLECONTAINERMATRIX_CTOR


/**
 * @brief The TimetableContainerMatrix class
 */
class TimetableContainerMatrix : public TimetableContainer {
public:

    // Exam-Room tuple definition
    typedef std::tuple<int, int> ExamRoomTuple;

    /**
     * @brief TimetableContainerMatrix Default ctor
     */
    inline TimetableContainerMatrix();

    /**
     * @brief TimetableContainerMatrix::TimetableContainerMatrix Ctor which receives the #exams and #periods
     * @param _numExams
     * @param _numPeriods
     */
    inline TimetableContainerMatrix(int _numExams, int _numPeriods, int _numRooms,
                                    const TimetableProblemData *_timetableProblemData);

    /**
     * @brief getNumPeriods Get # periods
     * @return
     */
    inline virtual int getNumPeriods() const override;

    /**
     * @brief getNumRooms Get # rooms
     * @return
     */
    inline virtual int getNumRooms() const override;

    /**
     * @brief getPeriodSize Return period size
     *        Complexity: O(1)
     * @param _period
     * @return
     */
    inline virtual int getPeriodSize(int _ti) const override;

    /**
     * @brief getPeriodExams Return _ti period exams vector
     * @param _ti
     * @return
     */
    inline virtual const std::vector<ExamRoomTuple> &getPeriodExams(int _ti) const override;

    /**
     * @brief getCompletePeriod Return _ti period vector
     * @param _ti
     * @return
     */
    inline virtual const std::vector<int> &getCompletePeriod(int _ti) const override;


    /**
     * @brief getRoom Return room where exam _ei is allocated in period _tj
     * @param _ei
     * @param _tj
     * @return
     */
    inline virtual int getRoom(int _ei, int _tj) const override;

    /**
     * @brief getScheduledExamsVector
     * @return The scheduled exams vector
     */
    inline virtual std::vector<ScheduledExam> const &getScheduledExamsVector() const override;

    /**
     * @brief getScheduledRoomsVector
     * @return The scheduled rooms vector
     */
    inline virtual std::vector<ScheduledRoom> const &getScheduledRoomsVector() const override;

    /**
     * @brief insertExam Insert exam _ei into period _tj and room _rk
     *        Complexity: O(1)
     * @param _ei
     * @param _tj
     * @param _rk
     */
    inline virtual void insertExam(int _ei, int _tj, int _rk) override;

    /**
     * @brief isExamScheduled
     *        Complexity: O(1)
     * @param _ei
     * @param _tj
     * @return true if exam _ei is scheduled in time slot _tj
     */
    inline virtual bool isExamScheduled(int _ei, int _tj) const override;

    /**
     * @brief removeAllPeriodExams Remove all period exams
     *        Complexity: O(NumExams)
     * @param _ti
     */
    virtual void removeAllPeriodExams(int _ti) override;

    /**
     * @brief removeExam Remove exam _ei from period _tj
     *        Complexity: O(1)
     * @param _ei
     * @param _tj
     */
    inline virtual void removeExam(int _ei, int _tj) override;

    /**
     * @brief replacePeriod Copy period _tj to period _ti
     * @param _ti
     * @param _tj
     */
    virtual void replacePeriod(int _ti, int _tj) override;

    /**
     * @brief replacePeriod Copy external _periodExams vector, with _size exams, to period _ti
     * @param _ti
     * @param _periodExams
     * @param _size
     */
    virtual void replacePeriod(int _ti, const std::vector<int> &_completePeriod, int _size) override;

    /**
     * @brief scheduleExam
     * @param _ei
     * @param _tj
     * @param _rk
     */
    virtual void scheduleExam(int _ei, int _tj, int _rk) override;

    /**
     * @brief addExamToRoom
     * @param _ei
     * @param _tj
     * @param _rk
     */
    virtual void addExamToRoom(int _ei, int _tj, int _rk) override;

    /**
     * @brief setTimetableProblemData
     * @param _value
     */
    inline virtual void setTimetableProblemData(const TimetableProblemData *_value) override;

    /**
     * @brief unscheduleExam
     * @param _ei
     * @param _tj
     */
    virtual void unscheduleExam(int _ei, int _tj) override;

    /**
     * @brief removeExamFromRoom
     * @param _ei
     * @param _tj
     * @param _rk
     */
    virtual void removeExamFromRoom(int _ei, int _tj, int _rk) override;

protected:
    /**
     * @brief init
     */
    inline void init();

    /**
     * @brief timetableContainer The timetable container based on a matrix
     */
    ColumnMatrix<int> timetableContainer;

    /**
     * @brief periodExams ADDED 16 Jan 2016
     */
    std::vector<std::vector<ExamRoomTuple>> periodsExams;
    /**
     * @brief periodExams Vector containing a hash set for each period exams,
     * in order to access in O(1) (ideally)
     */
    std::vector<int> periodsSizes;
    /**
     * @brief numRooms # rooms
     */
    int numRooms;
    /**
     * @brief scheduledExamsVector
     */
    std::vector<ScheduledExam> scheduledExamsVector;
    /**
     * @brief scheduledRoomsVector
     */
    std::vector<ScheduledRoom> scheduledRoomsVector;
    /**
     * @brief timetableProblemData The problem data
     */
    TimetableProblemData const *timetableProblemData;
};



// Constructors

/**
 * @brief TimetableContainerMatrix::TimetableContainerMatrix
 */
TimetableContainerMatrix::TimetableContainerMatrix()
    : timetableContainer(),
      periodsExams(0),
      periodsSizes(0),
      numRooms(0),
      scheduledExamsVector(0),
      scheduledRoomsVector(0)
{
    // Set timetableProblemData static field
    timetableProblemData = nullptr;
    // Initialise container and aux vectors
//    init();
}


/**
 * @brief TimetableContainerMatrix::TimetableContainerMatrix
 * @param _numExams
 * @param _numPeriods
 * @param _numRooms
 */
TimetableContainerMatrix::TimetableContainerMatrix(int _numExams, int _numPeriods, int _numRooms,
                                                   TimetableProblemData const *_timetableProblemData)
    :
      // Initialise timetable matrix to have numExams x numCols size
      timetableContainer(_numExams, _numPeriods),
      // Initialise periodsExams to have numCols size of empty vectors (exams)
      periodsExams(_numPeriods),
      // Initialize the periods sizes vector to have numCols size and zero value
      periodsSizes(_numPeriods),
      // # rooms
      numRooms(_numRooms),
      // Scheduled exams vector
      scheduledExamsVector(_numExams),
      // Scheduled rooms vector
      scheduledRoomsVector(_numRooms),
      // Set timetableProblemData field
      timetableProblemData(_timetableProblemData)
{

#ifdef TIMETABLECONTAINERMATRIX_CTOR
    std::cout << "TimetableContainerMatrix ctor" << std::endl;
#endif
    // Initialise container and aux vectors
    init();
}

// Protected methods

/**
 * @brief TimetableContainerMatrix::init
 */
void TimetableContainerMatrix::init() {
//    std::cout << "# Rooms = " << numRooms << std::endl;

//    std::cin.get();

    for (int ei = 0; ei < timetableContainer.getNumLines(); ++ei) {
        for (int tj = 0; tj < timetableContainer.getNumCols(); ++tj) {
            timetableContainer.setVal(ei, tj, REMOVE_EXAM);
        }
        // Initialise the scheduled exams vector
        scheduledExamsVector[ei].setId(ei);
    }
    for (int rk = 0; rk < numRooms; ++rk) {
        // Initialise the scheduled rooms vector
        scheduledRoomsVector[rk].setId(rk);
        scheduledRoomsVector[rk].setNumPeriods(timetableContainer.getNumCols());
    }

//    for (int rk = 0; rk < numRooms; ++rk) {
//        std::cout << "Room " << scheduledRoomsVector[rk].getId() << ":" << std::endl;
//        for (int pi = 0; pi < timetableContainer.getNumCols(); ++pi) {
//            std::cout << scheduledRoomsVector[rk].getNumExamsScheduled(pi) << ":"
//                 << scheduledRoomsVector[rk].getNumOccupiedSeats(pi) << " ";
//        }
//    }
//    std::cin.get();
}


// API

/**
 * @brief getNumPeriods Get # periods
 * @return
 */
int TimetableContainerMatrix::getNumPeriods() const {
    return timetableContainer.getNumCols();
}

/**
 * @brief getNumRooms Get # rooms
 * @return
 */
int TimetableContainerMatrix::getNumRooms() const {
    return numRooms;
}

/**
 * @brief getPeriodSize Return the period size (# scheduled exams) of period _period
 * @param _period
 * @return
 */
int TimetableContainerMatrix::getPeriodSize(int _ti) const {
    return periodsSizes[_ti];
}

/**
 * @brief getPeriodExams Return _ti period exams vector
 * @param _ti
 * @return
 */
const std::vector<TimetableContainerMatrix::ExamRoomTuple> &TimetableContainerMatrix::getPeriodExams(int _ti) const {
    // Return packed exams vector for period _ti
    return periodsExams[_ti];
}


/**
 * @brief getCompletePeriod Return _ti period vector
 * @param _ti
 * @return
 */
const std::vector<int> &TimetableContainerMatrix::getCompletePeriod(int _ti) const {
    return timetableContainer.getColumn(_ti);
}



/**
 * @brief getRoom Return room where exam _ei is allocated in period _tj
 * @param _ei
 * @param _tj
 * @return
 */
int TimetableContainerMatrix::getRoom(int _ei, int _tj) const {
    return timetableContainer.getVal(_ei, _tj);
}

/**
 * @brief getScheduledExamsVector
 * @return The scheduled exams vector
 */
std::vector<ScheduledExam> const &TimetableContainerMatrix::getScheduledExamsVector() const {
    return scheduledExamsVector;
}

/**
 * @brief getScheduledRoomsVector
 * @return The scheduled rooms vector
 */
std::vector<ScheduledRoom> const &TimetableContainerMatrix::getScheduledRoomsVector() const {
    return scheduledRoomsVector;
}

/**
 * @brief insertExam Insert exam _ei into period _tj and room _rk
 * @param _ei
 * @param _tj
 * @param _rk
 */
void TimetableContainerMatrix::insertExam(int _ei, int _tj, int _rk) {
    // Insert exam _ei into period _tj
    timetableContainer.setVal(_ei, _tj, _rk);
    // Increment period size
    ++periodsSizes[_tj];
    // Insert (exam _ei, room _rk) tuple into period _tj in periodsExams
    periodsExams[_tj].push_back(std::make_tuple(_ei, _rk));
}

/**
 * @brief isExamScheduled
 * @param _ei
 * @param _tj
 * @return true if exam _ei is scheduled in time slot _tj
 */
bool TimetableContainerMatrix::isExamScheduled(int _ei, int _tj) const {
    return timetableContainer.getVal(_ei, _tj) != REMOVE_EXAM;
}

/**
 * @brief setTimetableProblemData
 * @param _value
 */
void TimetableContainerMatrix::setTimetableProblemData(const TimetableProblemData *_value) {
    timetableProblemData = _value;
    init();
//    std::cout << "CALLED TimetableContainerMatrix::setTimetableProblemData!!!" << std::endl;
//    std::cin.get();
}

/**
 * @brief removeExam Remove exam _ei from period _tj
 * @param _ei
 * @param _tj
 */
void TimetableContainerMatrix::removeExam(int _ei, int _tj) {
    // Remove exam _ei from period _tj
    timetableContainer.setVal(_ei, _tj, REMOVE_EXAM);
    // Decrement period size
    --periodsSizes[_tj];
    // Remove exam _ei from period _tj in periodsExams
    auto const &it = std::find_if(periodsExams[_tj].begin(), periodsExams[_tj].end(),
                                  [_ei](ExamRoomTuple const &_examRoomTuple) {
                                        return std::get<0>(_examRoomTuple) == _ei; });
#ifdef DEBUG_MODE
    if (it == periodsExams[_tj].end())
        throw std::runtime_error("TimetableContainerMatrix::removeExam: exam not found");
#endif
    // Remove exam
    periodsExams[_tj].erase(it);
}

#endif // TIMETABLECONTAINERMATRIX_H
















