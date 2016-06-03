#ifndef EOCHROMOSOME_H
#define EOCHROMOSOME_H


#include <EO.h>

#include "containers/Matrix.h"
#include "containers/TimetableContainer.h"
#include "containers/TimetableContainerMatrix.h"
#include "data/TimetableProblemData.hpp"
#include "utils/Common.h"
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <vector>
#include <string>
#include <ostream>
#include "kempeChain/ETTPKempeChain.h"
#include "data/ScheduledExam.h"
#include "data/ScheduledRoom.h"



//#define EOCHROMOSOME_DEBUG_COPY_CTOR

//#define EOCHROMOSOME_DEBUG_CTOR


// EO (Evolving Object) chromosome implementation.
// A chromosome encodes a complete and feasible timetable.
// The chromosome holds a matrix where the columns represent the periods,
// holding, by its turn, the period exams.
// For fast access, insertion, removal, and random selection of exams,
// the timetable matrix has dimensions of (# exams x # periods) where the
// values 0/1 represent, respectively, absence/presence of exam in the period.

/**
 * @brief The eoChromosome class
 */
class eoChromosome : public EO<double> {

public:
    /**
     * @brief Chromosome Default chromosome constructor
     */
    eoChromosome()
        :
          /// Implementation #1 - TimetableContainerMatrix
          ///
//          timetableContainer(boost::make_shared<TimetableContainerMatrix>()),
          timetableContainer(nullptr),
          timetableProblemData(nullptr),
          feasible(false),
          solutionCost(0) { }


        eoChromosome(TimetableProblemData const *_timetableProblemData)
            :
              /// Implementation #1 - TimetableContainerMatrix
              ///
              timetableContainer(boost::make_shared<TimetableContainerMatrix>(
                                       _timetableProblemData->getNumExams(), _timetableProblemData->getNumPeriods(),
                                       _timetableProblemData->getNumRooms(), _timetableProblemData)),
              timetableProblemData(_timetableProblemData),
              feasible(false),
              solutionCost(0) {
#ifdef EOCHROMOSOME_DEBUG_CTOR
                std::cout << "eoChromosome(TimetableProblemData const *_timetableProblemData) ctor" << std::endl;
#endif

        }


    /**
     * @brief eoChromosome Copy constructor
     * @param _chrom
     */
    eoChromosome(const eoChromosome &_chrom)
        : timetableContainer(boost::make_shared<TimetableContainerMatrix>(
                                 _chrom.getNumExams(), _chrom.getNumPeriods(), _chrom.getNumRooms(),
                                 _chrom.getTimetableProblemData())),
          timetableProblemData(_chrom.getTimetableProblemData()),
          feasible(_chrom.isFeasible()),
          solutionCost(_chrom.solutionCost) {

        // Copy timetable data
        copyTimetableData(_chrom);

        // Set fitness
        fitness(_chrom.fitness());

//        validate();

#ifdef EOCHROMOSOME_DEBUG_COPY_CTOR
        std::cout << "eoChromosome::Copy ctor" << std::endl;
#endif
    }

    /**
     * @brief operator =
     * @param _chrom
     * @return
     */
    eoChromosome& operator=(const eoChromosome &_chrom) {
#ifdef EOCHROMOSOME_DEBUG_COPY_CTOR
        std::cout << "eoChromosome::operator=" << std::endl;
        std::cin.get();
#endif

        if (&_chrom != this) {
            timetableContainer = boost::make_shared<TimetableContainerMatrix>(
                        _chrom.getNumExams(), _chrom.getNumPeriods(), _chrom.getNumRooms(),
                        _chrom.getTimetableProblemData());
            timetableProblemData = _chrom.getTimetableProblemData();
            feasible = _chrom.isFeasible();
            solutionCost = _chrom.solutionCost;
            // Copy timetable data
            copyTimetableData(_chrom);
            // Set fitness
            fitness(_chrom.fitness());
        }
        return *this;
    }

protected:
    // Copy timetable data
    void copyTimetableData(const eoChromosome &_chrom) {
        // Copy timetable data
        auto &timetableContThis = getTimetableContainer();
        auto const &timetableContOther = _chrom.getTimetableContainer();

        // For each period do
        for (int pi = 0; pi < timetableContOther.getNumPeriods(); ++pi) {
            // Get period pi exams and size
            auto &periods = timetableContOther.getCompletePeriod(pi);
            int periodSize = timetableContOther.getPeriodSize(pi);
            // Copy to this timetable
            timetableContThis.replacePeriod(pi, periods, periodSize);
        }
    }

public:

