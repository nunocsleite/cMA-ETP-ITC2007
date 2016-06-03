

#include "containers/TimetableContainerMatrix.h"

using namespace  std;



//#define DEBUG_MODE




/**
 * @brief removeAllPeriodExams Remove all period exams
 * @param _ti
 */
void TimetableContainerMatrix::removeAllPeriodExams(int _ti) {
/// TODO - OPTIMIZED + UNSCHEDULE EXAM FROM ROOM
///
    // OPTIMIZED
//    for (int ei = 0; ei < timetableContainer.getNumLines(); ++ei) {
//        timetableContainer.setVal(ei, _ti, REMOVE_EXAM);
//    }
//    periodsSizes[_ti] = 0;
//    // Remove all exams from periodsExams in period _ti
//    periodsExams[_ti].clear();

    for (int ei = 0; ei < timetableContainer.getNumLines(); ++ei) {
        if (isExamScheduled(ei, _ti))
            unscheduleExam(ei, _ti);
    }

}



/**
 * @brief replacePeriod Copy period _tj to period _ti
 * @param _ti
 * @param _tj
 */
void TimetableContainerMatrix::replacePeriod(int _ti, int _tj) {
/// TODO - OPTIMIZED VERSION
///
///

//    auto const &colTj = timetableContainer.getColumn(_tj);
//    timetableContainer.setColumn(_ti, colTj);
//    periodsSizes[_ti] = periodsSizes[_tj];
//    // Replace periodExams
//    periodsExams[_ti] = periodsExams[_tj];


    removeAllPeriodExams(_ti);

    for (int ei = 0; ei < timetableContainer.getNumLines(); ++ei) {
        if (isExamScheduled(ei, _tj)) {
            int room = getRoom(ei, _ti);
            scheduleExam(ei, _ti, room);
        }
    }

}


/**
 * @brief replacePeriod Copy external _periodExams vector, with _size exams, to period _ti
 * @param _ti
 * @param _periodExams
 * @param _size
 */
void TimetableContainerMatrix::replacePeriod(int _ti, const std::vector<int> &_completePeriod, int _size) {

/// TODO - OPTIMIZED VERSION
///
///
//    // Copy complete column
//    timetableContainer.setColumn(_ti, _completePeriod);
//    periodsSizes[_ti] = _size;
//    // Replace periodExams
//    periodsExams[_ti].clear();
//    for (int ei = 0; ei < timetableContainer.getNumLines(); ++ei) {
//        // Copy period exams
//        //
//        // If there's an exam, insert it
//        if (_completePeriod[ei] != REMOVE_EXAM) {
//            // Get room
//            int roomi = _completePeriod[ei];
//            periodsExams[_ti].push_back(std::make_tuple(ei, roomi));
//        }
//    }


    removeAllPeriodExams(_ti);

    for (int ei = 0; ei < timetableContainer.getNumLines(); ++ei) {
        // Copy period exams
        //
        // If there's an exam, insert it
        if (_completePeriod[ei] != REMOVE_EXAM) {
            // Get room
            int roomi = _completePeriod[ei];
            // Schedule exam
            scheduleExam(ei, _ti, roomi);
        }
    }

}


/**
 * @brief TimetableContainerMatrix::scheduleExam
 * @param _ei
 * @param _tj
 * @param _rk
 */
void TimetableContainerMatrix::scheduleExam(int _ei, int _tj, int _rk) {

#ifdef DEBUG_MODE
    cout << "In [TimetableContainerMatrix::scheduleExam] method: " << endl;
    bool examScheduled = isExamScheduled(_ei, _tj);
    cout << "exam = " << _ei << ", timeslot = " << _tj << ", room = " << _rk << endl
        << ", isScheduled() = " << (examScheduled ? "Yes" : "No") << endl;
    if (examScheduled)
        throw std::runtime_error("TimetableContainerMatrix::scheduleExam: exam should be not scheduled");
#endif

#ifdef TIMETABLECONTAINERMATRIX_DEBUG
//    cout << "scheduledExamsVector.size() = " << scheduledExamsVector.size() << endl;
//    cout << "scheduledRoomsVector.size() = " << scheduledRoomsVector.size() << endl;
//    cout << "_ei = " << _ei << ", _tj = " << _tj << ", _rk = " << _rk << endl;
//    cin.get();

#endif
    //
    // Schedule exam 'ei' in time slot 'tj' and room 'rk'
    //
    // Insert exam in the selected period-room
    insertExam(_ei, _tj, _rk);
    // Set period and room in scheduleExamsVector
    scheduledExamsVector[_ei].schedule(_tj, _rk);

#ifdef TIMETABLECONTAINERMATRIX_DEBUG
    std::cout << "[scheduleExam] ei = " << _ei << ", tj = " << _tj << ", rk = " << _rk << endl;
#endif

    addExamToRoom(_ei, _tj, _rk);
}



