#ifndef ETTPKEMPECHAINHEURISTIC_H
#define ETTPKEMPECHAINHEURISTIC_H

#include "eoFunctor.h"
#include "kempeChain/ETTPKempeChain.h"
#include <boost/unordered_set.hpp>
#include <utils/eoRNG.h>
#include "utils/Common.h"
#include "containers/Matrix.h"
#include "graphColouring/GraphColouringHeuristics.h"
#include <vector>
#include <stdexcept>
#include "containers/TimetableContainerMatrix.h" /// BECAUSE OF REMOVE_EXAM



// For debugging purposes
//#define ETTP_KEMPE_CHAIN_HEURISTIC_DEBUG

//#define ETTP_KEMPE_CHAIN_HEURISTIC_DEBUG_INCREMENTAL // LAST

//#define DEBUG_MODE

//#define DEBUG_MODE_1



//////////////////////////////////////////////////////////
//
// (rf. Demeester paper)
// Concerning the uncapacitated Toronto examination time-
// tabling problem, we apply the Kempe chain based heuris-
// tics. These low-level heuristics perturb feasible solutions
// to the uncapacitated examination timetabling problem,
// without making them infeasible. Suppose a partial solu-
// tion that corresponds to the left hand side of Fig. 1. If
// we want to move exam e1 to time slot tj, the solution
// becomes infeasible, since exam e1 has students in com-
// mon with exams e6, e7, and e8 that are assigned to time
// slot tj. To overcome this, exams e6, e7, and e8 should be
// moved to time slot ti. This process is repeated until all the
// exams that have students in common are assigned to dif-
// ferent time slots. The result is depicted at the right hand
// side of Fig. 1. The Kempe Chain heuristic Uncap1 can be
// seen as moving an exam to another time slot whilst main-
// taining feasibility by repairing any infeasibilities that may
// have been introduced.
//
template <typename EOT>
class ETTPKempeChainHeuristic : public eoUF<EOT&, void> {

private:
    /**
     * @brief The Operators enum
     */
    enum class Operator { RoomMove, ShiftMove, SwapMove, SlotMove };

    /**
     * @brief currentOperator
     */
    Operator currentOperator;

public:
    /**
     * @brief ETTPKempeChainHeuristic Constructor
     */
    ETTPKempeChainHeuristic();

    /**
     * @brief build Create a Kempe chain for a random move
     * @param _sol
     */
    virtual void build(EOT& _sol);

    /**
     * @brief evaluateSolutionMove Evaluate move of solution to the neighbour.
     * This envolves doing:
     *   - move, temporarily, the solution to the neighbour
     *   - record ti and tj final time slots
     *   - evaluate neighbour and record neighbour fitness
     *   - undo solution move
     * @param _sol
     */
    void evaluateSolutionMove(EOT &_sol);

    /**
     * @brief getKempeChain Return Kempe chain object
     * @return
     */
    const ETTPKempeChain<EOT> &getKempeChain() const;

    /**
     * @brief getNeighborFitness Get neighbor fitness
     * @return
     */
    double getNeighborFitness() const;

    /**
     * @brief getNeighborSolutionCost
     * @return Return neighbor cost
     */
    long getNeighborSolutionCost() const;

    /**
     * @brief isFeasibleNeighbour
     * @return
     */
    bool isFeasibleNeighbour() const;


    /**
     * @brief operator () Apply Kempe chain move to solution _sol
     * @param _chrom
     */
    virtual void operator()(EOT& _sol);


    /**
     * @brief doSolutionMove Do solution move
     * @param _sol
     */
    void doSolutionMove(EOT &_sol);

    /**
     * @brief undoSolutionMove Undo solution move
     * @param _sol
     */
    void undoSolutionMove(EOT &_sol);

protected:
    /**
     * @brief apply
     */
    void apply();

    /**
     * @brief doFeasibleExamMove Move exams between time slots in order to maintain feasibility
     * @param _tSource
     * @param _conflictingExamsTsource
     * @param _tDest
     * @param _conflictingExamsTdest
     */
    void doFeasibleExamMove(int _tSource, std::vector<typename GCHeuristics<EOT>::VariableValueTuple> &_conflictingExamsTsource,
                   int _tDest, std::vector<typename GCHeuristics<EOT>::VariableValueTuple> &_conflictingExamsTdest);

    /**
     * @brief getSourceExamHardConflictsDestPeriod
     * @param _examSource
     * @param _examGraph
     * @param _tDest
     * @param _roomDest
     * @param _conflictingExamsTdest
     */
    void getSourceExamHardConflictsDestPeriod(int _examSource, AdjacencyList const &_examGraph,
                                          int _tDest, int _roomDest,
                                          std::vector<typename GCHeuristics<EOT>::VariableValueTuple> &_conflictingExamsTdest) const;

    /**
     * @brief initialiseKempeChain
     * @param _sol
     * @param _ti
     * @param _exami
     * @param _roomi
     * @param _tj
     */
    void initialiseKempeChain(EOT &_sol, int _ti, int _exami, int _roomi, int _tj);

    /**
     * @brief recordFinalPeriodExams Record Ti and Tj period exams after move is made
     * @param _ti
     * @param _tj
     */
    void recordFinalPeriodExams(int _ti, int _tj);

    /**
     * @brief recordOriginalPeriodExams Record source Ti and Tj period exams prior the move
     * @param _ti
     * @param _tj
     */
    void recordOriginalPeriodExams(int _ti, int _tj);

    /**
     * @brief selectRandomExam
     * @param _timetableCont
     * @param _sol
     * @param _ti
     * @param _exami
     * @param _roomi
     */
    void selectRandomExam(TimetableContainer const &_timetableCont, EOT &_sol,
                          int _ti, int &_exami, int &_roomi) const;

    /**
     * @brief selectRandomRoomWithCapacity
     * @param _sol
     * @param _ei
     * @param _tj
     * @param _rk
     * @return room with sufficient capacity for hosting _ei
     */
    bool selectRandomRoomWithCapacity(EOT &_sol, int _ei, int _tj, int &_rk) const;

    /**
     * @brief selectRandomTimeslots
     * @param _timetableCont
     * @param _numPeriods
     * @param _ti
     * @param _tj
     */
    void selectRandomTimeslots(TimetableContainer const &_timetableCont, int _numPeriods, int &_ti, int &_tj) const;