    /**
     * @brief init Initialise the chromosome
     * @param _timetableProblemData Timetable problem data
     */
    void init(TimetableProblemData const *_timetableProblemData);

    ////////// TimetableProblemData methods ////////////////////////////////////////////////
    /**
     * @brief getTimetableProblemData
     * @return
     */
    inline const TimetableProblemData *getTimetableProblemData() const;
    /**
     * @brief setTimetableProblemData
     * @param value
     */
    inline void setTimetableProblemData(const TimetableProblemData *_value);
    /**
     * @brief getConflictMatrix
     * @return The conflict matrix
     */
    inline IntMatrix const &getConflictMatrix() const;
    /**
     * @brief getConflictMatrixDensity
     * @return The conflict matrix density
     */
    inline double getConflictMatrixDensity() const;
    /**
     * @brief getCourseStudentCounts Obtain course student counts
     * @return
     */
    inline std::vector<int> const& getCourseClassSize() const;
    /**
     * @brief getCourseStudentCounts Obtain course student counts sorted in decreasing order by student counts
     * @return
     */
    inline std::vector<std::pair<int,int>> const &getSortedCourseClassSize() const;
    /**
     * @brief getGraph
     * @return The exam graph
     */
    inline AdjacencyList const &getExamGraph();
    /**
     * @brief getNumEnrolments
     * @return The number of enrolments
     */
    inline int getNumEnrolments() const;
    /**
     * @brief getNumExams
     * @return The number of exams
     */
    inline int getNumExams() const;
    /**
     * @brief getNumPeriods
     * @return The number of periods
     */
    inline int getNumPeriods() const;
    /**
     * @brief getNumRooms
     * @return The number of rooms
     */
    inline int getNumRooms() const;
    /**
     * @brief getNumStudents
     * @return The nmber of students
     */
    inline int getNumStudents() const;


    ////////// TimetableContainer methods //////////////////////////////////////////////////////
    /**
     * @brief getRoomNumExamsScheduled
     * @param _rk
     * @return
     */
    inline int getRoomNumExamsScheduled(int _rk) const;
    /**
     * @brief getScheduledExamVector
     * @return The scheduled exams vector
     */
    inline const std::vector<ScheduledExam> &getScheduledExamsVector() const;
    /**
     * @brief getScheduledRoomsVector
     * @return
     */
    inline const std::vector<ScheduledRoom> &getScheduledRoomsVector() const;
    /**
     * @brief getTimetableContainer Return the timetable container
     * @return
     */
    inline TimetableContainer &getTimetableContainer();
    /**
     * @brief getTimetableContainer
     * @return
     */
    inline TimetableContainer const &getTimetableContainer() const;


    ////////// Chromosome cost and feasibility manipulation methods //////////////////////////////

    /**
     * @brief computeCost
     */
    void computeCost();
    /**
     * @brief getSolutionCost
     * @return
     */
    inline long getSolutionCost() const;

    ///////////// Incremental cost computation ////////////////////////////////////////////////////

    /**
     * @brief getExamProximityConflicts
     * @param _ei
     * @param _period
     * @return
     */
    long getExamProximityConflicts(int _ei, int _pi);

    /**
     * @brief getConflictInADayAndRowFromDay
     * @param _examination
     * @param _period
     * @return
     */
    int getConflictInADayAndRowFromDay(int _examination, int _period);

    /**
     * @brief getConflictPeriodSpreadBeforeAndAfterPeriod
     * @param _examination
     * @param _period
     * @return
     */
    int getConflictPeriodSpreadBeforeAndAfterPeriod(int _examination, int _period);

