#ifndef ITC2007CONSTRAINTS_H
#define ITC2007CONSTRAINTS_H


#include "chromosome/eoChromosome.h"

#include "data/ScheduledExam.h"
#include "data/ScheduledRoom.h"
#include "containers/IntMatrix.h"
#include <boost/shared_ptr.hpp>
#include "data/Constraint.hpp"



//#define ITC2007CONSTRAINTS_DEBUG



////////////////////////////////////////////////////////////////////////////
//
// Hard constraints
//
// The STRICT-HARD constraints
//
// All solutions must, without exception, satisfy the following constraints:
// COMPLETE-ALLOCATION : All exams must actually take place. Scheduling problems are
//   never a good reason to not run an exam!
// NO-PERIOD-SPLITTING : Exams cannot be split between periods.
// NO-ROOM-SPLITTING : Exams cannot be split between rooms.
// Observe that these correspond to requiring that all exams are assigned to precisely one
// period and one room.
//
// The RELAXABLE-HARD constraints
//
// NO-CONFLICTS : Conflicting exams cannot be assigned to the same period. (As usual,
//   two exams are said to conflict whenever they have some student taking them both.)
// ROOM-OCCUPANCY : For every room and period no more seats are used than are available
//   for that room.
// PERIOD-UTILISATION : No exam assigned to a period uses more time than available for
//   that period.
// PERIOD-RELATED : All of a set of time-ordering requirements between pairs of exams
//   are obeyed. Specifically, for any pair (e1, e2) of exams, one can specify any of:
//   - ‘AFTER’: e1 must take place strictly after e2 ,
//   - ‘EXAM_COINCIDENCE’: e1 must take place at the same time as e2 ,
//   - ‘EXCLUSION’: e1 must not take place at the same time as e2 .
// ROOM-RELATED : Room requirements are obeyed. Specifically, for any exam one can
//   disallow the usual sharing of rooms and specify
//   - ‘EXCLUSIVE’: Exam e must take place in a room on its own.
////////////////////////////////////////////////////////////////////////////



// E.g.
// [PeriodHardConstraints]
// 0, AFTER, 3           Exam ‘0' should be timetabled after Exam ‘3'
class AfterConstraint : public BinaryConstraint {
public:
    // Constructor
    // Pre-cond: exams indexes e1 and e2 in [0, size-1]
    // Exams must be scheduled
    AfterConstraint(int _e1, int _e2)
        : BinaryConstraint(_e1, _e2) { }

    double operator()(eoChromosome const& _chrom) const override {
#ifdef ITC2007CONSTRAINTS_DEBUG
        std::cout << "[AfterConstraint] e1 = " << getE1() << ", e2 = " << getE2() << std::endl;
#endif
        // Get exams references
        std::vector<ScheduledExam> const& examVec = _chrom.getScheduledExamsVector();
        ScheduledExam const &exam1 = examVec[getE1()];
        ScheduledExam const &exam2 = examVec[getE2()];

        // If exam1 is scheduled after exam2 then return penalty = 0, else return penalty = 1
        double penalty = (exam1.getPeriod() > exam2.getPeriod()) ? 0 : 1;

        return penalty;
    }
};




// E.g.
// [PeriodHardConstraints]
// 0, EXAM_COINCIDENCE, 1   Exam ‘0' and Exam ‘1' should be timetabled in the same period. If two exams are set associated in
//                          this manner yet 'clash' with each other due to student enrolment, this hard constraint is ignored.
class ExamCoincidenceConstraint : public BinaryConstraint {
public:
    // Constructor
    // Pre-cond: exams indexes e1 and e2 in [0, size-1]
    // Exams must be scheduled
    ExamCoincidenceConstraint(int _e1, int _e2)
        : BinaryConstraint(_e1, _e2) { }