    /**
     * @brief selectRandomTimeslot
     * @param _timetableCont
     * @param _numPeriods
     * @param _ti
     */
    void selectRandomTimeslot(TimetableContainer const &_timetableCont, int _numPeriods, int &_ti) const;

    /**
     * @brief setNeighbourFeasibility
     * @param _value
     */
    void setNeighbourFeasibility(bool _value);


    /**
     * @brief shiftMove
     * @param _sol
     */
    void shiftMove(EOT &_sol);


    /**
     * @brief roomMove
     * @param _sol
     */
    void roomMove(EOT &_sol);


    //
    // Fields
    //
    /**
     * @brief kempeChain The kempe chain relating the current solution to the neighbour solution
     */
    ETTPKempeChain<EOT> kempeChain;
    /**
     * @brief neigborFitness Neighbor fitness
     */
    double neighborFitness;
    /**
     * @brief neighborExamProximityConflicts Sum of exam proximity conflicts
     */
    long neighborSolutionCost;
    /**
     * @brief feasibleNeighbour
     */
    bool feasibleNeighbour;
};



/////////////////////////////////////////////////////////////////
//
// Public members
//
/////////////////////////////////////////////////////////////////



/**
 * @brief ETTPKempeChainHeuristic Constructor
 */
template <typename EOT>
ETTPKempeChainHeuristic<EOT>::ETTPKempeChainHeuristic()
    : neighborFitness(0), neighborSolutionCost(0), feasibleNeighbour(false)
{ }




/**
 * @brief build Create a Kempe chain for a random move
 * @param _sol
 */
template <typename EOT>
void ETTPKempeChainHeuristic<EOT>::build(EOT &_sol)
{
    //
    // Operators based on those published by Yuri Bykov Technical Report 2013
    //
    // In all operators, feasibility is checked and Kempe Chain heuristic is
    // used to repair the infeasibilities. The four operators are selected randomly
    // in equal proportions.
    //
    // 1. Room move - Here a random exam is just moved into a different
    // (randomly chosen) room within the same timeslot.
    //
    // 2. Shift move - Here a random exam is moved into different
    // (randomly chosen) timeslot and room.
    //
    // 3. Swap move - Here the algorithm selects two random exams and
    // swaps their time slots. The rooms are chosen randomly.
    //
    // 4. Slot move - Here two randomly chosen timeslots are interchanged
    // including all their exams and rooms.
    //

    if (rng.flip() < 0.5) {
         // Apply operator 2. Shift move - Here a random exam is moved into different
         // (randomly chosen) timeslot and room.
        shiftMove(_sol);
    }
    else {
        // Apply operator 1. Room move - Here a random exam is just moved into a different
        // (randomly chosen) room within the same timeslot.
        roomMove(_sol);
    }

/// TODO: OTHER OPERATORS
///
///
///


}





/**
 * @brief evaluateSolutionMove Evaluate move of solution to the neighbour.
 * This envolves doing:
 *   - move, temporarily, the solution to the neighbour
 *   - record ti and tj final time slots
 *   - evaluate neighbour and record neighbour fitness
 *   - undo solution move
 * @param _sol
 */
template <typename EOT>
void ETTPKempeChainHeuristic<EOT>::evaluateSolutionMove(EOT &_sol) {

#ifdef DEBUG_MODE
    cout << "In [evaluateSolutionMove] method" << endl;
    cin.get();
#endif

    if (!this->isFeasibleNeighbour()) {
#ifdef ETTP_KEMPE_CHAIN_HEURISTIC_DEBUG_INCREMENTAL
    cout << "Infeasible neighbour as a result of build()" << endl;
//    cin.get();
#endif
        return;
    }

    // Get TimetableContainer object
    TimetableContainer &timetableCont = _sol.getTimetableContainer();
    // Get original solution cost prior moving the solution
    long solutionOriginalCost = _sol.getSolutionCost();

#ifdef ETTP_KEMPE_CHAIN_HEURISTIC_DEBUG_INCREMENTAL
    cout << "solutionOriginalCost = " << solutionOriginalCost << endl;
//    cin.get();
#endif

    // Move, temporarily, the solution to the neighbour.
    // _sol was previously recorded in the kempe chain, so we don't pass it as argument
    apply();


    if (!isFeasibleNeighbour()) {
#ifdef ETTP_KEMPE_CHAIN_HEURISTIC_DEBUG_INCREMENTAL
    cout << "Infeasible neighbour as a result of apply()" << endl;
//    cin.get();
#endif
        return;
    }


#ifdef DEBUG_MODE
    cout << "After invoking apply()" << endl;
//    cin.get();
#endif

    // Record ti and tj final time slots
    recordFinalPeriodExams(kempeChain.getTi(), kempeChain.getTj());

    /////////////////////////////////////////////////////////////////////
    /// NON-OPTIMIZED AND NON INCREMENTAL
    ///
    ///
    /// TODO - MOVE COINCIDENT EXAMS
    ///

    // Verify period Ti Period-utilisation and After constraints after the move
    for (auto const &examRoomTuple : kempeChain.getFinalTiPeriodExams()) {
        // Get exam
        int exam = std::get<0>(examRoomTuple);
        // Get room
        int room = std::get<1>(examRoomTuple);

/// TODO - IMPLEMENT A METHOD THAT ASSUMES THE EXAM IS ALREADY SCHEDULED
///
///
        // In order to verify the constraints, is necessary to unschedule the exam
        timetableCont.unscheduleExam(exam, kempeChain.getTi());
        // Verify constraints
        if (!(_sol.verifyPeriodUtilisationConstraint(exam, kempeChain.getTi())
               && _sol.verifyPeriodRelatedConstraints(exam, kempeChain.getTi()) )) {
#ifdef DEBUG_MODE
            cout << "Period-utilisation and After constraints were not verified" << endl;
#endif
            setNeighbourFeasibility(false);
        }
        // Schedule the exam again
        timetableCont.scheduleExam(exam, kempeChain.getTi(), room);
    }
    if (isFeasibleNeighbour()) {
        // Verify period Tj Period-utilisation and After constraints after the move
        for (auto const &examRoomTuple : kempeChain.getFinalTjPeriodExams()) {
            // Get exam
            int exam = std::get<0>(examRoomTuple);
            // Get room
            int room = std::get<1>(examRoomTuple);

    /// TODO - IMPLEMENT A METHOD THAT ASSUMES THE EXAM IS ALREADY SCHEDULED
    ///
    ///
            // In order to verify the constraints, is necessary to unschedule the exam
            timetableCont.unscheduleExam(exam, kempeChain.getTj());
            // Verify constraints
            if (!( _sol.verifyPeriodUtilisationConstraint(exam, kempeChain.getTj())
                   && _sol.verifyPeriodRelatedConstraints(exam, kempeChain.getTj()) )) {
    #ifdef DEBUG_MODE
                cout << "Period-utilisation and After constraints were not verified" << endl;
    #endif
                setNeighbourFeasibility(false);
            }
            // Schedule the exam again
            timetableCont.scheduleExam(exam, kempeChain.getTj(), room);
        }
    }
/*
    //
    // Non-incremental
    //
    if (isFeasibleNeighbour()) {
        // Compute chromosome cost non-incrementally
        _sol.computeCost();
        // Set chromosome fitness
//        _sol.fitness(_sol.getSolutionCost());

        // Record neighbour fitness
        neighborFitness = _sol.getSolutionCost();
        // Record neighbour exam proximity conflicts
        neighborSolutionCost = _sol.getSolutionCost();
    }


    //
    // Evaluate neighbour (incremental evaluation) and record neighbour fitness
    //
    // Undo solution move before computeExamProximityConflictsIncremental() - pre-condition
    undoSolutionMove(_sol);
    _sol.setSolutionCost(solutionOriginalCost);
*/

    if (isFeasibleNeighbour()) {
        //
        // Incremental evaluation
        //
        // Evaluate neighbour (incremental evaluation) and record neighbour fitness
        //
        // Undo solution move before computeExamProximityConflictsIncremental() - pre-condition
        undoSolutionMove(_sol);
        _sol.setSolutionCost(solutionOriginalCost);
        // Compute new solution exam proximity conflicts considering the move to the neighbor.
        // Perform an incremental evaluation
        _sol.computeSolutionCostIncremental(kempeChain);

        //    _sol.validate();

        // Record neighbour fitness
        neighborFitness = _sol.getSolutionCost();
        // Record neighbour exam proximity conflicts
        neighborSolutionCost = _sol.getSolutionCost();
    }

    // Undo solution move
    undoSolutionMove(_sol);
    // Reset exam proximity conflicts
    _sol.setSolutionCost(solutionOriginalCost);

    // Could not show fitness here, eval wasn't invoked yet
//    cout << "sol fitness = " << _sol.fitness() << endl;
//    cout << "neighbour fitness = " << neighborFitness << endl;
#ifdef ETTP_KEMPE_CHAIN_HEURISTIC_DEBUG_INCREMENTAL
    cout << "sol cost = " << _sol.getSolutionCost() << endl;
    cout << "neighbour cost = " << neighborSolutionCost << endl;
#endif

}






