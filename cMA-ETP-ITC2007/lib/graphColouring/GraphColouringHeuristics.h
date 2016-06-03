#ifndef GRAPH_COLOURING_HEURISTIC
#define GRAPH_COLOURING_HEURISTIC


#include "data/ITC2007Constraints.hpp"

#include <boost/graph/adjacency_list.hpp>
#include "utils/Common.h"
#include "containers/VertexPriorityQueue.h"
#include "containers/TimetableContainer.h"
#include "containers/TimetableContainerMatrix.h"
#include "data/TimetableProblemData.hpp"
#include <utils/eoRNG.h>
#include <boost/random.hpp>

#include <tuple>
#include <boost/unordered_map.hpp>



//#define GRAPH_COLOURING_HEURISTIC_DEBUG_CONFLICTS


//#define GRAPH_COLOURING_HEURISTIC_DEBUG_ROOM

//#define GRAPH_COLOURING_HEURISTIC_DEBUG

//#define GRAPH_COLOURING_HEURISTIC_DEBUG_1

//#define GRAPH_COLOURING_HEURISTIC_DEBUG_2

//#define GRAPH_COLOURING_HEURISTIC_DEBUG_CONSTRAINTS



//#define GRAPH_COLOURING_HEURISTIC_DEBUG_10

//===
//
// ITC2007 Hard constraints
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
// NO-CONFLICTS: Conflicting exams cannot be assigned to the same period. (As usual,
//   two exams are said to conflict whenever they have some student taking them both.)
// ROOM-OCCUPANCY: For every room and period no more seats are used than are available
//   for that room.
// PERIOD-UTILISATION: No exam assigned to a period uses more time than available for
//   that period.
// PERIOD-RELATED: All of a set of time-ordering requirements between pairs of exams
//   are obeyed. Specifically, for any pair (e1, e2) of exams, one can specify any of:
//   - ‘AFTER’: e1 must take place strictly after e2,
//   - ‘EXAM_COINCIDENCE’: e1 must take place at the same time as e2,
//   - ‘EXCLUSION’: e1 must not take place at the same time as e2.
// ROOM-RELATED: Room requirements are obeyed. Specifically, for any exam one can
//   disallow the usual sharing of rooms and specify
//   - ‘EXCLUSIVE’: Exam e must take place in a room on its own.
//
//===

template <typename EOT>
class GCHeuristics {
public:
    // Typedefs
    typedef std::tuple<int, int> i2tuple;
    typedef std::tuple<int, int, int, int, int, int> i6tuple;
    typedef std::tuple<int, int, int> VariableValueTuple;

public:
    /**
     * @brief saturationDegree
     * @param _timetableProblemData
     * @param _chrom
     */
    static void saturationDegree(TimetableProblemData const *_timetableProblemData, EOT &_chrom);

private:

    /**
     * @brief computePriority
     * @param _ej
     * @return
     */
    static int computePriority(int _ej);

    /**
     * @brief domain
     * @param _values
     */
    static void domain(EOT &_chrom, std::vector<i2tuple> &_values);

    /**
     * @brief feasiblePeriod
     * @param _variable
     * @return
     */
    static bool feasiblePeriod(EOT &_chrom, const VariableValueTuple &_variable);

    /**
     * @brief getFeasiblePeriodRoom
     * @param _ei
     * @param _tj
     * @param _rk
     * @param _examsAvailablePeriodsList
     * @return true is a feasible period-room was found for allocating exam _ei, and return false otherwise
     */
    static bool getFeasiblePeriodRoom(EOT &_chrom, int _ei, int& _tj, int& _rk, vector<int> &_examsAvailablePeriodsList);

public:
    /**
     * @brief hardConflicts
     * @param _A
     * @param _a
     * @return
     */
    static std::vector<VariableValueTuple> hardConflicts(EOT &_chrom, int _A, const i2tuple &_a);


    /**
     * @brief hardConflictsWithoutAfterConstraint
     * @param _A
     * @param _a
     * @return
     */
    static std::vector<VariableValueTuple> hardConflictsWithoutAfterConstraint(EOT &_chrom, int _A, const i2tuple &_a);




private:
    /**
     * @brief init
     * @param _timetableProblemData
     * @param _chrom
     */
    static void init(const TimetableProblemData *_timetableProblemData, EOT &_chrom);

    /**
     * @brief initialiseVertexPriorityQueue
     * @param _pq
     * @param _examsAvailablePeriodsList
     */
    static void initialiseVertexPriorityQueue(VertexPriorityQueue &_pq, vector<vector<int> > &_examsAvailablePeriodsList);

    /**
     * @brief scheduleExam
     * @param _ei
     * @param _value
     */
    static void scheduleExam(EOT &_chrom, int _ei, i2tuple const &_value);

    /**
     * @brief selectValue
     * @param _A
     * @param _hardConflicts
     * @param _domainA
     * @return
     */
    static i2tuple selectValue(EOT &_chrom, int _A, std::vector<VariableValueTuple> &_hardConflicts, const std::vector<i2tuple> &_domainA);

    /**
     * @brief selectVariable
     * @param _pq
     * @return
     */
    static i2tuple selectVariable(VertexPriorityQueue &_pq);

    /**
     * @brief tryScheduleExam
     * @param _ei
     * @param _tj
     * @param _rk
     * @param _examsAvailablePeriodsList
     * @return
     */
    static bool tryScheduleExam(EOT &_chrom, int _ei, int &_tj, int &_rk, vector<vector<int> > &_examsAvailablePeriodsList);

    /**
     * @brief unscheduleConflictingExams
     * @param _pq
     * @param _eiHardConflicts
     * @param _scheduledExams
     * @param _numScheduledExams
     */
    static void unscheduleConflictingExams(EOT &_chrom, VertexPriorityQueue &_pq,
                                           std::vector<VariableValueTuple> const &_eiHardConflicts,
                                           vector<bool> &_scheduledExams, int &_numScheduledExams);

    /**
     * @brief unscheduleExam
     * @param _ej
     */
    static void unscheduleExam(EOT &_chrom, int _ei);


    /**
     * @brief updatePeriodAvailability
     * @param _ei
     * @param _tj
     * @param _chrom
     * @param _scheduledExams
     * @param _examsAvailablePeriodsList
     * @param _pq
     */
    static void updatePeriodAvailability(EOT &_chrom, int _ei, int _tj, std::vector<bool> _scheduledExams,
                                         vector<vector<int>> &_examsAvailablePeriodsList, VertexPriorityQueue &_pq);

    /**
     * @brief buildPeriodRelatedHardConflicts
     * @param _ei
     * @param _tj
     * @param _variables
     */
    static void buildPeriodRelatedHardConflicts(EOT &_chrom, int _ei, int _tj, std::vector<VariableValueTuple> &_variables);


    /**
     * @brief buildPeriodRelatedExclusionHardConflicts
     * @param _ei
     * @param _tj
     * @param _variables
     */
    static void buildPeriodRelatedExclusionHardConflicts(EOT &_chrom, int _ei, int _tj, std::vector<VariableValueTuple> &_variables);


    /**
     * @brief buildRoomRelatedConflicts
     * @param _ei
     * @param _tj
     * @param _rk
     * @param _variables
     */
    static void buildRoomRelatedConflicts(EOT &_chrom, int _ei, int _tj, int _rk, std::vector<VariableValueTuple> &_variables);

    //--
    // Fields
    //--

    /**
     * @brief numExams the number of vertices
     */
    static int numExams;
    /**
     * @brief numPeriods Number of periods
     */
    static int numPeriods;

    /**
     * @brief timetableProblemData - TimetableProblemData is const
     */
    static TimetableProblemData const *timetableProblemData;

    /**
     * @brief CBS Conflict-Based Statistics
     */
    static boost::unordered_map<i6tuple, int> CBS;
};



// Static fields



template <typename EOT>
int GCHeuristics<EOT>::numExams = 0;


template <typename EOT>
int GCHeuristics<EOT>::numPeriods = 0;


template <typename EOT>
TimetableProblemData const *GCHeuristics<EOT>::timetableProblemData = nullptr;


template <typename EOT>
boost::unordered_map<typename GCHeuristics<EOT>::i6tuple, int> GCHeuristics<EOT>::CBS;




/**
 * @brief saturationDegree
 * @param _timetableProblemData
 * @param _chrom
 */