/**
 * @brief addExamToRoom
 * @param _ei
 * @param _tj
 * @param _rk
 */
void TimetableContainerMatrix::addExamToRoom(int _ei, int _tj, int _rk) {
    // Get number of currently occupied seats for room 'rk'
    int numOccupiedSeats = scheduledRoomsVector[_rk].getNumOccupiedSeats(_tj);
    // Get exam vector
    auto const &examVector = timetableProblemData->getExamVector();
    // Get number of students for exam _ei
    int thisExamNumStudents = examVector[_ei]->getNumStudents();
    // Update room's # occupied seats
    scheduledRoomsVector[_rk].setNumOccupiedSeats(_tj, numOccupiedSeats + thisExamNumStudents);
    // Update room's # exams scheduled
    scheduledRoomsVector[_rk].setNumExamsScheduled(_tj, scheduledRoomsVector[_rk].getNumExamsScheduled(_tj)+1);
}



/**
 * @brief TimetableContainerMatrix::unscheduleExam
 * @param _ei
 * @param _tj
 */
void TimetableContainerMatrix::unscheduleExam(int _ei, int _tj) {

#ifdef DEBUG_MODE
    cout << "In [TimetableContainerMatrix::unscheduleExam] method: " << endl;
    bool examScheduled = isExamScheduled(_ei, _tj);
    // Get scheduled period
    int scheduledTimeslot = scheduledExamsVector[_ei].getPeriod();
    // Get scheduled room
    int scheduledRoom = scheduledExamsVector[_ei].getRoom();
    cout << "exam = " << _ei << ", timeslot = " << _tj << ", room = " << scheduledRoom << endl
         << "scheduledTimeslot = " << scheduledTimeslot << ", scheduledRoom = " << scheduledRoom
        << ", isScheduled() = " << (examScheduled ? "Yes" : "No") << endl;
    if (!examScheduled)
        throw std::runtime_error("TimetableContainerMatrix::unscheduleExam: exam should be scheduled");
#endif

    //
    // Unschedule exam 'ei' from time slot 'tj'
    //
    // Get exam room
    int rk = getRoom(_ei, _tj);
    // Remove exam in the selected period-room
    removeExam(_ei, _tj);
    // Unset period and room in scheduleExamsVector
    scheduledExamsVector[_ei].unschedule();

#ifdef TIMETABLECONTAINERMATRIX_DEBUG
    std::cout << "[unscheduleExam] ei = " << _ei << ", tj = " << _tj << ", rk = " << rk << endl;
#endif

    removeExamFromRoom(_ei, _tj, rk);

}





/**
 * @brief removeExamFromRoom
 * @param _ei
 * @param _tj
 */
void TimetableContainerMatrix::removeExamFromRoom(int _ei, int _tj, int _rk) {
    // Get number of currently occupied seats for room 'rk'
    int numOccupiedSeats = scheduledRoomsVector[_rk].getNumOccupiedSeats(_tj);
    // Get exam vector
    auto const &examVector = timetableProblemData->getExamVector();
    // Get number of students for exam _ei
    int thisExamNumStudents = examVector[_ei]->getNumStudents();
    // Update room's # occupied seats
    scheduledRoomsVector[_rk].setNumOccupiedSeats(_tj, numOccupiedSeats - thisExamNumStudents);
    // Update room's # exams scheduled
    scheduledRoomsVector[_rk].setNumExamsScheduled(_tj, scheduledRoomsVector[_rk].getNumExamsScheduled(_tj)-1);

}