/**
 * @brief getKempeChain Return Kempe chain object
 * @return
 */
template <typename EOT>
ETTPKempeChain<EOT> const &ETTPKempeChainHeuristic<EOT>::getKempeChain() const
{
    return kempeChain;
}



/**
 * @brief getNeighborFitness Get neighbor fitness
 * @return
 */
template <typename EOT>
double ETTPKempeChainHeuristic<EOT>::getNeighborFitness() const {
    return neighborFitness;
}



/**
 * @brief getNeighborSolutionCost
 * @return Return neighbor cost
 */
template <typename EOT>
long ETTPKempeChainHeuristic<EOT>::getNeighborSolutionCost() const {
    return neighborSolutionCost;
}




/**
 * @brief isFeasibleNeighbour
 * @return
 */
template <typename EOT>
bool ETTPKempeChainHeuristic<EOT>::isFeasibleNeighbour() const {
    return feasibleNeighbour;
}




/**
 * @brief operator() Move current solution _sol to this neighbour using
 * information maintained in the KempeChain structure
 */
template <typename EOT>
void ETTPKempeChainHeuristic<EOT>::operator()(EOT &_sol) {
    //
    // Apply Kempe chain heuristic to move exami from time slot ti to time slot tj,
    // in a feasible way.
    // If the solution becomes infeasible,
    // because exam ei has students in common with exams ej, ek, ...,
    // that are assigned to time slot tj, one have to move exams
    // ej, ek, ..., to time slot ti. This process is repeated until all the
    // exams that have students in common are assigned to different time slots.
    //
    doSolutionMove(_sol);
}




/**
 * @brief doSolutionMove Do solution move
 * @param _sol
 */
template <typename EOT>
void ETTPKempeChainHeuristic<EOT>::doSolutionMove(EOT &_sol) {
    // Get timetable container
    auto &timetableCont = _sol.getTimetableContainer();
    // Do solution move by fill solution time slots ti and tj to final values
    // Remove Ti actual contents
    timetableCont.removeAllPeriodExams(kempeChain.getTi());
    if (currentOperator == Operator::ShiftMove) {
        // Remove Tj actual contents
        timetableCont.removeAllPeriodExams(kempeChain.getTj());
    }
    // Insert final Ti and Tj exams
    auto const &finalTiExams = kempeChain.getFinalTiPeriodExams();
    auto const &finalTjExams = kempeChain.getFinalTjPeriodExams();

    // Insert Ti exams
    for (auto const &examRoomTuple : finalTiExams) {
        // Get exam
        int exam = std::get<0>(examRoomTuple);
        // Get room
        int room = std::get<1>(examRoomTuple);
        // Schedule exam into period ti and room
        timetableCont.scheduleExam(exam, kempeChain.getTi(), room);
    }
    if (currentOperator == Operator::ShiftMove) {
        // Insert Tj exams
        for (auto const &examRoomTuple : finalTjExams) {
            // Get exam
            int exam = std::get<0>(examRoomTuple);
            // Get room
            int room = std::get<1>(examRoomTuple);
            // Schedule exam into period ti and room
            timetableCont.scheduleExam(exam, kempeChain.getTj(), room);
        }
    }
    // ADDED 28-JAN-2016
//    _sol.setSolutionCost(neighborSolutionCost);
}




/**
 * @brief undoSolutionMove Undo solution move
 * @param _sol
 */