template <typename EOT>
void GCHeuristics<EOT>::saturationDegree(TimetableProblemData const *_timetableProblemData, EOT &_chrom) {
    //===
    // SD (Saturation Degree) graph colouring heuristic for ITC2007:
    //
    //    In the SD heuristic, the exams are sorted in a priority queue by their period availability and difficulty.
    //    Exams that have more hard constraints are scheduled first.
    //    A period is marked as available for a given exam if the Non-Conflicts hard constraint is respected.
    //    The remaining hard constraints are guaranteed when the exam is scheduled.
    //    The hard constraints comprise:
    //    1 - No-Conflicts
    //    2 - Room-Occupancy
    //    3 - Period-Utilisation
    //    4 - Period-Related
    //    5 - Room-Related
    //
    //    That is:
    //    (1) if there aren't conflicts with exams scheduled in that period,
    //    (2) if there exists at least one room with sufficient seats,
    //    (3) the exam does not exceed the time available for that period, and
    //    (4 and 5) all period and room related constraints are satisfied.
    //
    //===
    // Muller construction algorithm
    //
    // function Construction()
    //     sigma = empty
    //     while not Complete(sigma) and not Timeout do
    //         v = SelectVariable(sigma)
    //         d = SelectValue(sigma, v)
    //         eta = HardConflicts(sigma, v, d)
    //         sigma = sigma \ eta U {v/d}
    //     return sigma
    //===
    //
    // SD construction algorithm
    //
    // 1. Create a priority queue with the exams sorted randomly. Only exams that have the 'After' harder constraint
    //    have less available periods and are scheduled first. The others exams have, initially, all available periods.
    //
    // 2. while not Complete(sigma) and not Timeout do
    //    2.1     v = SelectVariable(sigma)
    //    2.2     d = SelectValue(sigma, v)
    //    2.3     eta = HardConflicts(sigma, v, d)
    //    2.4     sigma = sigma \ eta U {v/d}
    // 3. return sigma
    //===
    // Description
    //
    // 1. Create a priority queue with the exams sorted randomly. Only exams that have the 'After' harder constraint
    //    have less available periods and are scheduled first. The others exams have, initially, all available periods.
    //
    // 2. While the queue is not empty
    //   2.1 - SelectVariable - Pop exam 'ei' from the top of exam's priority queue
    //   2.2 - If the available period list for exam 'ei' is not empty, try to schedule 'ei' into feasible period 'tj' and room 'rk'.
    //   2.3 - In case of success, go to Step 2.5, else go to Step 2.4
    //   2.4 - SelectValue - If the available period list for exam 'ei' is empty, or the
    //         selected period and room is not feasible (at least one of the constraints 2-5 is violated),
    //         schedule 'ei' into period 'tj' and room 'rk' which minimises the Hard conflicts (apply Conflict-Based Statistics),
    //         unschedule all conflicting exams, and put them in the priority queue again. Optionally, we can also minimise the cost.
    //   2.5. Update the period availability for all unscheduled exams according to the 'No-Conflicts' hard constraint.
    //
    // 3. Return the feasible solution
    //===

#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG
    cout << "GCHeuristics::saturationDegree method" << endl;
#endif

    // Init fields
    init(_timetableProblemData, _chrom);
    // Create exam priority queue
    VertexPriorityQueue pq(numExams);
    // Available period's list
    vector<vector<int>> examsAvailablePeriodsList(numExams);
    // 1. Create a priority queue with the exams sorted randomly. Only exams that have the 'After' harder constraint
    //    have less available periods and are scheduled first. The others exams have, initially, all available periods.
    initialiseVertexPriorityQueue(pq, examsAvailablePeriodsList);
    // Variable domain
    std::vector<i2tuple> domainValues;
    domain(_chrom, domainValues);
    // Scheduled exams
    vector<bool> scheduledExams(numExams); // Initially, no exam is scheduled, so entries are set to false
    // # scheduled exams
    int numScheduledExams = 0;
    // Flag to indicate if exam was scheduled
    bool examWasScheduled = false;
    // Period where exam 'ei' was scheduled
    int tj = -1;
    // Room where exam 'ei' was scheduled
    int rk = -1;

    // 2. While the queue is not empty
    while (!pq.empty()) {
        //   2.1 - SelectVariable - Pop exam 'ei' from the top of exam's priority queue
        i2tuple eiVariable = selectVariable(pq);
        // Exam ei
        int ei = std::get<0>(eiVariable);
        // # AvailablePeriods
        int numAvailablePeriods = std::get<1>(eiVariable);
        // Se current exam ei as unsecheduled
        examWasScheduled = false;
        //
        //   2.2 - If the available period list for exam 'ei' is not empty, try to schedule 'ei' into feasible period 'tj' and room 'rk'.
        if (numAvailablePeriods > 0) {
            // Period tj and room rk are out parameters and are updated
            examWasScheduled = tryScheduleExam(_chrom, ei, tj, rk, examsAvailablePeriodsList);
        }
        //   2.3 - In case of success, go to Step 2.5, else go to Step 2.4
        if (!examWasScheduled) {
            for (;;) {

            //   2.4 - SelectValue - If the available period list for exam 'ei' is empty, or the
            //         selected period and room is not feasible (at least one of the constraints 2-5 is violated),
            //         schedule 'ei' into period 'tj' and room 'rk' which minimises the Hard conflicts (apply Conflict-Based Statistics),
            //         unschedule all conflicting exams, and put them in the priority queue again. Optionally, we can also minimise the cost.
            //
            // Vector for keeping the 'ei' Hard conflicts
            std::vector<VariableValueTuple> eiHardConflicts;
            // Select feasible period 'tj' and room 'rk'
            i2tuple value = selectValue(_chrom, ei, eiHardConflicts, domainValues);
            // Update tj
            tj = std::get<0>(value);
            // Get room rk
            rk = std::get<1>(value);
            // Unschedule all conflicting exams, and put them in the priority queue again
            unscheduleConflictingExams(_chrom, pq, eiHardConflicts, scheduledExams, numScheduledExams);
            // Schedule 'ei' into period and room
            scheduleExam(_chrom, ei, value);
            // Set exam as scheduled
            examWasScheduled = true;

            if (pq.empty()) {
                //
                // 3. Return the feasible solution
                //
                // Set chromosome as a feasible one
                _chrom.setFeasible(true);
                return;
            }
            //   2.1 - SelectVariable - Pop exam 'ei' from the top of exam's priority queue
            i2tuple eiVariable = selectVariable(pq);
            // Exam ei
            ei = std::get<0>(eiVariable);
            // # AvailablePeriods
//            int numAvailablePeriods = std::get<1>(eiVariable);

        }
    }

        //   2.5. Update the period availability for all unscheduled exams according to the 'No-Conflicts' hard constraint.
        //

//        if (examWasScheduled) {
            updatePeriodAvailability(_chrom, ei, tj, scheduledExams, examsAvailablePeriodsList, pq);
            // Set exam ei as scheduled
            scheduledExams[ei] = true;
            // Increment # scheduled exams
            ++numScheduledExams;

    }
    //
    // 3. Return the feasible solution
    //
    // Set chromosome as a feasible one
    _chrom.setFeasible(true);

//    _chrom.validate();

#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG_1
    cout << "Total # scheduled exams: " << numScheduledExams << endl;
#endif

}





/**
 * @brief buildPeriodRelatedHardConflicts Pre-condition: ei is not scheduled
 * @param _ei
 * @param _tj
 * @param _variables
 */
