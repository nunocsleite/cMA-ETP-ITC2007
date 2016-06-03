#ifndef TIMETABLEPROBLEMDATA_H
#define TIMETABLEPROBLEMDATA_H

#include <vector>
#include <iostream>
#include "containers/IntMatrix.h"
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include "utils/Common.h"
#include "data/Exam.h"
#include "data/Room.h"
#include "data/Period.h"
#include "data/ITC2007Period.h"
#include "data/InstitutionalModelWeightings.h"


class TimetableProblemData {

public:
    // Constructors
    TimetableProblemData() { }

    TimetableProblemData(int _numPeriods, int _numStudents, int _numExams, int _numEnrolments,
                         boost::shared_ptr<IntMatrix> _conflictMatrix,
                         boost::shared_ptr<AdjacencyList> _examGraph)

        : numPeriods(_numPeriods), numStudents(_numStudents), numExams(_numExams),
          numEnrolments(_numEnrolments),
          conflictMatrix(_conflictMatrix),
          examGraph(_examGraph)
    {
        // Compute conflict matrix density
        computeConflictMatrixDensity();

    }

    // Public interface
    //
    // Getters & setters
    int getNumPeriods() const;
    void setNumPeriods(int value);

    int getNumRooms() const;
    void setNumRooms(int value);

    int getNumStudents() const;
    void setNumStudents(int value);

    int getNumExams() const;
    void setNumExams(int value);

    int getNumEnrolments() const;
    void setNumEnrolments(int value);

    double getConflictMatrixDensity() const;
    void setConflictMatrixDensity(double value);

    const IntMatrix &getConflictMatrix() const;
    void setConflictMatrix(const boost::shared_ptr<IntMatrix > &value);

    const AdjacencyList &getExamGraph() const;
    void setExamGraph(const boost::shared_ptr<AdjacencyList> &value);

    // Get/set course class sizes
    const std::vector<int> &getCourseClassSize() const;
    void setCourseClassSize(const boost::shared_ptr<std::vector<int> > &_courseClassSize);

    // Get/set sorted course class sizes
    const std::vector<std::pair<int,int> > &getSortedCourseClassSize() const;
    void setSortedCourseClassSize(const boost::shared_ptr<std::vector<std::pair<int,int> > > &_sortedCourseClassSize);

    InstitutionalModelWeightings const &getInstitutionalModelWeightings() const;
    void setInstitutionalModelWeightings(InstitutionalModelWeightings const &_model_weightings);

    std::vector<boost::shared_ptr<Exam> > const& getExamVector() const;
    void setExamVector(const boost::shared_ptr<std::vector<boost::shared_ptr<Exam> > > &value);

    std::vector<boost::shared_ptr<Room> > const& getRoomVector() const;
    void setRoomVector(const boost::shared_ptr<std::vector<boost::shared_ptr<Room> > > &value);

    const std::vector<boost::shared_ptr<ITC2007Period> > &getPeriodVector() const;
    void setPeriodVector(const boost::shared_ptr<std::vector<boost::shared_ptr<ITC2007Period> > > &value);

    friend std::ostream& operator<<(std::ostream& _os, const TimetableProblemData& _timetableProblemData);

    // Const versions of getters
    std::vector<boost::shared_ptr<Constraint> > const& getHardConstraints() const;
    // Non-const versions of getters
    std::vector<boost::shared_ptr<Constraint> > & getHardConstraints();
    // Setters
    void setHardConstraints(const std::vector<boost::shared_ptr<Constraint> > &value);

private:

    //--
    // Auxiliary methods
    //--
    void computeConflictMatrixDensity() {
        // Compute conflict matrix density

        // Count the number of non-zero elements
        int nonZeroElements = 0;
        for (int i = 0; i < conflictMatrix->getNumLines(); ++i) {
            for (int j = 0; j < conflictMatrix->getNumCols(); ++j) {
                if (conflictMatrix->getVal(i,j) != 0)
                    ++nonZeroElements;
            }
        }

        // The ‘conflict’ density is the ratio of the number of non-zero elements
        // in the conflict matrix to the total number of conflict matrix elements.
        double numMatrixElements = conflictMatrix->getNumCols() * conflictMatrix->getNumCols();
        // Do not consider the matrix diagonal
        conflictMatrixDensity = nonZeroElements / (numMatrixElements - conflictMatrix->getNumCols());
    }


