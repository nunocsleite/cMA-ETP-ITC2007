#ifndef ITC07TESTSET_H
#define ITC07TESTSET_H

#include "data/ITC2007Constraints.hpp"
#include "TestSet.h"
#include <string>
#include "utils/CurrentDateTime.h"
#include "data/Exam.h"
#include "data/Period.h"
#include "data/Room.h"
#include "data/ITC2007Period.h"
#include "data/ConstraintValidator.hpp"
#include <boost/tokenizer.hpp>




class ITC2007TestSet : public TestSet {

public:
    // Constructor
    ITC2007TestSet(std::string _testSetName, std::string _description, std::string _rootDir)
        : TestSet(_testSetName, _description, _rootDir, boost::shared_ptr<TimetableProblemData>(new TimetableProblemData()))
    {

//        cout << "ITC07TestSet ctor" << endl;

        // After creating the instance, the caller should invoke the load method
    }


public:
    // Overriden method
    virtual void load() override;

protected:
    // Read exams and students
    void readExams(boost::tokenizer<boost::escaped_list_separator<char> >::iterator& it,
                   boost::tokenizer<boost::escaped_list_separator<char> > const& tok);
    // Read periods
    void readPeriods(boost::tokenizer<boost::escaped_list_separator<char> >::iterator& it,
                     boost::tokenizer<boost::escaped_list_separator<char> > const& tok);
    // Read rooms
    void readRooms(boost::tokenizer<boost::escaped_list_separator<char> >::iterator& it,
                   boost::tokenizer<boost::escaped_list_separator<char> > const& tok);
    // Read constraints and weightings
    void readConstraints(boost::tokenizer<boost::escaped_list_separator<char> >::iterator& it,
                         boost::tokenizer<boost::escaped_list_separator<char> > const& tok);


    ///////////////////////////
    // Auxiliary methods
    ///////////////////////////
    int matchPeriods(boost::tokenizer<boost::escaped_list_separator<char> >::iterator& it);
    int matchPeriodSequenceLine(int _periodId, boost::tokenizer<boost::escaped_list_separator<char> >::iterator& _it,
                                boost::shared_ptr<std::vector<boost::shared_ptr<ITC2007Period> > > &_periodVector);

    int matchRooms(boost::tokenizer<boost::escaped_list_separator<char> >::iterator& it);
    int matchRoomSequenceLine(int _roomId, boost::tokenizer<boost::escaped_list_separator<char> >::iterator& _it,
                              boost::shared_ptr<std::vector<boost::shared_ptr<Room> > > &_roomVector);

    void readPeriodHardConstraints(boost::tokenizer<boost::escaped_list_separator<char> >::iterator& it,
                                   std::vector<boost::shared_ptr<Constraint> > &hardConstraints);
    bool readPeriodConstraint(boost::tokenizer<boost::escaped_list_separator<char> >::iterator& it,
                              std::vector<boost::shared_ptr<Constraint> >& hardConstraints);

    void readRoomHardConstraints(boost::tokenizer<boost::escaped_list_separator<char> >::iterator& it,
                                  std::vector<boost::shared_ptr<Constraint> >& hardConstraints);
    bool readRoomConstraint(boost::tokenizer<boost::escaped_list_separator<char> >::iterator& it,
                            std::vector<boost::shared_ptr<Constraint> >& hardConstraints);

    void readInstitutionalWeightingsSoftConstraints(boost::tokenizer<boost::escaped_list_separator<char> >::iterator& it,
                                                 InstitutionalModelWeightings &_model_weightings);
    bool readInstitutionalWeightingsConstraint(boost::tokenizer<boost::escaped_list_separator<char> >::iterator& it,
                                                 InstitutionalModelWeightings &_model_weightings);

    void buildStudentMap(boost::tokenizer<boost::escaped_list_separator<char> >::iterator &_it,
                         const boost::tokenizer<boost::escaped_list_separator<char> > &_tok,
                         boost::unordered_map<int, std::vector<int> > & _studentMap);

    void buildConflictMatrix(boost::unordered_map<int, std::vector<int> > const& _studentMap);

    void buildExamGraph(const IntMatrix &conflictMatrix);

};


#endif // ITC07TESTSET_H