template <typename EOT>
void GCHeuristics<EOT>::buildPeriodRelatedHardConflicts(EOT &_chrom, int _ei, int _tj, std::vector<VariableValueTuple> &_variables) {
#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG_1
    cout << "verifyAfterAndExlusionHardConstraints" << endl;
#endif

    // Get scheduled exams vector
    auto const &scheduledExamsVector = _chrom.getScheduledExamsVector();
    // Get scheduled exam
    ScheduledExam const &exam_i = scheduledExamsVector[_ei];
    // Get exam vector
    auto const &examVector = _chrom.getExamVector();
    // Get  exam
    Exam &exam_info = *examVector[_ei].get();
    // Get exam 'ei' Period-Related Hard constraints
    auto const &periodRelatedHardConstraintsVector = exam_info.getPeriodRelatedHardConstraints();
    // Get TimetableContainer object
    TimetableContainer &timetableCont = _chrom.getTimetableContainer();
    //---
    // We have to schedule temporarily exam 'ei' in order to verify the constraints
    int dummyRoom = 0;
    // We have to schedule temporarily exam 'ei' in order to verify the constraints
    timetableCont.scheduleExam(_ei, _tj, dummyRoom);
    //---

    // Verify if constraints are satisfied for exam 'ei' scheduled at period 'tj'
    for (auto const& ptrConstraint : periodRelatedHardConstraintsVector) {
        boost::shared_ptr<BinaryConstraint> ptrBinConstraint =
                boost::dynamic_pointer_cast<BinaryConstraint, Constraint>(ptrConstraint);
        // Verify if exams are scheduled. Only if both exams are scheduled the constraint is verified.
        if (scheduledExamsVector[ptrBinConstraint->getE1()].isScheduled() &&
            scheduledExamsVector[ptrBinConstraint->getE2()].isScheduled()) {

            double distanceToFeasibility;
            // If the constraint is not satisfied, add conflicting exam to the list to return
            if ( (distanceToFeasibility = ptrConstraint->operator()(_chrom)) > 0) {
///========================================================================
/// SEE!!!
///
///
///
///
///
///========================================================================
                // The 'ei' exam is E1; the other exam is E2
                int otherExam = (ptrBinConstraint->getE1() == _ei) ? ptrBinConstraint->getE2() : ptrBinConstraint->getE1();
//                Exam &other = *examVector[otherExam].get();
                ScheduledExam const &other = scheduledExamsVector[otherExam];
                // Get other exam period and room
                int conflictingExamRoom = other.getRoom();
                int conflictingExamPeriod = other.getPeriod();
                // Add exam to unscheduled exams
                _variables.push_back(std::make_tuple(otherExam, conflictingExamPeriod, conflictingExamRoom));
            }
        }
    }
    //---
    // Unschedule exam
    timetableCont.unscheduleExam(_ei, _tj);
    //---
}






/**
 * @brief buildPeriodRelatedExclusionHardConflicts Pre-condition: ei is not scheduled
 * @param _ei
 * @param _tj
 * @param _variables
 */
template <typename EOT>
void GCHeuristics<EOT>::buildPeriodRelatedExclusionHardConflicts(EOT &_chrom, int _ei, int _tj, std::vector<VariableValueTuple> &_variables) {
    // Get scheduled exams vector
    auto const &scheduledExamsVector = _chrom.getScheduledExamsVector();
    // Get scheduled exam
    ScheduledExam const &exam_i = scheduledExamsVector[_ei];
    // Get exam vector
    auto const &examVector = _chrom.getExamVector();
    // Get  exam
    Exam &exam_info = *examVector[_ei].get();
    // Get exam 'ei' Period-Related Hard constraints
    auto const &periodRelatedHardConstraintsVector = exam_info.getPeriodRelatedHardConstraints();
    // Get TimetableContainer object
    TimetableContainer &timetableCont = _chrom.getTimetableContainer();
    //---
    // We have to schedule temporarily exam 'ei' in order to verify the constraints
    int dummyRoom = 0;
    // We have to schedule temporarily exam 'ei' in order to verify the constraints
    timetableCont.scheduleExam(_ei, _tj, dummyRoom);
    //---

    // Verify if constraints are satisfied for exam 'ei' scheduled at period 'tj'
    for (auto const &ptrConstraint : periodRelatedHardConstraintsVector) {
        boost::shared_ptr<ExamExclusionConstraint> ptrExclusionConstraint =
                boost::dynamic_pointer_cast<ExamExclusionConstraint, Constraint>(ptrConstraint);
        // Verify if exams are scheduled. Only if both exams are scheduled the constraint is verified.
        if (ptrExclusionConstraint.get() != nullptr &&
            scheduledExamsVector[ptrExclusionConstraint->getE1()].isScheduled() &&
            scheduledExamsVector[ptrExclusionConstraint->getE2()].isScheduled()) {

            double distanceToFeasibility;
            // If the constraint is not satisfied, add conflicting exam to the list to return
            if ( (distanceToFeasibility = ptrConstraint->operator()(_chrom)) > 0) {
///========================================================================
/// SEE!!!
///
///
///
///
///
///========================================================================
                // The 'ei' exam is E1; the other exam is E2
                int otherExam = (ptrExclusionConstraint->getE1() == _ei) ? ptrExclusionConstraint->getE2() : ptrExclusionConstraint->getE1();
                ScheduledExam const &other = scheduledExamsVector[otherExam];
                // Get other exam period and room
                int conflictingExamRoom = other.getRoom();
                int conflictingExamPeriod = other.getPeriod();
                // Add exam to unscheduled exams
                _variables.push_back(std::make_tuple(otherExam, conflictingExamPeriod, conflictingExamRoom));
            }
        }
    }
    //---
    // Unschedule exam
    timetableCont.unscheduleExam(_ei, _tj);
    //---
}





/**
 * @brief buildRoomRelatedConflicts Pre-condition: ei is not scheduled
 * @param _ei
 * @param _tj
 * @param _rk
 * @param _variables
 */
template <typename EOT>
void GCHeuristics<EOT>::buildRoomRelatedConflicts(EOT &_chrom, int _ei, int _tj, int _rk, std::vector<VariableValueTuple> &_variables) {
    //==
    // Room-Related: Room requirements are obeyed. Specifically, for any exam one can
    //   disallow the usual sharing of rooms and specify
    //   - ‘EXCLUSIVE’: Exam e must take place in a room on its own.
    //   E.g.: 2, ROOM_EXCLUSIVE      Exam '2' must be timetabled in a room by itself.
    //==

    // Get scheduled exams vector
    auto const &scheduledExamsVector = _chrom.getScheduledExamsVector();
    // Get scheduled exam
    ScheduledExam const &exam_i = scheduledExamsVector[_ei];
    // Get exam vector
    auto const &examVector = _chrom.getExamVector();
    // Get exam
    Exam &exam_info = *examVector[_ei].get();
    // Get TimetableContainer object
    TimetableContainer &timetableCont = _chrom.getTimetableContainer();
    //
    // There are two possible scenarios:
    // 1. ei has ROOM_EXCLUSIVE constraint itself
    //     - the conflicting exams must be removed
    //
    // 2. The room where ei is going to be inserted has one other exam
    //    which has ROOM_EXCLUSIVE constraint
    //     - that exam is conflicting and have to be removed
    //

    //
    // Scenario 1. ei has ROOM_EXCLUSIVE constraint itself
    //     - the conflicting exams must be removed
    //

    // Get exam 'ei' Room-Related Hard constraints
    auto const &roomRelatedHardConstraintsVector = exam_info.getRoomRelatedHardConstraints();
    if (!roomRelatedHardConstraintsVector.empty()) {
        // Verify if the room constraint is satisfied for exam 'ei' scheduled at period 'tj' and room 'rk'
        auto const &ptrConstraint = roomRelatedHardConstraintsVector.front();
        boost::shared_ptr<UnaryConstraint> ptrUnaConstraint =
                boost::dynamic_pointer_cast<UnaryConstraint, Constraint>(ptrConstraint);
        //---
        // We have to schedule temporarily exam 'ei' in order to verify the constraints
        timetableCont.scheduleExam(_ei, _tj, _rk);
        //---
        double distanceToFeasibility;
        // If the constraint is not satisfied return the exams that cause hard conflicts
        if ( (distanceToFeasibility = ptrConstraint->operator()(_chrom)) > 0) {
/// COULD BE OPTIMIZED
///
///
            // Determine exams allocated to room rk in period tj
            vector<pair<int,int> > roomExams = _chrom.getRoomExams(_tj, _rk);
//            // Get scheduled rooms vector
//            auto const &scheduledRoomsVector = chrom->getScheduledRoomsVector();
//            // Get # scheduled exams in room rk along with ei
//            int numScheduledExamsRoom = scheduledRoomsVector[_rk].getNumExamsScheduled(_tj);
            // Get # scheduled exams in room rk along with ei
            int numScheduledExamsRoom = roomExams.size();
            // Register as conflicting exams all exams schedule in room rk except ei
            for (int i = 0; i < numScheduledExamsRoom; ++i) {
                // Get conflicting exam
                int conflictingExam = roomExams.back().first;
                if (conflictingExam != _ei) {
                    // Add exam to unscheduled exams
                    _variables.push_back(std::make_tuple(conflictingExam, _tj, _rk));
                }
                // Remove from roomExams
                roomExams.pop_back();
            }
        }
        //---
        // Unschedule exam
        timetableCont.unscheduleExam(_ei, _tj);
        //---

    }


    //
    // Scenario 2. The room where ei is going to be inserted has one other exam
    //    which has ROOM_EXCLUSIVE constraint
    //     - that exam is conflicting and have to be removed
    //
    // Determine exams allocated to room rk in period tj
    vector<pair<int,int> > roomExams = _chrom.getRoomExams(_tj, _rk);
    // Get scheduled rooms vector
    auto const &scheduledRoomsVector = _chrom.getScheduledRoomsVector();
    // Get # scheduled exams in room rk along with ei
    int numScheduledExamsRoom = scheduledRoomsVector[_rk].getNumExamsScheduled(_tj);
//    if (numScheduledExamsRoom == 1) {
    if (roomExams.size() == 1) {
        int conflictingExam = roomExams.back().first;
        // Get exam
        Exam &exam_info = *examVector[conflictingExam].get();
        // Get exam Room-Related Hard constraints
        auto const &roomRelatedHardConstraintsVector = exam_info.getRoomRelatedHardConstraints();
        if (!roomRelatedHardConstraintsVector.empty()) {
            auto const &ptrConstraint = roomRelatedHardConstraintsVector.front();
            boost::shared_ptr<UnaryConstraint> ptrUnaConstraint =
                    boost::dynamic_pointer_cast<UnaryConstraint, Constraint>(ptrConstraint);

            //---
            // We have to schedule temporarily exam 'ei' in order to verify the constraints
            timetableCont.scheduleExam(_ei, _tj, _rk);
            //---

            double distanceToFeasibility;
            // If the constraint is not satisfied return the exams that cause hard conflicts
            if ( (distanceToFeasibility = ptrConstraint->operator()(_chrom)) > 0) {
                // Add exam to unscheduled exams
                _variables.push_back(std::make_tuple(conflictingExam, _tj, _rk));
            }
            //---
            // Unschedule exam
            timetableCont.unscheduleExam(_ei, _tj);
            //---
        }
    }

}