     double operator()(eoChromosome const& _chrom) const override {
#ifdef ITC2007CONSTRAINTS_DEBUG
         std::cout << "[ExamCoincidenceConstraint] e1 = " << getE1() << ", e2 = " << getE2() << std::endl;
#endif
        // Get exams references
        std::vector<ScheduledExam> const& examVec = _chrom.getScheduledExamsVector();
        ScheduledExam const& exam1 = examVec[getE1()];
        ScheduledExam const& exam2 = examVec[getE2()];
        // Get conflict matrix
        IntMatrix const& conflictMatrix = _chrom.getConflictMatrix();
        // If exam1 and exam2 have not conflicts then they should be scheduled on the same period.
        // Return penalty == 0 if exams clash with each other (ignoring the constraint)
        // or if exams don't clash and are scheduled in the same period. Return penalty == 1 otherwise.
        double penalty = (exam1.getPeriod() == exam2.getPeriod() && conflictMatrix.getVal(getE1(), getE2()) == 0
                          || conflictMatrix.getVal(getE1(), getE2()) > 0) ? 0 : 1;
        return penalty;
    }
};




// E.g.
// [PeriodHardConstraints]
// 0, EXCLUSION, 2       Exam ‘0' and Exam ‘2' should be not be timetabled in the same period
class ExamExclusionConstraint : public BinaryConstraint {
public:
    // Constructor
    // Pre-cond: exams indexes e1 and e2 in [0, size-1]
    // Exams must be scheduled
    ExamExclusionConstraint(int _e1, int _e2)
        : BinaryConstraint(_e1, _e2) { }

    double operator()(eoChromosome const& _chrom) const override {
#ifdef ITC2007CONSTRAINTS_DEBUG
        std::cout << "[ExamExclusionConstraint] e1 = " << getE1() << ", e2 = " << getE2() << std::endl;
#endif
        // Get exams references
        std::vector<ScheduledExam> const& examVec = _chrom.getScheduledExamsVector();
        ScheduledExam const &exam1 = examVec[getE1()];
        ScheduledExam const &exam2 = examVec[getE2()];

        // if exam1 is scheduled in a different period than exam2 then return penalty = 0, else return penalty = 1
        double penalty = (exam1.getPeriod() != exam2.getPeriod()) ? 0 : 1;
#ifdef ITC2007CONSTRAINTS_DEBUG
        std::cout << "exam1.getPeriod() = " << exam1.getPeriod() << ", exam2.getPeriod() = " << exam2.getPeriod()
             << ", distance to feasibility = " << penalty << std::endl;
#endif
        return penalty;

    }
};




// E.g.
// [RoomHardConstraints]
// 2, ROOM_EXCLUSIVE      Exam ‘2' must be timetabled in a room by itself.
class RoomExclusiveConstraint : public UnaryConstraint {
public:
    // Constructor
    // Pre-cond: exam index e and room index in [0, size-1]
    // Exam must be scheduled
    RoomExclusiveConstraint(int _e)
        : UnaryConstraint(_e) { }

    double operator()(eoChromosome const &_chrom) const override {
#ifdef ITC2007CONSTRAINTS_DEBUG
        std::cout << "[RoomExclusiveConstraint] e = " << getE() << std::endl;
#endif
        // If exam is scheduled in a room by itself then return penalty = 0, else return penalty = 1

        // Get exam reference
        std::vector<ScheduledExam> const &examVec = _chrom.getScheduledExamsVector();
        ScheduledExam const &exami = examVec[getE()];
        // Period where the exam was scheduled
        int timeslot = exami.getPeriod();
        // Get room reference
        std::vector<ScheduledRoom> const &roomVec = _chrom.getScheduledRoomsVector();
        ScheduledRoom const &room = roomVec[exami.getRoom()];
        // Return penalty = 0 if this room only has exami timetabled in it, and penalty = 1 otherwise
        double penalty = (room.getNumExamsScheduled(timeslot) == 1) ? 0 : 1;

        return penalty;
    }
};




#endif // ITC2007CONSTRAINTS_H










