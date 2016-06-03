#ifndef EXAM_H
#define EXAM_H

#include <vector>
#include <boost/shared_ptr.hpp>
#include "containers/TimetableContainer.h"

//#define EXAM_DEBUG


class Constraint;


class Exam {

public:
    // Constructors
    /**
     * @brief Exam
     * @param _id
     */
    inline Exam(int _id);
    /**
     * @brief Exam
     * @param _id
     * @param _numStudents
     * @param _duration
     */
    inline Exam(int _id, int _numStudents, int _duration);

    // Public interface
    /**
     * @brief getId
     * @return
     */
    inline int getId() const;
    /**
     * @brief getDuration
     * @return
     */
    inline int getDuration() const;
    /**
     * @brief getNumStudents
     * @return
     */
    inline int getNumStudents() const;
    /**
     * @brief insertPeriodRelatedHardConstraint
     * @param _hardConstraint
     */
    inline void insertPeriodRelatedHardConstraint(boost::shared_ptr<Constraint> _hardConstraint);
    /**
     * @brief getPeriodRelatedHardConstraints
     * @return
     */
    inline std::vector<boost::shared_ptr<Constraint> > const &getPeriodRelatedHardConstraints() const;
    /**
     * @brief insertRoomRelatedHardConstraint
     * @param _hardConstraint
     */
    inline void insertRoomRelatedHardConstraint(boost::shared_ptr<Constraint> _hardConstraint);
    /**
     * @brief getRoomRelatedHardConstraints
     * @return
     */
    inline std::vector<boost::shared_ptr<Constraint> > const &getRoomRelatedHardConstraints() const;

private:
    // Fields
    /**
     * @brief id
     */
    int id;
    /**
     * @brief numStudents
     */
    int numStudents;
    /**
     * @brief duration
     */
    int duration;
    /**
     * @brief periodRelatedHardConstraints
     */
    std::vector<boost::shared_ptr<Constraint> > periodRelatedHardConstraints;
    /**
     * @brief roomRelatedHardConstraints
     */
    std::vector<boost::shared_ptr<Constraint> > roomRelatedHardConstraints;
};


/**
 * @brief Exam
 * @param _id
 */
Exam::Exam(int _id)
    : id(_id), numStudents(0), duration(0),
      periodRelatedHardConstraints(0), roomRelatedHardConstraints(0)
{ }

/**
 * @brief Exam
 * @param _id
 * @param _numStudents
 * @param _duration
 */
Exam::Exam(int _id, int _numStudents, int _duration)
    : id(_id), numStudents(_numStudents), duration(_duration),
    periodRelatedHardConstraints(0), roomRelatedHardConstraints(0)
{ }

/**
 * @brief Exam::getId
 * @return
 */
int Exam::getId() const {
    return id;
}
/**
 * @brief Exam::getNumStudents
 * @return
 */
int Exam::getNumStudents() const {
    return numStudents;
}
/**
 * @brief Exam::getDuration
 * @return
 */
int Exam::getDuration() const {
    return duration;
}

/**
 * @brief Exam::insertPeriodRelatedHardConstraint
 * @param hardConstraint
 */
void Exam::insertPeriodRelatedHardConstraint(boost::shared_ptr<Constraint> _hardConstraint) {
    // Insert Period-related hard constraint into internal vector
    periodRelatedHardConstraints.push_back(_hardConstraint);
}
/**
 * @brief Exam::getPeriodRelatedHardConstraints
 * @return
 */
const std::vector<boost::shared_ptr<Constraint> > &Exam::getPeriodRelatedHardConstraints() const {
    return periodRelatedHardConstraints;
}

/**
 * @brief Exam::insertRoomRelatedHardConstraint
 * @param hardConstraint
 */
void Exam::insertRoomRelatedHardConstraint(boost::shared_ptr<Constraint> _hardConstraint) {
    // Insert Room-related hard constraint into internal vector
    roomRelatedHardConstraints.push_back(_hardConstraint);
}
/**
 * @brief Exam::getRoomRelatedHardConstraints
 * @return
 */
const std::vector<boost::shared_ptr<Constraint> > &Exam::getRoomRelatedHardConstraints() const {
    return roomRelatedHardConstraints;
}


#endif // EXAM_H