template <typename EOT>
int GCHeuristics<EOT>::computePriority(int _ej) {
    // Compute ej's priority (number of available periods in the timetable)
    //

/// TODO
///
///

    return 0;
}





/**
 * @brief domain
 * @param _values
 */
template <typename EOT>
void GCHeuristics<EOT>::domain(EOT &_chrom, std::vector<i2tuple> &_values) {
    // Return all possible values (tuples <period, room>) for exam variable
    //
    // Chromosome's timetable
    TimetableContainer &timetableCont = _chrom.getTimetableContainer();
    // For each period do
    for (int pi = 0; pi < timetableCont.getNumPeriods(); ++pi) {
        // For each room of period pi do
        for (int rk = 0; rk < timetableCont.getNumRooms(); ++rk) {
            _values.push_back(std::make_tuple(pi, rk));
        }
    }
}





/**
 * @brief feasiblePeriod
 * @param _variable
 * @return
 */
template <typename EOT>
bool GCHeuristics<EOT>::feasiblePeriod(EOT &_chrom, VariableValueTuple const & _variable) {
    // Get exam, period and room
    int ei = std::get<0>(_variable);
    int tj = std::get<1>(_variable);
    int rk = std::get<2>(_variable);
    // If period tj don't respect the Period-Utilisation hard constraint, then tj cannot be used.
    // Return false in this case.
    if (!_chrom.verifyPeriodUtilisationConstraint(ei, tj))
        return false;
    //
    // Verify the Room-Occupancy hard constraint
    //
    // If the exam's students can't be seated in the selected room, return false
    if (!_chrom.verifyRoomCapacity(ei, rk))
        return false;
    // Else, return true.
    return true;
}






/**
 * @brief getFeasiblePeriodRoom
 * @param _ei
 * @param _tj
 * @param _rk
 * @param _examsAvailablePeriodsList
 * @return true is a feasible period-room was found for allocating exam _ei, and return false otherwise
 */
template <typename EOT>
bool GCHeuristics<EOT>::getFeasiblePeriodRoom(EOT &_chrom, int _ei, int &_tj, int &_rk, vector<int> & _availablePeriodsList)
{
    //
    // If a period is marked as available for a given exam, only the 'No-conflicts' hard constraint is guaranteed.
    // The remainder hard constraints must be verified in order for the period to be feasible.
    //
    bool feasiblePeriodFound = false;
    int numAvailablePeriods = _availablePeriodsList.size();

    do {
        // Select a random period 'tj' (with no conficts) for scheduling exam 'ei'.
        int idx = rng.uniform(numAvailablePeriods);
        _tj = _availablePeriodsList[idx];
        // Get a feasible period-room pair. 'tj' and 'rk' are out parameters.
        feasiblePeriodFound = _chrom.getFeasiblePeriodRoom(_ei, _tj, _rk);
#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG
        if (!feasiblePeriodFound) {
            cout << "Period " << _tj << " is not feasible. Try another one..."  << endl;
        }
#endif

        // If the selected period is not feasible, try another random period.
        // If no feasible period can be found, return false.
        if (!feasiblePeriodFound) {
            // Remove period from period list
            _availablePeriodsList.erase(_availablePeriodsList.begin()+idx);
            // Decrement number of available periods
            --numAvailablePeriods;
        }
    }
    while (numAvailablePeriods > 0 && !feasiblePeriodFound);

#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG
    if (feasiblePeriodFound) {
        cout << "Feasible period found."  << endl;
        cout << "_ei = " << _ei << ", _tj = " << _tj << ", _rk = " << _rk << endl;
    }
    else {
        cout << "No feasible period was found." << endl;
    }
#endif
    return feasiblePeriodFound;
}







/**
 * @brief hardConflicts
 * @param _A
 * @param _a
 * @return
 */