    /**
     * @brief getConflictMixedDurationsFromPeriodAndRoom
     * @param _period
     * @param _room
     * @return
     */
    int getConflictMixedDurationsFromPeriodAndRoom(int _period, int _room);


    /////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * @brief isFeasible
     * @return true if chromosome is feasible and false otherwise
     */
    inline bool isFeasible() const;

    /**
     * @brief setSolutionCost
     * @param _solutionCost
     */
    inline void setSolutionCost(long _solutionCost);
    /**
     * @brief setFeasible
     * @param feasible
     */
    inline void setFeasible(bool _feasible);


    /**
     * @brief computeExamProximityConflictsIncremental Compute chromosome's proximity cost
     * performing an incremental evaluation based on the Kempe chain information.
     *
     * @param _kempeChain
     */
    void computeSolutionCostIncremental(const ETTPKempeChain<eoChromosome> &_kempeChain);

    /**
     * @brief getExamsToBeMoved
     * @param _srcPeriodExams
     * @param _destPeriodExams
     * @param _tSource
     * @param _tDest
     * @param _sourceRoom
     * @param _examsToBeMovedFromSourcePeriod
     */
    void getExamsToBeMoved(std::vector<typename ETTPKempeChain<eoChromosome>::ExamRoomTuple> const &_srcPeriodExams,
                           std::vector<typename ETTPKempeChain<eoChromosome>::ExamRoomTuple> const &_destPeriodExams,
                           int _tSource, int _tDest, int _sourceRoom,
                           std::vector<std::tuple<int, int, int> > &_examsToBeMovedFromSourcePeriod);


    /**
     * @brief validate Validate a chromosome solution
     */
    void validate() const;

    ////////// Timetable manipulation methods //////////////////////////////

    /**
     * @brief isExamFeasibleInPeriod
     * @param _exam
     * @param _period
     * @return
     */
    bool isExamFeasibleInPeriod(int _exam, int _period) const;

    /**
     * @brief getExamVector
     * @return The exam vector
     */
    inline std::vector<boost::shared_ptr<Exam> > const &getExamVector() const;
    /**
     * @brief getRoomVector
     * @return The room vector
     */
    inline std::vector<boost::shared_ptr<Room> > const &getRoomVector() const;
    /**
     * @brief getPeriodVector
     * @return The period vector
     */
    inline std::vector<boost::shared_ptr<ITC2007Period> > const &getPeriodVector() const;
    /**
     * @brief getFeasiblePeriodRoom
     * @param _ei
     * @param _tj
     * @param _rk
     */
    bool getFeasiblePeriodRoom(int _ei, int _tj, int& _rk);
    /**
     * @brief removeConflictingExams  Remove ei conflicting exams located in period tj and room rk
     * @param _ei
     * @param _tj
     * @param _rk
     * @param _unscheduledExams
     * @return
     */
    bool removeConflictingExams(int _ei, int _tj, int _rk, std::vector<int> & _unscheduledExams);

    /**
     * @brief verifyAfterConstraint
     * @param _ei
     * @param _tj
     * @return
     */
    bool verifyAfterConstraint(int _ei, int _tj);

    /**
     * @brief verifyRoomCapacity Verify room capacity for exam _ei
     * @param _ei
     * @param _rk
     * @return
     */
    bool verifyRoomCapacity(int _ei, int _rk) const;
    /**
     * @brief verifyPeriodUtilisationConstraint Verify Period-Utilisation constraint
     * @param _ei
     * @param _tj
     * @return
     */
    bool verifyPeriodUtilisationConstraint(int _ei, int _tj) const;

    /**
     * @brief verifyPeriodUtilisationAndAfterConstraints
     * @param _ei
     * @param _tj
     * @return
     */
    bool verifyPeriodUtilisationAndAfterConstraints(int _ei, int _tj);

    /**
     * @brief verifyPeriodRelatedConstraintsScheduled Verify Period-Related constraint. Pre-condition: ei *is* scheduled
     * @param _ei
     * @param _tj
     * @return
     */
    bool verifyPeriodRelatedConstraintsScheduled(int _ei, int _tj) const;