    //--
    // Fields
    //--
    // Number of Periods
    int numPeriods;
    // Number of Rooms
    int numRooms;
    // Number of students
    int numStudents;
    // Number of exams
    int numExams;
    // Number of enrolments
    int numEnrolments;
    // Conflict matrix density
    double conflictMatrixDensity;
    // Conflict matrix
    boost::shared_ptr<IntMatrix > conflictMatrix;
    // Graph
    boost::shared_ptr<AdjacencyList> examGraph;
    // Vector to keep course total students. Exams indexed from [0..numExams-1].
    boost::shared_ptr<std::vector<int> > courseClassSize;
    // Sorted vector containing exams sorted by class size and by earliest index.
    // This field is instantiated when evaluating the soft constraints
    boost::shared_ptr<std::vector<std::pair<int, int> > > sortedCourseClassSize;
    // InstitutionalModelWeightings
    InstitutionalModelWeightings model_weightings;
    //--
    // For efficient lookup purposes
    //--
    // Exams vector
    boost::shared_ptr<std::vector<boost::shared_ptr<Exam> > >examVector;
    // Rooms vector
    boost::shared_ptr<std::vector<boost::shared_ptr<Room> > >roomVector;
    // Periods vector
    boost::shared_ptr<std::vector<boost::shared_ptr<ITC2007Period> > > periodVector;
    // Hard and Soft constraints
    std::vector<boost::shared_ptr<Constraint> > hardConstraints;
    std::vector<boost::shared_ptr<Constraint> > softConstraints;
};


inline int TimetableProblemData::getNumPeriods() const
{
    return numPeriods;
}
inline void TimetableProblemData::setNumPeriods(int value)
{
    numPeriods = value;
}

inline int TimetableProblemData::getNumRooms() const
{
    return numRooms;
}
inline void TimetableProblemData::setNumRooms(int value)
{
    numRooms = value;
}

inline int TimetableProblemData::getNumStudents() const
{
    return numStudents;
}
inline void TimetableProblemData::setNumStudents(int value)
{
    numStudents = value;
}

inline int TimetableProblemData::getNumExams() const
{
    return numExams;
}
inline void TimetableProblemData::setNumExams(int value)
{
    numExams = value;
}

inline int TimetableProblemData::getNumEnrolments() const
{
    return numEnrolments;
}
inline void TimetableProblemData::setNumEnrolments(int value)
{
    numEnrolments = value;
}

inline double TimetableProblemData::getConflictMatrixDensity() const
{
    return conflictMatrixDensity;
}
inline void TimetableProblemData::setConflictMatrixDensity(double value) {
    conflictMatrixDensity = value;
}

inline const IntMatrix &TimetableProblemData::getConflictMatrix() const
{
    return *conflictMatrix.get();
}
inline void TimetableProblemData::setConflictMatrix(const boost::shared_ptr<IntMatrix > &value)
{
    // Set conflict matrix
    conflictMatrix = value;
    // Compute conflict matrix density
    computeConflictMatrixDensity();
}

inline AdjacencyList const& TimetableProblemData::getExamGraph() const
{
    return *examGraph.get();
}
inline void TimetableProblemData::setExamGraph(const boost::shared_ptr<AdjacencyList> &value)
{
    examGraph = value;
}

// Get/set course class sizes
inline std::vector<int> const& TimetableProblemData::getCourseClassSize() const
{
    return *courseClassSize.get();
}
inline void TimetableProblemData::setCourseClassSize(const boost::shared_ptr<std::vector<int> > &_courseClassSize){
    courseClassSize = _courseClassSize;
}

// Get/set sorted course class sizes
inline std::vector<std::pair<int,int> > const& TimetableProblemData::getSortedCourseClassSize() const {
    return *sortedCourseClassSize.get();
}
inline void TimetableProblemData::setSortedCourseClassSize(
        const boost::shared_ptr<std::vector<std::pair<int,int> > > &_sortedCourseClassSize) {
    sortedCourseClassSize = _sortedCourseClassSize;
}

// Get/set InstitutionalModelWeightings
inline InstitutionalModelWeightings const &TimetableProblemData::getInstitutionalModelWeightings() const {
    return model_weightings;
}
inline void TimetableProblemData::setInstitutionalModelWeightings(InstitutionalModelWeightings const &_model_weightings) {
    model_weightings = _model_weightings;
}

inline std::vector<boost::shared_ptr<Exam> > const& TimetableProblemData::getExamVector() const
{
    return *examVector.get();
}
inline void TimetableProblemData::setExamVector(const boost::shared_ptr<std::vector<boost::shared_ptr<Exam> > > &value)
{
    examVector = value;
}

inline std::vector<boost::shared_ptr<Room> > const& TimetableProblemData::getRoomVector() const
{
    return *roomVector.get();
}
inline void TimetableProblemData::setRoomVector(const boost::shared_ptr<std::vector<boost::shared_ptr<Room> > > &value)
{
    roomVector = value;
}

inline const std::vector<boost::shared_ptr<ITC2007Period> > &TimetableProblemData::getPeriodVector() const
{
    return *periodVector.get();
}
inline void TimetableProblemData::setPeriodVector(const boost::shared_ptr<std::vector<boost::shared_ptr<ITC2007Period> > > &value)
{
    periodVector = value;
}

// Const versions


inline std::vector<boost::shared_ptr<Constraint> > const& TimetableProblemData::getHardConstraints() const
{
    return hardConstraints;
}


// Non-const versions

inline std::vector<boost::shared_ptr<Constraint> > &TimetableProblemData::getHardConstraints()
{
    return hardConstraints;
}

inline void TimetableProblemData::setHardConstraints(const std::vector<boost::shared_ptr<Constraint> > &value)
{
    hardConstraints = value;
}

#endif // TIMETABLEPROBLEMDATA_H