template <typename EOT>
std::vector<typename GCHeuristics<EOT>::VariableValueTuple> GCHeuristics<EOT>::hardConflicts(EOT &_chrom, int _A, i2tuple const &_a) {
    //
    // The method hardConflicts returns a subset gamma of the solution sigma,
    // so that there are no hard constraints violated between the remaining
    // assignments sigma\gamma and the new assignment {_A/_a} of the selected
    // variable _A with the selected value _a.
    //
    //==
    // When removing conflicting exams, one should consider the RELAXABLE-HARD constraints:
    //
    // No-Conflicts: Conflicting exams cannot be assigned to the same period. (As usual,
    //   two exams are said to conflict whenever they have some student taking them both.)
    // Room-Occupancy: For every room and period no more seats are used than are available
    //   for that room.
    // [PRE-CONDITION ALREADY VERIFIED IN SELECT_VALUE METHOD]
    // Period-Utilisation: No exam assigned to a period uses more time than available for
    //   that period.
    // Period-Related: All of a set of time-ordering requirements between pairs of exams
    //   are obeyed. Specifically, for any pair (e1, e2) of exams, one can specify any of:
    //   - ‘AFTER’: e1 must take place strictly after e2,
    //   - ‘EXAM_COINCIDENCE’: e1 must take place at the same time as e2,
    //   - ‘EXCLUSION’: e1 must not take place at the same time as e2.
    // Room-Related: Room requirements are obeyed. Specifically, for any exam one can
    //   disallow the usual sharing of rooms and specify
    //   - ‘EXCLUSIVE’: Exam e must take place in a room on its own.
    //==
    // Vector containing variables to return
    std::vector<VariableValueTuple> variables;
    // Timetable container
    TimetableContainer &timetableCont = _chrom.getTimetableContainer();
    // Conflict matrix
    IntMatrix const &conflictMatrix = _chrom.getConflictMatrix();
    // Exam vector
    auto const& examVector = _chrom.getExamVector();
    // Get exam, period and room
    int ei = _A;
    int tj = std::get<0>(_a);
    int rk = std::get<1>(_a);
#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG_ROOM
        cout << "hardConflicts" << endl;
        cout << "ei " << ei << endl;
        cout << "tj " << tj << endl;
        cout << "rk " << rk << endl;
        // Room info
//        cout << "Room info:" << endl;
//        auto const& roomVector = chrom->getRoomVector();
//        for (int i = 0; i < roomVector.size(); ++i) {
//            cout << "r" << i << ": ";
//            for (int period = 0; period < chrom->getNumPeriods(); ++period) {
//                // Get number of currently occupied seats for room i
//                int numOccupiedSeats = roomVector[i]->getNumOccupiedSeats(period);
//                // Get seat capacity for room _rk
//                int roomSeatCapacity = roomVector[i]->getCapacity();
//                // numOccupiedSeats / roomSeatCapacity
//                cout << numOccupiedSeats << ":" << roomSeatCapacity << " ";
//            }
//            cout << endl;
//        }
//        cin.get();
#endif


    //-
    // Verify the Room-Occupancy hard constraint within period tj and room rk
    // Room-Occupancy: For every room and period no more seats are used than are available
    //   for that room.
    //-
    if (!_chrom.verifyRoomOccupancyConstraint(ei, tj, rk)) {
        // Remove the minimum set of conflicting exams that prevent ei from being allocated to room rk
        //
///
/// TODO - COULD BE OPTIMIZED - MAINTAIN IN AN AUXILIARY STRUCTURE EXAMS SORTED
///
        // Determine exams allocated to room rk in period tj
        vector<pair<int,int> > roomExams = _chrom.getRoomExams(tj, rk);
        // Get number of students for exam _ei
        int thisExamNumStudents = examVector[ei]->getNumStudents();
        // Get room vector
        auto const &roomVector = _chrom.getRoomVector();
        // Get scheduled rooms vector
        auto const &scheduledRoomsVector = _chrom.getScheduledRoomsVector();
        // Get number of currently occupied seats for room _rk
        int numOccupiedSeats = scheduledRoomsVector[rk].getNumOccupiedSeats(tj);
        // Get seat capacity for room _rk
        int roomSeatCapacity = roomVector[rk]->getCapacity();

#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG_ROOM
        cout << "numOccupiedSeats = " << numOccupiedSeats << ", thisExamNumStudents = " << thisExamNumStudents
             << ", roomSeatCapacity = " << roomSeatCapacity << endl;
        cout << "Room " << rk << " exams: " << endl;
        for (auto const &p : roomExams)
            cout << "exam: " << p.first << " - # students " << p.second << endl;
#endif

        while (numOccupiedSeats + thisExamNumStudents > roomSeatCapacity) {
#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG_ROOM
            cout << "before removing in roomExams..." << endl;
#endif
            // Remove exam with greatest # students allocated to room rk
            // Remove from roomExams
            int largestExam = roomExams.back().first;
            int numStudents = roomExams.back().second;
#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG
//            cout << "largestExam = " << largestExam << endl;
//            cout << "numSeats = " << numSeats << endl;
//            cin.get();
#endif
            roomExams.pop_back();
#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG_ROOM
            cout << "removed exam with num students = " << numStudents << endl;
#endif
            // Add exam to unscheduled exams
            variables.push_back(std::make_tuple(largestExam, tj, rk));
            // Decrement # occupied seats
            numOccupiedSeats -= numStudents;
        }
#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG_ROOM
        cout << "After removing exams from roomExams:" << endl;
        // Get updated numOccupiedSeats
        numOccupiedSeats = scheduledRoomsVector[rk].getNumOccupiedSeats(tj);
        // Not necessary reading these two values because they didn't change...
        thisExamNumStudents = examVector[ei]->getNumStudents();
        roomSeatCapacity = roomVector[rk]->getCapacity();
        cout << "numOccupiedSeats = " << numOccupiedSeats << ", thisExamNumStudents = " << thisExamNumStudents
             << ", roomSeatCapacity = " << roomSeatCapacity << endl;
        cout << "Room " << rk << " exams: " << endl;
        for (auto const &p : roomExams)
            cout << "exam: " << p.first << " - # students " << p.second << endl;
        cout << "Done removing exams from roomExams" << endl;
#endif

    }

    //-
    // Verify the No-Conflicts hard constraint within period tj
    // No-Conflicts: Conflicting exams cannot be assigned to the same period. (As usual,
    //   two exams are said to conflict whenever they have some student taking them both.)
    //-
    // For each exam of period tj do
    for (int exam_j = 0; exam_j < _chrom.getNumExams(); ++exam_j) {
        // If exam exam_j is scheduled in period tj
        if (exam_j != ei && timetableCont.isExamScheduled(exam_j, tj)) {
            // Obtain conflicts by consulting the conflicting matrix
            int n = conflictMatrix.getVal(exam_j, ei);
            if (n > 0) {
                // Get exam room
                int conflictingExamRoom = timetableCont.getRoom(exam_j, tj);
                // Add exam to unscheduled exams
                variables.push_back(std::make_tuple(exam_j, tj, conflictingExamRoom));
            }
        }
    }

#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG
    cout << "variables after verifying Room-Occupancy + No-Conflicts:" << endl;
    for (VariableValueTuple v : variables) {
        // Get exam, period and room
        int ei = std::get<0>(v);;
        int tj = std::get<1>(v);
        int rk = std::get<2>(v);
        cout << "ei = " << ei << ", tj = " << tj << ", rk = " << rk << endl;
    }
#endif

    //-
    // Verify the Period-Related hard constraint
    // Period-Related: All of a set of time-ordering requirements between pairs of exams
    //   are obeyed. Specifically, for any pair (e1, e2) of exams, one can specify any of:
    //   - ‘AFTER’: e1 must take place strictly after e2,
    //   - ‘EXAM_COINCIDENCE’: e1 must take place at the same time as e2,
    //   - ‘EXCLUSION’: e1 must not take place at the same time as e2.
    //-
    buildPeriodRelatedHardConflicts(_chrom, ei, tj, variables);

#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG
    cout << "variables after verifying Period-Related:" << endl;
    for (VariableValueTuple v : variables) {
        // Get exam, period and room
        int ei = std::get<0>(v);;
        int tj = std::get<1>(v);
        int rk = std::get<2>(v);
        cout << "ei = " << ei << ", tj = " << tj << ", rk = " << rk << endl;
    }
#endif

    //-
    // Verify the Room-Related hard constraint
    // Room-Related: Room requirements are obeyed. Specifically, for any exam one can
    //   disallow the usual sharing of rooms and specify
    //   - ‘EXCLUSIVE’: Exam e must take place in a room on its own.
    //-
    buildRoomRelatedConflicts(_chrom, ei, tj, rk, variables);

#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG
    cout << "variables after verifying Room-Related:" << endl;
    for (VariableValueTuple v : variables) {
        // Get exam, period and room
        int ei = std::get<0>(v);;
        int tj = std::get<1>(v);
        int rk = std::get<2>(v);
        cout << "ei = " << ei << ", tj = " << tj << ", rk = " << rk << endl;
    }
#endif

    // Apply unique to variable vector to find the new end without duplicate.
    // Then remove duplicates
    std::sort(variables.begin(), variables.end());
    auto it = std::unique(variables.begin(), variables.end());
    variables.resize(it-variables.begin());

#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG
    cout << "final variables to return from hardConflicts method:" << endl;
    for (VariableValueTuple v : variables) {
        // Get exam, period and room
        int ei = std::get<0>(v);;
        int tj = std::get<1>(v);
        int rk = std::get<2>(v);
        cout << "ei = " << ei << ", tj = " << tj << ", rk = " << rk << endl;
    }
#endif

    return variables;
}






/**
 * @brief hardConflicts
 * @param _A
 * @param _a
 * @return
 */