    /**
     * @brief verifyPeriodRelatedConstraints Verify Period-Related constraint
     * @param _ei
     * @param _tj
     * @return
     */
    bool verifyPeriodRelatedConstraints(int _ei, int _tj);
    /**
     * @brief getFeasibleRoom Verify Room-Occupancy and Room-Related constraints and return feasible random room
     * @param _ei
     * @param _tj
     * @param _rk
     * @return
     */
    bool getFeasibleRoom(int _ei, int _tj, int &_rk);

    /**
     * @brief verifyRoomCapacityConstraint Verify Room capacity constraint
     * @param _ei
     * @param _rk
     * @return
     */
    bool verifyRoomCapacityConstraint(int _ei, int _rk) const;


    /**
     * @brief verifyRoomsCapacityConstraint
     * @param _ti
     * @return
     */
    bool verifyRoomsCapacityConstraint(int _ti) const;


    /**
     * @brief verifyRoomOccupancyConstraint Verify Room-Occupancy constraint
     * @param _ei
     * @param _tj
     * @param _rk
     * @return
     */
    bool verifyRoomOccupancyConstraint(int _ei, int _tj, int _rk) const;

    /**
     * @brief verifyRoomRelatedConstraints Verify Room-Related constraint
     * @param _ei
     * @param _tj
     * @param _rk
     * @return
     */
    bool verifyRoomRelatedConstraints(int _ei, int _tj, int _rk);
    /**
     * @brief getRoomExamsSorted
     * @param _tj
     * @param _rk
     * @return Vector containing exams allocated to room rk, sorted by decreasing order of # occuppied seats
     */
    std::vector<std::pair<int, int>> getRoomExamsSorted(int _tj, int _rk) const;


    /**
     * @brief getRoomExams
     * @param _tj
     * @param _rk
     * @return Vector containing exams allocated to room rk
     */
    std::vector<std::pair<int, int>> getRoomExams(int _tj, int _rk) const;


    ////////// Chromosome Misc //////////////////////////////
    /**
     * @brief className +Overriden method+
     * @return The class name
     */
    inline std::string className() const override;
    /**
     * @brief operator << Print chromosome contents
     * @param _os
     * @param _chrom
     * @return
     */
    friend std::ostream& operator<<(std::ostream& _os, const eoChromosome &_chrom);



protected:
    /**
     * @brief timetableMatrix The timetable container
     */
    boost::shared_ptr<TimetableContainer> timetableContainer;
    /**
     * @brief timetableProblemData The problem data
     */
    TimetableProblemData const* timetableProblemData;
    /**
     * @brief isFeasible This variable is used to determine if the chromosome is feasible or not
     */
    bool feasible;

    /**
     * @brief solutionFitness
     */
    long solutionCost;
};



////////// TimetableProblemData methods ////////////////////////////////////////////////
/**
 * @brief eoChromosome::getTimetableProblemData
 * @return
 */
const TimetableProblemData *eoChromosome::getTimetableProblemData() const {
    return timetableProblemData;
}
/**
 * @brief eoChromosome::setTimetableProblemData
 * @param _value
 */
void eoChromosome::setTimetableProblemData(const TimetableProblemData *_value) {
    timetableProblemData = _value;
    // Set timetableProblemData field in timetableContainer object
    timetableContainer->setTimetableProblemData(_value);
}

/**
 * @brief getConflictMatrix
 * @return The conflict matrix
 */
IntMatrix const &eoChromosome::getConflictMatrix() const {
    return timetableProblemData->getConflictMatrix();
}
/**
 * @brief getConflictMatrixDensity
 * @return The conflict matrix density
 */
double eoChromosome::getConflictMatrixDensity() const {
    return timetableProblemData->getConflictMatrixDensity();
}
/**
 * @brief getCourseStudentCounts Obtain course student counts
 * @return
 */
std::vector<int> const &eoChromosome::getCourseClassSize() const {
    return timetableProblemData->getCourseClassSize();
}