template <typename EOT>
void ETTPKempeChainHeuristic<EOT>::undoSolutionMove(EOT &_sol) {
    // Get timetable container
    auto &timetableCont = _sol.getTimetableContainer();
    // Undo solution move by fill solution time slots ti and tj to source values
    // Remove Ti actual contents
    timetableCont.removeAllPeriodExams(kempeChain.getTi());
    if (currentOperator == Operator::ShiftMove) {
        // Remove Tj actual contents
        timetableCont.removeAllPeriodExams(kempeChain.getTj());
    }
    // Insert source Ti and Tj exams
    auto const &srcTiExams = kempeChain.getOriginalTiPeriodExams();
    auto const &srcTjExams = kempeChain.getOriginalTjPeriodExams();

    // Insert Ti exams
    for (auto const &examRoomTuple : srcTiExams) {
        // Get exam
        int exam = std::get<0>(examRoomTuple);
        // Get room
        int room = std::get<1>(examRoomTuple);
        // Schedule exam into period ti and room
        timetableCont.scheduleExam(exam, kempeChain.getTi(), room);
    }
    if (currentOperator == Operator::ShiftMove) {
        // Insert Tj exams
        for (auto const &examRoomTuple : srcTjExams) {
            // Get exam
            int exam = std::get<0>(examRoomTuple);
            // Get room
            int room = std::get<1>(examRoomTuple);
            // Schedule exam into period ti and room
            timetableCont.scheduleExam(exam, kempeChain.getTj(), room);
        }
    }
}




/////////////////////////////////////////////////////////////////
//
// Protected members
//
/////////////////////////////////////////////////////////////////



/**
 * @brief apply
 */
template <typename EOT>
void ETTPKempeChainHeuristic<EOT>::apply() {
    //
    // Apply Kempe chain heuristic to move exami from time slot ti to time slot tj,
    // in a feasible way.
    // If the solution becomes infeasible,
    // because exam ei has students in common with exams ej, ek, ...,
    // that are assigned to time slot tj, one have to move exams
    // ej, ek, ..., to time slot ti. This process is repeated until all the
    // exams that have students in common are assigned to different time slots.
    //
    // Vector containing conflicting exams in time slot ti
    std::vector<typename GCHeuristics<EOT>::VariableValueTuple> conflictingExamsTi;
    // Vector containing conflicting exams in time slot tj
    std::vector<typename GCHeuristics<EOT>::VariableValueTuple> conflictingExamsTj;
    // Push tuple (exami, ti, roomi) into the conflictingExamsTi vector
    conflictingExamsTi.push_back(std::make_tuple(kempeChain.getEi(), kempeChain.getTi(), kempeChain.getRi()));
    // Move exams between time slots in order to maintain feasibility
    doFeasibleExamMove(kempeChain.getTi(), conflictingExamsTi,
                       kempeChain.getTj(), conflictingExamsTj);
}





/**
 * @brief doFeasibleExamMove Move exams between time slots in order to maintain feasibility
 * @param _tSource
 * @param _conflictingExamsTsource
 * @param _tDest
 * @param _conflictingExamsTdest
 */