template <typename EOT>
std::vector<typename GCHeuristics<EOT>::VariableValueTuple> GCHeuristics<EOT>
    ::hardConflictsWithoutAfterConstraint(EOT &_chrom, int _A, i2tuple const &_a) {
    //
    // The method hardConflicts returns a subset gamma of the solution sigma,
    // so that there are no hard constraints violated between the remaining
    // assignments sigma\gamma and the new assignment {_A/_a} of the selected
    // variable _A with the selected value _a.
    //
    //==
    // When removing conflicting exams, one should consider the RELAXABLE-HARD constraints:
    //
    // No-Conflicts: Conflicting exams cannot be assigned to the same period. (As usual,
    //   two exams are said to conflict whenever they have some student taking them both.)
    // Room-Occupancy: For every room and period no more seats are used than are available
    //   for that room.
    // [PRE-CONDITION ALREADY VERIFIED IN SELECT_VALUE METHOD]
    // Period-Utilisation: No exam assigned to a period uses more time than available for
    //   that period.
    // Period-Related: All of a set of time-ordering requirements between pairs of exams
    //   are obeyed. Specifically, for any pair (e1, e2) of exams, one can specify any of:
    //   - ‘AFTER’: e1 must take place strictly after e2,
    //   - ‘EXAM_COINCIDENCE’: e1 must take place at the same time as e2,
    //   - ‘EXCLUSION’: e1 must not take place at the same time as e2.
    // Room-Related: Room requirements are obeyed. Specifically, for any exam one can
    //   disallow the usual sharing of rooms and specify
    //   - ‘EXCLUSIVE’: Exam e must take place in a room on its own.
    //==
    // Vector containing variables to return
    std::vector<VariableValueTuple> variables;
    // Timetable container
    TimetableContainer &timetableCont = _chrom.getTimetableContainer();
    // Conflict matrix
    IntMatrix const &conflictMatrix = _chrom.getConflictMatrix();
    // Exam vector
    auto const& examVector = _chrom.getExamVector();
    // Get exam, period and room
    int ei = _A;
    int tj = std::get<0>(_a);
    int rk = std::get<1>(_a);



#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG_ROOM
        cout << "hardConflicts" << endl;
        cout << "ei " << ei << endl;
        cout << "tj " << tj << endl;
        cout << "rk " << rk << endl;
        // Room info
//        cout << "Room info:" << endl;
//        auto const& roomVector = chrom->getRoomVector();
//        for (int i = 0; i < roomVector.size(); ++i) {
//            cout << "r" << i << ": ";
//            for (int period = 0; period < chrom->getNumPeriods(); ++period) {
//                // Get number of currently occupied seats for room i
//                int numOccupiedSeats = roomVector[i]->getNumOccupiedSeats(period);
//                // Get seat capacity for room _rk
//                int roomSeatCapacity = roomVector[i]->getCapacity();
//                // numOccupiedSeats / roomSeatCapacity
//                cout << numOccupiedSeats << ":" << roomSeatCapacity << " ";
//            }
//            cout << endl;
//        }
//        cin.get();
#endif


    //-
    // Verify the Room-Occupancy hard constraint within period tj and room rk
    // Room-Occupancy: For every room and period no more seats are used than are available
    //   for that room.
    //-
    if (!_chrom.verifyRoomOccupancyConstraint(ei, tj, rk)) {
        // Remove the minimum set of conflicting exams that prevent ei from being allocated to room rk
        //
///
/// TODO - COULD BE OPTIMIZED - MAINTAIN IN AN AUXILIARY STRUCTURE EXAMS SORTED
///
        // Determine exams allocated to room rk in period tj
        vector<pair<int,int> > roomExams = _chrom.getRoomExams(tj, rk);
        // Get number of students for exam _ei
        int thisExamNumStudents = examVector[ei]->getNumStudents();
        // Get room vector
        auto const &roomVector = _chrom.getRoomVector();
        // Get scheduled rooms vector
        auto const &scheduledRoomsVector = _chrom.getScheduledRoomsVector();
        // Get number of currently occupied seats for room _rk
        int numOccupiedSeats = scheduledRoomsVector[rk].getNumOccupiedSeats(tj);
        // Get seat capacity for room _rk
        int roomSeatCapacity = roomVector[rk]->getCapacity();

#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG_ROOM
        cout << "numOccupiedSeats = " << numOccupiedSeats << ", thisExamNumStudents = " << thisExamNumStudents
             << ", roomSeatCapacity = " << roomSeatCapacity << endl;
        cout << "Room " << rk << " exams: " << endl;
        for (auto const &p : roomExams)
            cout << "exam: " << p.first << " - # students " << p.second << endl;
#endif

        while (roomExams.size() > 0 && numOccupiedSeats + thisExamNumStudents > roomSeatCapacity) {

#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG_ROOM
            cout << "before removing in roomExams..." << endl;
#endif
            // Remove exam with greatest # students allocated to room rk
            // Remove from roomExams
            int largestExam = roomExams.back().first;
            int numStudents = roomExams.back().second;
#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG
//            cout << "largestExam = " << largestExam << endl;
//            cout << "numSeats = " << numSeats << endl;
//            cin.get();
#endif
            roomExams.pop_back();
#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG_ROOM
            cout << "removed exam with num students = " << numStudents << endl;
#endif
            // Add exam to unscheduled exams
            variables.push_back(std::make_tuple(largestExam, tj, rk));
            // Decrement # occupied seats
            numOccupiedSeats -= numStudents;
        }
#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG_ROOM
        cout << "After removing exams from roomExams:" << endl;
        // Get updated numOccupiedSeats
        numOccupiedSeats = scheduledRoomsVector[rk].getNumOccupiedSeats(tj);
        // Not necessary reading these two values because they didn't change...
        thisExamNumStudents = examVector[ei]->getNumStudents();
        roomSeatCapacity = roomVector[rk]->getCapacity();
        cout << "numOccupiedSeats = " << numOccupiedSeats << ", thisExamNumStudents = " << thisExamNumStudents
             << ", roomSeatCapacity = " << roomSeatCapacity << endl;
        cout << "Room " << rk << " exams: " << endl;
        for (auto const &p : roomExams)
            cout << "exam: " << p.first << " - # students " << p.second << endl;
        cout << "Done removing exams from roomExams" << endl;


#endif

    }

#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG
    cout << "variables after verifying Room-Occupancy:" << endl;
    for (VariableValueTuple v : variables) {
        // Get exam, period and room
        int ei = std::get<0>(v);;
        int tj = std::get<1>(v);
        int rk = std::get<2>(v);
        cout << "ei = " << ei << ", tj = " << tj << ", rk = " << rk << endl;
    }
#endif

    //-
    // Verify the No-Conflicts hard constraint within period tj
    // No-Conflicts: Conflicting exams cannot be assigned to the same period. (As usual,
    //   two exams are said to conflict whenever they have some student taking them both.)
    //-
    // For each exam of period tj do
    for (int exam_j = 0; exam_j < _chrom.getNumExams(); ++exam_j) {
        // If exam exam_j is scheduled in period tj
        if (exam_j != ei && timetableCont.isExamScheduled(exam_j, tj)) {
            // Obtain conflicts by consulting the conflicting matrix
            int n = conflictMatrix.getVal(exam_j, ei);
            if (n > 0) {
                // Get exam room
                int conflictingExamRoom = timetableCont.getRoom(exam_j, tj);
                // Add exam to unscheduled exams
                variables.push_back(std::make_tuple(exam_j, tj, conflictingExamRoom));
            }
        }
    }

#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG
    cout << "variables after verifying Room-Occupancy + No-Conflicts:" << endl;
    for (VariableValueTuple v : variables) {
        // Get exam, period and room
        int ei = std::get<0>(v);;
        int tj = std::get<1>(v);
        int rk = std::get<2>(v);
        cout << "ei = " << ei << ", tj = " << tj << ", rk = " << rk << endl;
    }
#endif
/*
    //-
    // Verify the Period-Related hard constraint
    // Period-Related: All of a set of time-ordering requirements between pairs of exams
    //   are obeyed. Specifically, for any pair (e1, e2) of exams, one can specify any of:
    //   - ‘AFTER’: e1 must take place strictly after e2,
    //   - ‘EXAM_COINCIDENCE’: e1 must take place at the same time as e2,
    //   - ‘EXCLUSION’: e1 must not take place at the same time as e2.
    //-
//    buildPeriodRelatedHardConflicts(ei, tj, variables);
*/

      buildPeriodRelatedExclusionHardConflicts(_chrom, ei, tj, variables);

#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG
    cout << "variables after verifying Period-Related:" << endl;
    for (VariableValueTuple v : variables) {
        // Get exam, period and room
        int ei = std::get<0>(v);;
        int tj = std::get<1>(v);
        int rk = std::get<2>(v);
        cout << "ei = " << ei << ", tj = " << tj << ", rk = " << rk << endl;
    }
#endif

    //-
    // Verify the Room-Related hard constraint
    // Room-Related: Room requirements are obeyed. Specifically, for any exam one can
    //   disallow the usual sharing of rooms and specify
    //   - ‘EXCLUSIVE’: Exam e must take place in a room on its own.
    //-
    buildRoomRelatedConflicts(_chrom, ei, tj, rk, variables);

#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG
    cout << "variables after verifying Room-Related:" << endl;
    for (VariableValueTuple v : variables) {
        // Get exam, period and room
        int ei = std::get<0>(v);;
        int tj = std::get<1>(v);
        int rk = std::get<2>(v);
        cout << "ei = " << ei << ", tj = " << tj << ", rk = " << rk << endl;
    }
#endif

    // Apply unique to variable vector to find the new end without duplicate.
    // Then remove duplicates
    std::sort(variables.begin(), variables.end());
    auto it = std::unique(variables.begin(), variables.end());
    variables.resize(it-variables.begin());

#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG
    cout << "final variables to return from hardConflicts method:" << endl;
    for (VariableValueTuple v : variables) {
        // Get exam, period and room
        int ei = std::get<0>(v);;
        int tj = std::get<1>(v);
        int rk = std::get<2>(v);
        cout << "ei = " << ei << ", tj = " << tj << ", rk = " << rk << endl;
    }
#endif

    return variables;
}




/**
 * @brief init
 * @param _timetableProblemData
 * @param _chrom
 */
template <typename EOT>
void GCHeuristics<EOT>::init(const TimetableProblemData *_timetableProblemData, EOT &_chrom) {
    // Register timetable problem data
    timetableProblemData = _timetableProblemData;
    // N is the number of vertices
    numExams = _chrom.getNumExams();
    // Number of periods
    numPeriods = _chrom.getNumPeriods();

//    // Register timetable problem data
//    timetableProblemData = &_timetableProblemData;
    //
    // We use the ParadisEO random generator eoRng which is reseed on initialization.
    //
    // Initialise EO random generator
    rng.reseed(static_cast<uint32_t>(time(0)));
#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG
    cout << "numExams = " << numExams << endl;
    cout << "numPeriods = " << numPeriods << endl;
#endif
    // Clear CBS structure contents
    CBS.clear();
}