/**
 * @brief getCourseStudentCounts Obtain course student counts sorted in decreasing order by student counts
 * @return
 */
std::vector<std::pair<int,int>> const &eoChromosome::getSortedCourseClassSize() const {
    return timetableProblemData->getSortedCourseClassSize();
}

/**
 * @brief getGraph
 * @return The exam graph
 */
AdjacencyList const &eoChromosome::getExamGraph() {
    return timetableProblemData->getExamGraph();
}
/**
 * @brief getNumEnrolments
 * @return The number of enrolments
 */
int eoChromosome::getNumEnrolments() const {
    return timetableProblemData->getNumEnrolments();
}
/**
 * @brief getNumExams
 * @return The number of exams
 */
int eoChromosome::getNumExams() const {
    return timetableProblemData->getNumExams();
}
/**
 * @brief getNumPeriods
 * @return The number of periods
 */
int eoChromosome::getNumPeriods() const {
    return timetableProblemData->getNumPeriods();
}
/**
 * @brief getNumRooms
 * @return The number of rooms
 */
int eoChromosome::getNumRooms() const {
    return timetableProblemData->getNumRooms();
}
/**
 * @brief getNumStudents
 * @return The nmber of students
 */
int eoChromosome::getNumStudents() const {
    return timetableProblemData->getNumStudents();
}

////////// TimetableContainer methods //////////////////////////////////////////////////////

/**
 * @brief getScheduledExamsVector
 * @return The scheduled exams vector
 */
std::vector<ScheduledExam> const &eoChromosome::getScheduledExamsVector() const {
    return timetableContainer->getScheduledExamsVector();
}

/**
 * @brief getScheduledRoomsVector
 * @return
 */
inline const std::vector<ScheduledRoom> &eoChromosome::getScheduledRoomsVector() const {
    return timetableContainer->getScheduledRoomsVector();
}

/**
 * @brief getRoomNumExamsScheduled
 * @param _rk
 * @return
 */
int eoChromosome::getRoomNumExamsScheduled(int _rk) const {
    /// TODO
    ///
}

/**
 * @brief getTimetableContainer
 * @return the timetable container
 */
TimetableContainer &eoChromosome::getTimetableContainer() {
    return *timetableContainer.get();
}
/**
 * @brief getTimetableContainer
 * @return the timetable container
 */
TimetableContainer const &eoChromosome::getTimetableContainer() const {
    return *timetableContainer.get();
}



////////// Chromosome cost and feasibility manipulation methods //////////////////////////////

/**
 * @brief getSolutionCost
 * @return
 */
long eoChromosome::getSolutionCost() const {
    return solutionCost;
}

/**
 * @brief isFeasible
 * @return true if chromosome is feasible and false otherwise
 */
bool eoChromosome::isFeasible() const {
    return feasible;
}

/**
 * @brief eoChromosome::setSolutionCost
 * @param _solutionCost
 */
void eoChromosome::setSolutionCost(long _solutionCost) {
    solutionCost = _solutionCost;
}

/**
 * @brief setFeasible
 * @param feasible
 */
void eoChromosome::setFeasible(bool _feasible) {
    feasible = _feasible;
}


////////// Timetable manipulation methods //////////////////////////////

/**
 * @brief getExamVector
 * @return The exam vector
 */
std::vector<boost::shared_ptr<Exam> > const &eoChromosome::getExamVector() const {
    return timetableProblemData->getExamVector();
}
/**
 * @brief getRoomVector
 * @return The room vector
 */
std::vector<boost::shared_ptr<Room> > const &eoChromosome::getRoomVector() const {
    return timetableProblemData->getRoomVector();
}
/**
 * @brief getPeriodVector
 * @return The period vector
 */
std::vector<boost::shared_ptr<ITC2007Period> > const &eoChromosome::getPeriodVector() const {
    return timetableProblemData->getPeriodVector();
}


////////// Chromosome Misc //////////////////////////////
/**
 * @brief className +Overriden method+
 * @return The class name
 */
std::string eoChromosome::className() const {
    return "EO Chromosome";
}



#endif // EOCHROMOSOME_H