template <typename EOT>
void ETTPKempeChainHeuristic<EOT>::doFeasibleExamMove(
        int _tSource, std::vector<typename GCHeuristics<EOT>::VariableValueTuple> &_conflictingExamsTsource,
        int _tDest, std::vector<typename GCHeuristics<EOT>::VariableValueTuple> &_conflictingExamsTdest) {

    // Get solution reference
    EOT &sol = this->kempeChain.getSolution();
    // Get exam graph reference
    AdjacencyList const &examGraph = sol.getExamGraph();
    // Timetable container
    TimetableContainer &timetableCont = sol.getTimetableContainer();
//    // Get scheduled rooms vector
//    auto const &scheduledRoomsVector = sol.getScheduledRoomsVector();
//    // Get exam vector
//    auto const &examVector = sol.getExamVector();

    // Get source exam from conflict vector
    int examSource = std::get<0>(_conflictingExamsTsource.back());

    std::vector<typename GCHeuristics<EOT>::VariableValueTuple> finalExamsTsource, finalExamsTdest;


    std::vector<TimetableContainer::ExamRoomTuple> periodTiExams, periodTjExams;


    int randomDestRoom;
    ///////////////////////////////////////////////////////////////////////////////////
    // If selected operator is RoomMove
    if (currentOperator == Operator::RoomMove) {
        // Get source room
        int sourceRoom = std::get<2>(_conflictingExamsTsource.back());
///
/// TODO - Determine feasible rooms
///
///
///
        // Get room vector
        auto const &roomVector = sol.getRoomVector();
        // Get scheduled room vector
        auto const &scheduledRoomsVector = sol.getScheduledRoomsVector();
        int capacitySourceRoom, capacityDestRoom;
        int numOccupiedSeatsSourceRoom, numOccupiedSeatsDestRoom;

        // Source room capacity
        capacitySourceRoom = roomVector[sourceRoom]->getCapacity();
        // Num of occupied seats in source room
        numOccupiedSeatsSourceRoom = scheduledRoomsVector[sourceRoom].getNumOccupiedSeats(_tSource);

        int numTries = 0;
        do {
#ifdef DEBUG_MODE
            cout << "Getting feasible room..." << endl;
#endif

            // Generate random room index
            randomDestRoom = rng.uniform(sol.getNumRooms());

            // Dest room capacity
            capacityDestRoom = roomVector[randomDestRoom]->getCapacity();
            // Num of occupied seats in dest room
            numOccupiedSeatsDestRoom = scheduledRoomsVector[randomDestRoom].getNumOccupiedSeats(_tSource);

            if (++numTries > 3) {
#ifdef DEBUG_MODE
                cout << "\tNo feasible room found." << endl;
#endif
                // Set solution to be unfeasible
                setNeighbourFeasibility(false);
                return;
            }
        }
        while (randomDestRoom == sourceRoom ||
               numOccupiedSeatsDestRoom > capacitySourceRoom || numOccupiedSeatsSourceRoom > capacityDestRoom ||
               !sol.verifyRoomCapacity(examSource, randomDestRoom));

        // Get dest room
        int destRoom = randomDestRoom;
        // Record in kempe chain
        kempeChain.setRj(destRoom);

#ifdef DEBUG_MODE_1
        cout << endl << endl << "In [doFeasibleExamMove] method:" << endl;
        cout << "Exam to move: " <<  examSource << "-" << sourceRoom << endl;

        // Get periodTi exams
        periodTiExams = timetableCont.getPeriodExams(kempeChain.getTi());

        cout << "Period Ti = " << kempeChain.getTi() << ": " <<endl;
        // For each exam of periodTiexams do
        for (auto const& examRoomTuple : periodTiExams)
        {
            // Get exam_id
            int exam_id = std::get<0>(examRoomTuple);
            // Get room_id
            int room_id = std::get<1>(examRoomTuple);
            cout << exam_id << "-" << room_id << ", ";
        }
        cout << endl;

        // Get periodTj exams
        periodTjExams = timetableCont.getPeriodExams(kempeChain.getTj());
        cout << "Period Tj = " << kempeChain.getTj() << ":" << endl;
        // For each exam of periodTiexams do
        for (auto const& examRoomTuple : periodTjExams)
        {
            // Get exam_id
            int exam_id = std::get<0>(examRoomTuple);
            // Get room_id
            int room_id = std::get<1>(examRoomTuple);
            cout << exam_id << "-" << room_id << ", ";
        }
        cout << endl;
#endif

        //
        // Unschedule exam from source time slot
        //
        timetableCont.unscheduleExam(examSource, _tSource);


        while (!_conflictingExamsTsource.empty() || !_conflictingExamsTdest.empty()) {

    #ifdef DEBUG_MODE
            cout << "doFeasibleMove cycle..." << endl;
            cin.get();
    #endif

            while (!_conflictingExamsTsource.empty()) {
                // Get source exam from conflict vector
                int examSource = std::get<0>(_conflictingExamsTsource.back());

                ////////////////////////////////////////////////////////////////////////////////
                // 1. Get source exam adjacent vertices that are scheduled in tDest time slot
                getSourceExamHardConflictsDestPeriod(examSource, examGraph, _tDest, destRoom, _conflictingExamsTdest);
                //
                // Unschedule all conflicting exams
                //
                for (int i = 0; i < _conflictingExamsTdest.size(); ++i) {
                    // Get exam variable
                    auto const &variable = _conflictingExamsTdest[i];
                    // Get exam
                    int examDest = std::get<0>(variable);

                    // If not yet unscheduled, remove exam from timetable
                    if (timetableCont.isExamScheduled(examDest, _tDest)) {
                        // Unschedule exam from dest time slot
                        timetableCont.unscheduleExam(examDest, _tDest);
                    }
                }
                //
                // Move sourceExam to time slot tDest
                //
                // Schedule exam examSource in time slot tDest
                ////////////////////
                //
                // Add num students of source exam to occupied capacity in (_tDest, roomDest)
                //
                // Get number of currently occupied seats for room roomDest
                timetableCont.addExamToRoom(examSource, _tDest, destRoom);
                ////////////////////
                // Put examSource in time slot tDest scheduled exams queue
                finalExamsTdest.push_back(std::make_tuple(examSource, _tDest, destRoom));
                // Remove sourceExam from source conflicting exams
                _conflictingExamsTsource.pop_back();
                ////////////////////////////////////////////////////////////////////////////////
            }

            while (!_conflictingExamsTdest.empty()) {
                // Get source exam from conflict vector
                int examSource = std::get<0>(_conflictingExamsTdest.back());

                ////////////////////////////////////////////////////////////////////////////////
                // 1. Get source exam adjacent vertices that are scheduled in tDest time slot
                getSourceExamHardConflictsDestPeriod(examSource, examGraph, _tSource, sourceRoom, _conflictingExamsTsource);
                //
                // Unschedule all conflicting exams
                //
                for (int i = 0; i < _conflictingExamsTsource.size(); ++i) {
                    // Get exam variable
                    auto const &variable = _conflictingExamsTsource[i];
                    // Get exam
                    int examDest = std::get<0>(variable);
                    // If not yet unscheduled, remove exam from timetable
                    if (timetableCont.isExamScheduled(examDest, _tSource)) {
                        // Unschedule exam from dest time slot
                        timetableCont.unscheduleExam(examDest, _tSource);
                    }
                }
                //
                // Move sourceExam to time slot tDest
                //
                // Schedule exam examSource in time slot tDest
                ////////////////////
                //
                // Add num students of source exam to occupied capacity in (_tSource, roomDest)
                //
                // Get number of currently occupied seats for room roomDest
                timetableCont.addExamToRoom(examSource, _tSource, sourceRoom);
                ////////////////////
                // Put examSource in time slot tSource scheduled exams queue
                finalExamsTsource.push_back(std::make_tuple(examSource, _tSource, sourceRoom));
                // Remove sourceExam from source conflicting exams
                _conflictingExamsTdest.pop_back();
                ////////////////////////////////////////////////////////////////////////////////
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////
    else if (currentOperator == Operator::ShiftMove) {
        //
        // Unschedule exam from source time slot
        //
        timetableCont.unscheduleExam(examSource, _tSource);

        bool isFirstExam = true;

        while (!_conflictingExamsTsource.empty() || !_conflictingExamsTdest.empty()) {

#ifdef DEBUG_MODE
            cout << "doFeasibleMove cycle..." << endl;
            cin.get();
#endif

            int roomDest;

            while (!_conflictingExamsTsource.empty()) {
                // Get source exam from conflict vector
                int examSource = std::get<0>(_conflictingExamsTsource.back());
                // If selected operator is ShiftMove
                if (currentOperator == Operator::ShiftMove) {
                    if (isFirstExam) {
                        // Select random room with sufficient capacity for hosting _examSource
                        bool isfeasibleRoom = selectRandomRoomWithCapacity(sol, examSource, _tSource, roomDest);
                        // Test if it's a feasible room
                        if (isfeasibleRoom) {
                            // Change _examSource room to new room
                            _conflictingExamsTsource.pop_back();
                            _conflictingExamsTsource.push_back(std::make_tuple(examSource, _tSource, roomDest));
                        }
                        else {
                            // If no feasible room was found, maintain the same room
                            roomDest = std::get<2>(_conflictingExamsTsource.back());
                        }

                        isFirstExam = false;
                    }
                    else {
                        /// LETS SIMPLIFY AND MOVE TO THE SAME ROOM
                        ///
                        roomDest = std::get<2>(_conflictingExamsTsource.back());
                    }
                }

                ////////////////////////////////////////////////////////////////////////////////
                // 1. Get source exam adjacent vertices that are scheduled in tDest time slot
                getSourceExamHardConflictsDestPeriod(examSource, examGraph, _tDest, roomDest, _conflictingExamsTdest);
                //
                // Unschedule all conflicting exams
                //
                for (int i = 0; i < _conflictingExamsTdest.size(); ++i) {
                    // Get exam variable
                    auto const &variable = _conflictingExamsTdest[i];
                    // Get exam
                    int examDest = std::get<0>(variable);
                    // Get roomDest
                    int roomDest = std::get<2>(variable);
                    // If not yet unscheduled, remove exam from timetable
                    if (timetableCont.isExamScheduled(examDest, _tDest)) {
                        // Unschedule exam from dest time slot
                        timetableCont.unscheduleExam(examDest, _tDest);
                    }
                }
                //
                // Move sourceExam to time slot tDest
                //
                // Schedule exam examSource in time slot tDest
                ////////////////////
                //
                // Add num students of source exam to occupied capacity in (_tDest, roomDest)
                //
                // Get number of currently occupied seats for room roomDest
                timetableCont.addExamToRoom(examSource, _tDest, roomDest);
                ////////////////////
                // Put examSource in time slot tDest scheduled exams queue
                finalExamsTdest.push_back(_conflictingExamsTsource.back());
                // Remove sourceExam from source conflicting exams
                _conflictingExamsTsource.pop_back();
                ////////////////////////////////////////////////////////////////////////////////
            }

            while (!_conflictingExamsTdest.empty()) {
                // Get source exam from conflict vector
                int examSource = std::get<0>(_conflictingExamsTdest.back());
                // Select random room with sufficient capacity for hosting _examSource
                int roomDest;
                roomDest = std::get<2>(_conflictingExamsTdest.back());
                ////////////////////////////////////////////////////////////////////////////////
                // 1. Get source exam adjacent vertices that are scheduled in tDest time slot
                getSourceExamHardConflictsDestPeriod(examSource, examGraph, _tSource, roomDest, _conflictingExamsTsource);
                //
                // Unschedule all conflicting exams
                //
                for (int i = 0; i < _conflictingExamsTsource.size(); ++i) {
                    // Get exam variable
                    auto const &variable = _conflictingExamsTsource[i];
                    // Get exam
                    int examDest = std::get<0>(variable);
                    // Get roomDest
                    int roomDest = std::get<2>(variable);
                    // If not yet unscheduled, remove exam from timetable
                    if (timetableCont.isExamScheduled(examDest, _tSource)) {
                        // Unschedule exam from dest time slot
                        timetableCont.unscheduleExam(examDest, _tSource);
                    }
                }
                //
                // Move sourceExam to time slot tDest
                //
                // Schedule exam examSource in time slot tDest
                ////////////////////
                //
                // Add num students of source exam to occupied capacity in (_tSource, roomDest)
                //
                // Get number of currently occupied seats for room roomDest
                timetableCont.addExamToRoom(examSource, _tSource, roomDest);
                ////////////////////
                // Put examSource in time slot tSource scheduled exams queue
                finalExamsTsource.push_back(_conflictingExamsTdest.back());
                // Remove sourceExam from source conflicting exams
                _conflictingExamsTdest.pop_back();
                ////////////////////////////////////////////////////////////////////////////////
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////


    // Insert exams in Ti
    while (!finalExamsTsource.empty()) {
        // Get source exam
        int examSource = std::get<0>(finalExamsTsource.back());
        // Select random room with sufficient capacity for hosting _examSource
        int roomDest;
        roomDest = std::get<2>(finalExamsTsource.back());
        ////////////////////
        //
        // Subtract num students of source exam to occupied capacity in (_tSource, roomDest)
        //
        timetableCont.removeExamFromRoom(examSource, _tSource, roomDest);
        ////////////////////
        // Schedule exam
        timetableCont.scheduleExam(examSource, _tSource, roomDest);
        // Pop exam
        finalExamsTsource.pop_back();
    }

    // Insert exams in Tj
    while (!finalExamsTdest.empty()) {
        // Get source exam
        int examSource = std::get<0>(finalExamsTdest.back());
        // Select random room with sufficient capacity for hosting _examSource
        int roomDest;
        roomDest = std::get<2>(finalExamsTdest.back());

        ////////////////////
        //
        // Subtract num students of source exam to occupied capacity in (_tDest, roomDest)
        //
        timetableCont.removeExamFromRoom(examSource, _tDest, roomDest);
        ////////////////////
        // Schedule exam
        timetableCont.scheduleExam(examSource, _tDest, roomDest);
        // Pop exam
        finalExamsTdest.pop_back();
    }


#ifdef DEBUG_MODE_1
        cout << endl << "After move" << endl;
        // Get periodTi exams
        periodTiExams = timetableCont.getPeriodExams(kempeChain.getTi());

        cout << "Period Ti = " << kempeChain.getTi() << ":" << endl;
        // For each exam of periodTiexams do
        for (auto const& examRoomTuple : periodTiExams)
        {
            // Get exam_id
            int exam_id = std::get<0>(examRoomTuple);
            // Get room_id
            int room_id = std::get<1>(examRoomTuple);
            cout << exam_id << "-" << room_id << ", ";
        }
        cout << endl;

        // Get periodTj exams
        periodTjExams = timetableCont.getPeriodExams(kempeChain.getTj());
        cout << "Period Tj = " << kempeChain.getTj() << ":" << endl;
        // For each exam of periodTiexams do
        for (auto const& examRoomTuple : periodTjExams)
        {
            // Get exam_id
            int exam_id = std::get<0>(examRoomTuple);
            // Get room_id
            int room_id = std::get<1>(examRoomTuple);
            cout << exam_id << "-" << room_id << ", ";
        }
        cout << endl;
#endif


#ifdef DEBUG_MODE
    cout << "At the end of [ETTPKempeChainHeuristic<EOT>::doFeasibleExamMove] method" << endl;
    cout << "Verifying room occupancy constraints for affected periods..." << endl;
    bool verificationPeriodTi = sol.verifyRoomsCapacityConstraint(kempeChain.getTi());
    cout << "verificationPeriodTi = " << verificationPeriodTi << endl;
    bool verificationPeriodTj = sol.verifyRoomsCapacityConstraint(kempeChain.getTj());
    cout << "verificationPeriodTj = " << verificationPeriodTj << endl;
//    cin.get();
#endif
}






/**
 * @brief getSourceExamHardConflictsDestPeriod
 * @param _examSource
 * @param _examGraph
 * @param _tDest
 * @param _roomDest
 * @param _conflictingExamsTdest
 */
template <typename EOT>
void ETTPKempeChainHeuristic<EOT>
    ::getSourceExamHardConflictsDestPeriod(int _examSource, AdjacencyList const &_examGraph,
                                       int _tDest, int _roomDest,
                                       std::vector<typename GCHeuristics<EOT>::VariableValueTuple> &_conflictingExamsTdest) const {

    // Get solution
    auto &sol = this->kempeChain.getSolution();
    // Get timetable container
    TimetableContainer &timetableCont = sol.getTimetableContainer();
    // Get scheduled exams vector
    auto const &scheduledExamsVector = sol.getScheduledExamsVector();

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Pre-condition for verifying the hard constraints: examSource must not be scheduled
    //
    // We have first to unschedule the _examSource from its current period and room in order to find
    // the hard conflicts of _examSource in the (_tDest, _roomDest) period-room pair.
    // This is because the hardConflicts method schedules the exam (if necessary) in order to verify the constraints.
    //

#ifdef DEBUG_MODE
    // Get ScheduledExam object
    ScheduledExam const &scheduledExam = scheduledExamsVector[_examSource];
    // Source time slot
    int sourceTimeslot = scheduledExam.getPeriod();
    // Source room
    int sourceRoom = scheduledExam.getRoom();
    cout << endl << "In [ETTPKempeChainHeuristic<EOT>::getSourceExamHardConflictsDestPeriod] method:" << endl;
    bool examScheduled = scheduledExam.isScheduled();
    cout << "_examSource = " << _examSource << ", sourceTimeslot = " << sourceTimeslot << ", sourceRoom = " << sourceRoom
         << ", isScheduled() = " << (examScheduled ? "Yes" : "No") << endl;
//    if (!examScheduled)
//        throw runtime_error("In method [ETTPKempeChainHeuristic<EOT>::getSourceExamHardConflictsDestPeriod]: Exam should be scheduled");
    cout << "Determine hardConflicts for _examSource " << _examSource << " in (_tDest, _roomDest) = ("
         << _tDest << ", " << _roomDest << ")" << endl;
#endif

    // Unschedule exam
//    timetableCont.unscheduleExam(_examSource, sourceTimeslot);
    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Get conflicting exams
//    auto hardConflicts = GCHeuristics<EOT>::hardConflicts(_examSource, std::make_tuple(_tDest, _roomDest));

    auto hardConflicts = GCHeuristics<EOT>::hardConflictsWithoutAfterConstraint(sol, _examSource, std::make_tuple(_tDest, _roomDest));



    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Schedule exam again
//    timetableCont.scheduleExam(_examSource, sourceTimeslot, sourceRoom);
    //////////////////////////////////////////////////////////////////////////////////////////////////

    // Add hard conflicts to _conflictingExamsTdest  vector
    for (auto const &conflict : hardConflicts) {
        _conflictingExamsTdest.push_back(conflict);
    }

    // Apply unique to variable vector to find the new end without duplicate.
    // Then remove duplicates
    std::sort(_conflictingExamsTdest.begin(), _conflictingExamsTdest.end());
    auto it = std::unique(_conflictingExamsTdest.begin(), _conflictingExamsTdest.end());
    _conflictingExamsTdest.resize(it-_conflictingExamsTdest.begin());

#ifdef DEBUG_MODE
    cout << "Hard conflicts: " << endl;
    for (auto const &conflict : _conflictingExamsTdest) {
        // Get exam
        int exam = std::get<0>(conflict);
        // Get timeslot
        int timeslot = std::get<1>(conflict);
        // Get room
        int room = std::get<2>(conflict);
        // Get ScheduledExam object
        ScheduledExam const &scheduledExam = scheduledExamsVector[exam];
        // Source time slot
        int scheduledTimeslot = scheduledExam.getPeriod();
        // Source room
        int scheduledRoom = scheduledExam.getRoom();
        // Exam scheduled?
        bool examScheduled = scheduledExam.isScheduled();
        cout << "exam = " << exam << ", timeslot = " << timeslot << ", room = " << room << endl
             << "scheduledExam = " << scheduledExam.getId() << ", scheduledTimeslot = " << scheduledTimeslot
             << ", scheduledRoom = " << scheduledRoom
             << ", isScheduled() = " << (examScheduled ? "Yes" : "No") << endl;
    }

#endif

}





/**
 * @brief initialiseKempeChain
 * @param _sol
 * @param _ti
 * @param _exami
 * @param _roomi
 * @param _tj
 */
template <typename EOT>
void ETTPKempeChainHeuristic<EOT>::initialiseKempeChain(EOT &_sol, int _ti, int _exami, int _roomi, int _tj) {
    // Initialise Kempe chain object
    //
    // Set solution reference
    kempeChain.setSolution(&_sol);
    // Set source timeslot Ti
    kempeChain.setTi(_ti);
    // Set source exam id
    kempeChain.setEi(_exami);
    // Set source room id
    kempeChain.setRi(_roomi);
    // Set destination timeslot Tj
    kempeChain.setTj(_tj);
    // Record source Ti and Tj period exams
    // Pre-condition: kempe chain solution must be initialised
    recordOriginalPeriodExams(_ti, _tj);
}




/**
 * @brief recordOriginalPeriodExams Record source Ti and Tj period exams
 * @param _ti
 * @param _tj
 */
template <typename EOT>
void ETTPKempeChainHeuristic<EOT>::recordOriginalPeriodExams(int _ti, int _tj) {
    // Get timetable container
    auto const &timetableContainer = kempeChain.getSolution().getTimetableContainer();
    // Note: getPeriodExams return a packed vector with the period exams
    // Copy original ti an tj contents and put them in the kempe chain data structure
    kempeChain.setOriginalTiPeriodExams(timetableContainer.getPeriodExams(_ti));
    kempeChain.setOriginalTjPeriodExams(timetableContainer.getPeriodExams(_tj));
}




/**
 * @brief recordFinalPeriodExams Record Ti and Tj period exams after move is made
 * @param _ti
 * @param _tj
 */
template <typename EOT>
void ETTPKempeChainHeuristic<EOT>::recordFinalPeriodExams(int _ti, int _tj) {
    // Get timetable container
    auto &timetableContainer = kempeChain.getSolution().getTimetableContainer();
    // Note: getPeriodExams return a packed vector with the period exams
    // Copy destination ti an tj contents and put them in the kempe chain data structure
    kempeChain.setFinalTiPeriodExams(timetableContainer.getPeriodExams(_ti));
    kempeChain.setFinalTjPeriodExams(timetableContainer.getPeriodExams(_tj));
}




/**
 * @brief selectRandomExam
 * @param _timetableCont
 * @param _sol
 * @param _ti
 * @param _exami
 * @param _roomi
 */
template <typename EOT>
void ETTPKempeChainHeuristic<EOT>::selectRandomExam(TimetableContainer const &_timetableCont, EOT &_sol,
                                                    int _ti, int &_exami, int &_roomi) const
{
    //
    // Selected exam id (randomly) to move, and return its room
    //
    // Get period exams
    auto &periodExams = _timetableCont.getPeriodExams(_ti);
    // Generate random exam ei index
    int randIdx = rng.random(periodExams.size());
    // Selected exam id to move
    auto &examRoomTuple = periodExams[randIdx];
    // Get exam
    _exami = std::get<0>(examRoomTuple);
    // Get exam
    _roomi = std::get<1>(examRoomTuple);
}





/**
 * @brief selectRandomRoomWithCapacity
 * @param _sol
 * @param _ei
 * @param _tj
 * @param _rk
 * @return room with sufficient capacity for hosting _ei
 */
template <typename EOT>
bool ETTPKempeChainHeuristic<EOT>::selectRandomRoomWithCapacity(EOT &_sol, int _ei, int _tj, int &_rk) const {
    return _sol.getFeasibleRoom(_ei, _tj, _rk);
}





/**
 * @brief selectRandomTimeslots
 * @param _timetableCont
 * @param _numPeriods
 * @param _ti
 * @param _tj
 */
template <typename EOT>
void ETTPKempeChainHeuristic<EOT>::selectRandomTimeslots(TimetableContainer const &_timetableCont, int _numPeriods,
                                                         int &_ti, int &_tj) const {
    // Select randomly two time slots, ti and tj.
    do {
        _ti = rng.random(_numPeriods);
        do {
            _tj = rng.random(_numPeriods);
        }
        while (_ti == _tj);
    }
    // Repeat until we found a non-empty time slot ti different from tj
    while (_timetableCont.getPeriodSize(_ti) == 0);
}





/**
 * @brief selectRandomTimeslot
 * @param _timetableCont
 * @param _numPeriods
 * @param _ti
 * @param _tj
 */
template <typename EOT>
void ETTPKempeChainHeuristic<EOT>::selectRandomTimeslot(TimetableContainer const &_timetableCont, int _numPeriods,
                                                         int &_ti) const {
    // Select randomly a time slots, ti.
    do {
        _ti = rng.random(_numPeriods);
    }
    // Repeat until we found a non-empty time slot ti
    while (_timetableCont.getPeriodSize(_ti) == 0);
}




/**
 * @brief setNeighbourFeasibility
 * @param _value
 */
template <typename EOT>
void ETTPKempeChainHeuristic<EOT>::setNeighbourFeasibility(bool _value) {
    feasibleNeighbour = _value;
}





/**
 * @brief shiftMove
 * @param _sol
 */
template <typename EOT>
void ETTPKempeChainHeuristic<EOT>:: shiftMove(EOT &_sol) {
    //
    // 2. Shift move - Here a random exam is moved into different
    // (randomly chosen) timeslot and room.
    //
    // Set operator type
    currentOperator = Operator::ShiftMove;
    // Initially assume that neigbour solution is feasible
    setNeighbourFeasibility(true);
    // Get timetable container
    TimetableContainer &timetableCont = _sol.getTimetableContainer();
    // Get # periods
    int numPeriods = _sol.getNumPeriods();
    // Select distinct source and destination time slots
    int ti, tj;
    selectRandomTimeslots(timetableCont, numPeriods, ti, tj);
    // Selected exam id (randomly) to move, and return its room
    int exami, roomi;
    selectRandomExam(timetableCont, _sol, ti, exami, roomi);
     // Initialise Kempe chain object
    initialiseKempeChain(_sol, ti, exami, roomi, tj);

#ifdef ETTP_KEMPE_CHAIN_HEURISTIC_DEBUG
    std::cout << std::endl << std::endl
              << "/////////////////////////////////////////////////////////////" << std::endl;
    std::cout << "In [ETTPKempeChainHeuristic<EOT>::shiftMove()]:" << std::endl;
    std::cout << "ti = " << ti << ", exami = " << exami << ", roomi = " << roomi << std::endl;
    std::cout << "tj = " << tj << std::endl;
    std::cout << "isExamScheduled? " << (timetableCont.isExamScheduled(exami, ti) ? "Yes" : "No") << std::endl;
//    cin.get();
#endif
}





/**
 * @brief roomMove
 * @param _sol
 */
template <typename EOT>
void ETTPKempeChainHeuristic<EOT>::roomMove(EOT &_sol) {
    //
    // Apply operator 1. Room move - Here a random exam is just moved into a different
    // (randomly chosen) room within the same timeslot.
    //
    // Set operator type
    currentOperator = Operator::RoomMove;
    // This operator always produce a neigbour solution that is feasible
    setNeighbourFeasibility(true);
    // Get timetable container
    TimetableContainer &timetableCont = _sol.getTimetableContainer();
    // Get # periods
    int numPeriods = _sol.getNumPeriods();
    // Select a random time slot
    int ti, tj;
    selectRandomTimeslot(timetableCont, numPeriods, ti);
    // Set tj equal to ti
    tj = ti;
    // Selected exam id (randomly) to move, and return its room
    int exami, roomi;
    selectRandomExam(timetableCont, _sol, ti, exami, roomi);
     // Initialise Kempe chain object
    initialiseKempeChain(_sol, ti, exami, roomi, tj);
#ifdef ETTP_KEMPE_CHAIN_HEURISTIC_DEBUG
    std::cout << std::endl << std::endl
              << "/////////////////////////////////////////////////////////////" << std::endl;
    std::cout << "In [ETTPKempeChainHeuristic<EOT>::roomMove()]:" << std::endl;
    std::cout << "ti = " << ti << ", exami = " << exami << ", roomi = " << roomi << std::endl;
    std::cout << "tj = " << tj << std::endl;
//    std::cout << "isExamScheduled? " << (timetableCont.isExamScheduled(exami, ti) ? "Yes" : "No") << std::endl;
//    cin.get();
#endif
}




#endif // ETTPKEMPECHAINHEURISTIC_H