/**
 * @brief initialiseVertexPriorityQueue
 * @param _pq
 * @param _examsAvailablePeriodsList
 */
template <typename EOT>
void GCHeuristics<EOT>::initialiseVertexPriorityQueue(VertexPriorityQueue &_pq, vector<vector<int> > &_examsAvailablePeriodsList) {

    //    1.1 Create a priority queue with the exams sorted randomly. Only exams that have the 'After' harder constraint
    //        have less available periods and are scheduled first. The others exams have, initially, all available periods.
    //
    // Exam's vector. Exams start at index 0.
    vector<int> exams(numExams);
    for (int ei = 0; ei < numExams; ++ei)
        exams[ei] = ei;
    // Shuffle exams

/// TODO:
///
/// USING RANDOM SEED GENERATE THE SAME SOLUTION FOR ALL INDIVIDUALS OF THE POPULATION
///
//    std::srand(std::time(0)); // Seed random generator used in random_shuffle
    std::random_shuffle(exams.begin(), exams.end());

#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG
    cout << "Random exams to insert into the priority queue: " << endl;
    copy(exams.begin(), exams.end(), ostream_iterator<int>(cout, " "));
    cout << endl;
#endif

    // Initially all periods are feasible
    vector<int> allPeriods(numPeriods);
    for (int p = 0; p < numPeriods; ++p)
        allPeriods[p] = p; // Periods indexes

#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG
    cout << "allPeriods vector: ";
    copy(allPeriods.begin(), allPeriods.end(), ostream_iterator<int>(cout, " "));
    cout << endl;
#endif

    // Set available periods of all unscheduled exams equal to T, where T is the max. number of periods.
    for (int exam_i = 0; exam_i < numExams; ++exam_i) {
        _examsAvailablePeriodsList[exam_i] = allPeriods;
    }

    //
    // Update period availability according to the hard constraints.
    // In this phase only the 'After' hard constraint is observed, because it limits the exams's period availability.
    //
    // Define a Union-Find structure for maintaining exams dependencies relating to the 'After' hard constraint
    vector<int> unionFind(numExams);
    for (int exami = 0; exami < numExams; ++exami)
        unionFind[exami] = exami; // All exams have no parent

    // Obtain "AFTER" hard constraints and fill the union-find structure
    vector<boost::shared_ptr<Constraint> > const& hardConstraints = timetableProblemData->getHardConstraints();
    for (auto & sharedPtr : hardConstraints) {
        AfterConstraint* ptrConstr = dynamic_cast<AfterConstraint*>(sharedPtr.get());
        if (ptrConstr != nullptr) {
//            cout << "Found AfterConstraint: e1 = " << ptrConstr->getE1() << ", e2 = " << ptrConstr->getE2() << endl;
            unionFind[ptrConstr->getE2()] = ptrConstr->getE1();
        }
    }

    int count;
    int ei;
    // Now, determine the period availability using the union-find
    for (int exam_i = 0; exam_i < numExams; ++exam_i) {
//        cout << "exam_i = " << exam_i << endl;

        count = 0; // Number of infeasible periods
        ei = exam_i;
        while (ei != unionFind[ei]) {
            ei = unionFind[ei];
            ++count; // Traverse union-find tree
        }

//        cout << "count = " << count << endl;

        // Remove infeasible periods
        // E.g. If e2 -> e1 -> e3 then e2 has T-2 available periods, e1 has T-1 available periods,
        // and e3 has T available periods.
        while (count > 0) {
            int size = _examsAvailablePeriodsList[exam_i].size();
            // Remove last period
            auto lastItr = _examsAvailablePeriodsList[exam_i].begin() + (size-1);
            _examsAvailablePeriodsList[exam_i].erase(lastItr);
            // Decrement counter
            --count;
        }
    }

    // Insert exams and respective priority (number of feasible available periods) in the priority queue
    for (int exam_i = 0; exam_i < numExams; ++exam_i) {
        _pq.push(exams[exam_i], _examsAvailablePeriodsList[exams[exam_i]].size());
//        _pq.push(exams[exam_i], 0);
    }

#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG
    for (int exami = 0; exami < numExams; ++exami) {
        cout << "Exam " << exami << ", # available periods " << _examsAvailablePeriodsList[exami].size() << endl;
        for (auto const& b : _examsAvailablePeriodsList[exami])
            cout << b << " ";
        cout << endl;
    }
    cout << endl;
//    cout << "Press any key..." << endl;
//    cin.get();
#endif
}






/**
 * @brief scheduleExam
 * @param _ei
 * @param _tj
 * @param _rk
 */
template <typename EOT>
void GCHeuristics<EOT>::scheduleExam(EOT &_chrom, int _ei, i2tuple const &_value) {
    // Get period and room
    int tj = std::get<0>(_value);
    int rk = std::get<1>(_value);
    //
    // Schedule exam 'ei' in time slot 'tj' and room 'rk'
    //
    // Get TimetableContainer object
    TimetableContainer &timetableCont = _chrom.getTimetableContainer();
    //---
    // We have to schedule temporarily exam 'ei' in order to verify the constraints
    timetableCont.scheduleExam(_ei, tj, rk);
    //---
}






/**
 * @brief selectValue
 * @param _A
 * @param _hardConflicts
 * @param _domainA
 * @return
 */
template <typename EOT>
typename GCHeuristics<EOT>::i2tuple GCHeuristics<EOT>::selectValue(EOT &_chrom, int _A,
                                                                   std::vector<GCHeuristics<EOT>::VariableValueTuple> &_hardConflicts,
                                                                   std::vector<i2tuple> const &_domainA) {
    //==
    // Muller's algorithm
    //
    // procedure selectValue(σ, A)
    //   bestValues = {}; bestNrConfs = 0;
    //   for each a ∈ D_A do
    //     nrConfs = 0;
    //     for each B/b ∈ conflicts(σ, A, a) do
    //       nrConfs += 1+CBS[A=a -> B≠b];
    //     end for
    //     if (bestValues is empty) or (bestNrConfs > nrConfs) then
    //       bestValues = {a}; bestNrConfs = nrConfs;
    //     else if (bestNrConfs == nrConfs) then
    //       bestValues = bestValues ∪ {a};
    //     end if
    //   end for
    //   a = randomly selected a value from bestValues;
    //   for each B/b ∈ conflicts(σ, A, a) do
    //     CBS[A=a -> B≠b]++;
    //   end for
    //   return a;
    // end procedure
    //==
    //   bestValues = {}; bestNrConfs = Inf;
    std::vector<i2tuple> bestValues;
    int bestNrConfs = 0;
    //   for each a ∈ D_A do
    for(i2tuple a : _domainA)
    {
        // If period verifies period utilisation and room capacity constraints
        VariableValueTuple variable = std::make_tuple(_A, std::get<0>(a), std::get<1>(a));
        if (feasiblePeriod(_chrom, variable)) {
            //     nrConfs = 0;
            int nrConfs = 0;
            //     for each B/b ∈ conflicts(σ, A, a) do
            //       nrConfs += 1+CBS[A=a -> B≠b];
            //     end for
            // Get conflicting exams
            for(VariableValueTuple b : hardConflicts(_chrom, _A, a)) {
                nrConfs += 1 + CBS[std::make_tuple(_A, std::get<0>(a), std::get<1>(a), std::get<0>(b), std::get<1>(b), std::get<2>(b))];
            }
#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG_1
            cout << "nrConfs =" << nrConfs << endl;
#endif
            //     if (bestValues is empty) or (bestNrConfs > nrConfs) then
            //       bestValues = {a}; bestNrConfs = nrConfs;
            //     else if (bestNrConfs == nrConfs) then
            //       bestValues = bestValues ∪ {a};
            //     end if
            if (bestValues.empty() || nrConfs < bestNrConfs) {
                // Clear the vector and add 'a'
                bestValues.clear();
                bestValues.push_back(a);
                bestNrConfs = nrConfs;
            }
            else if (bestNrConfs == nrConfs) {
                bestValues.push_back(a);
            }
        }
    //   end for
    }
    //   a = randomly selected a value from bestValues;
    int idx = rng.uniform(bestValues.size());
    i2tuple a = bestValues[idx];
    //   for each B/b ∈ conflicts(σ, A, a) do
    //     CBS[A=a -> B≠b]++;
    //   end for
    // Get conflicting exams
    _hardConflicts = hardConflicts(_chrom, _A, a);
    for(VariableValueTuple b : _hardConflicts) {
        CBS[std::make_tuple(_A, std::get<0>(a), std::get<1>(a), std::get<0>(b), std::get<1>(b), std::get<2>(b))]++;
    }
    //   return a;
    return a;



    //   a = randomly selected a value from bestValues;

//    i2tuple a;
//    VariableValueTuple variable;
//    for (;;) {
//        int periodIdx = rng.uniform(chrom->getNumPeriods());
//        int roomIdx = rng.uniform(chrom->getNumRooms());
//        a = std::make_tuple(periodIdx, roomIdx);
//        // If period verifies period utilisation and room capacity constraints
//        variable = std::make_tuple(_A, std::get<0>(a), std::get<1>(a));
//        if (feasiblePeriod(variable))
//            break;
//    }
//    _hardConflicts = hardConflicts(_A, a);
//    return a;

}





/**
 * @brief selectVariable
 * @param _pq
 * @return
 */
template <typename EOT>
typename GCHeuristics<EOT>::i2tuple GCHeuristics<EOT>::selectVariable(VertexPriorityQueue &_pq) {
    VertexPriorityQueue::heap_data data = _pq.top();
    // Get exam index
    int ei = data.vertex;
    // Get # available periods for exam 'ei'
    int numAvailablePeriods = data.priority;
    // Remove 'ei' from the priority queue
    _pq.pop();
    // Return tuple
    return std::make_tuple(ei, numAvailablePeriods);
}






/**
 * @brief tryScheduleExam
 * @param _ei
 * @param _tj
 * @param _rk
 * @param _examsAvailablePeriodsList
 * @return
 */
template <typename EOT>
bool GCHeuristics<EOT>::tryScheduleExam(EOT &_chrom, int _ei, int &_tj, int &_rk, vector<vector<int>> &_examsAvailablePeriodsList) {
    //
    // 2.2 - If the available period list for exam 'ei' is not empty, try to schedule 'ei' into feasible period 'tj' and room 'rk'.
    //
    bool examWasScheduled = false;
    bool feasiblePeriodRoom = false;
    // Select random feasible period and room for exam 'ei'. 'tj' and 'rk' are out parameters.
    feasiblePeriodRoom = getFeasiblePeriodRoom(_chrom, _ei, _tj, _rk, _examsAvailablePeriodsList[_ei]);
    if (feasiblePeriodRoom) {
        // Schedule exam 'ei' in time slot 'tj' and room 'rk'
        i2tuple value = std::make_tuple(_tj, _rk);
        scheduleExam(_chrom, _ei, value);
        examWasScheduled = true;
    }
    return examWasScheduled;
}






/**
 * @brief unscheduleConflictingExams
 * @param _pq
 * @param _eiHardConflicts
 * @param _scheduledExams
 * @param _numScheduledExams
 */
template <typename EOT>
void GCHeuristics<EOT>::unscheduleConflictingExams(EOT &_chrom, VertexPriorityQueue &_pq,
                                                   std::vector<VariableValueTuple> const &_eiHardConflicts,
                                                   vector<bool> &_scheduledExams,
                                                   int &_numScheduledExams) {
    // Unschedule all conflicting exams, and put them in the priority queue again
    //
    // 1. Unschedule all conflicting exams
    for (VariableValueTuple b : _eiHardConflicts) {
        // Conflicting exam
        int ej = std::get<0>(b);
#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG_CONFLICTS
        // Get period tj
        int tj = std::get<1>(b);
        // Get room rk
        int rk = std::get<2>(b);
        cout << "[unscheduleConflictingExams]" << endl;
        cout << "ej = " << ej << ", tj = " << tj << ", rk = " << rk << endl;
#endif
        // Unschedule exam
        unscheduleExam(_chrom, ej);
        // Set exam entry to false in the _scheduledExams vector
        _scheduledExams[ej] = false;
        // Decrement # scheduled exams
        --_numScheduledExams;
    }

    // 2. Put conflicting exams in the priority queue again
    for (VariableValueTuple b : _eiHardConflicts) {
        // Conflicting exam
        int ej = std::get<0>(b);
        // Compute ej's priority (number of available periods in the timetable)
        int priority = computePriority(ej);
        // Emplace ej in the priority queue
        _pq.push(ej, priority);
    }
}





/**
 * @brief unscheduleExam
 * @param _ej
 */
template <typename EOT>
void GCHeuristics<EOT>::unscheduleExam(EOT &_chrom, int _ei) {
    // Get scheduled exams vector
    auto const &scheduledExamsVector = _chrom.getScheduledExamsVector();
    // Get scheduled exam
    ScheduledExam const &exam_i = scheduledExamsVector[_ei];
    // Get exam period
    int tj = exam_i.getPeriod();
    // Get exam room
    int rk = exam_i.getRoom();
#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG_ROOM
    cout << "[GCHeuristics<EOT>::unscheduleExam] ei = " << _ei << ", tj = " << tj << ", rk = " << rk << endl;
#endif
    // Get TimetableContainer object
    TimetableContainer &timetableCont = _chrom.getTimetableContainer();
    //---
    // We have to schedule temporarily exam 'ei' in order to verify the constraints
    timetableCont.unscheduleExam(_ei, tj);
    //---
}





/**
 * @brief updatePeriodAvailability
 * @param _ei
 * @param _tj
 * @param _scheduledExams
 * @param _examsAvailablePeriodsList
 * @param _pq
 */
template <typename EOT>
void GCHeuristics<EOT>::updatePeriodAvailability(EOT &_chrom, int _ei, int _tj, std::vector<bool> _scheduledExams,
                                                 vector<vector<int> > &_examsAvailablePeriodsList, VertexPriorityQueue &_pq) {

    // 2.5. Update the period availability for all unscheduled exams according to the 'No-Conflicts' hard constraint.
    //
    // 2.5.1. Mark period 'tj' of all the exams connected to 'ei' as unvailable, because they have common registered students.
    //        The number of available periods is the priority used to sort the priority queue of unscheduled exams.

#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG_GRAPH
   cout << "////////////////////////////////////////////////////////////////////////////////////////" << endl <<
           "//" << endl <<
           "// Get adjacent vertices" << endl <<
           "////////////////////////////////////////////////////////////////////////////////////////" << endl;
#endif
   // Get adjacent vertices
   property_map<AdjacencyList, vertex_index_t>::type index_map = get(vertex_index, _chrom.getExamGraph());
   graph_traits<AdjacencyList>::adjacency_iterator ai, a_end;
   boost::tie(ai, a_end) = adjacent_vertices(_ei, _chrom.getExamGraph());
   for (; ai != a_end; ++ai) {
       // Get adjacent exam
       int ej = get(index_map, *ai);

#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG_GRAPH
       cout << "Adjacent vertex ej = " << ej << endl;
#endif
       if (!_scheduledExams[ej])  {
#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG_GRAPH
           //cout << "ej = " << ej << ", tj = " << tj << endl;
           cout << "Adjacent vertex ej = " << ej << " not scheduled yet" << endl;
#endif
           // If exam ej was not scheduled yet, mark adjacent exam's tj period as unavailable.
           // This is done by removing from available period's vector the period tj
           bool periodRemoved = false;
           // Find and remove period tj from ej's exams periods list
           vector<int>::iterator it;
           for (it = _examsAvailablePeriodsList[ej].begin(); it != _examsAvailablePeriodsList[ej].end(); ++it) {
               if (*it == _tj) {
#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG_GRAPH
                   cout << "Size before removing = " << examsAvailablePeriodsList[ej].size() << endl;
#endif
                   // Remove period tj from ej available periods list
                   _examsAvailablePeriodsList[ej].erase(it);

#ifdef GRAPH_COLOURING_HEURISTIC_DEBUG_GRAPH
                   cout << "Size after removing = " << examsAvailablePeriodsList[ej].size() << endl;
#endif
                   // Update exam ej priority
                   _pq.update(ej, _examsAvailablePeriodsList[ej].size());
                   periodRemoved = true;
                   break;
               }
           }

//                if (!periodRemoved) {
//                    cout << "In method [GCHeuristics<EOT>::saturationDegree] Period tj not removed -> ERROR?" << endl;
////                    cin.get();
//                }

           //copy(examsAvailablePeriodsList[ej].begin(), examsAvailablePeriodsList[ej].end(), ostream_iterator<int>(cout, " "));
       } // if (!scheduled_exams[ej])
   }
}


#endif
