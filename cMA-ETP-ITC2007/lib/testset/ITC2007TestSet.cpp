#include "ITC2007TestSet.h"
#include <boost/regex.hpp>
#include <boost/unordered_map.hpp>
#include "utils/DateTime.h"


//#define ITC2007TESTSET_DEBUG



using namespace boost;
using namespace std;


////
/// \brief ITC2007TestSet::load
/// \param _testSetName
/// \param _rootDir
///
void ITC2007TestSet::load() {

    //Input Format

    //The problem instance files have the following format;

    //Number of Exams:
    //Number of exams to be timetabled e.g. [Exams:2176]. As with all other entities the numbering of Exams starts at 0.
    //Sequence of lines detailing information regarding each exam:
    //The first number is the duration of the examination (specified in minutes). This is followed by the student numbers of
    //those students taking the exam. Students numbers are integers starting with 0.
    //The line ends with a return character and line feed and is comma separated.

    //Number of Periods:
    //Number of Periods specified within the Timetabling Session e.g. [Periods:42]
    //Sequence of lines detailing Period Dates, Times, Durations associated Penalty:
    //E.g.  31:05:2005, 09:00:00, 180, 0.   The Date is in a standard date format. The time is in 24 hour format, the duration
    //is in minutes and the penalty has a value which is a positive integer. It should be noted that 0 represents no penalty.
    //In relation to other periods a relative value may be present here which indicates that a penalty is added when this period
    //is used by placing an exam. The higher the relative value the least the period should be used.

    //Number of Rooms:
    //Number of Rooms to be used e.g.. [Rooms:10]
    //Sequence of lines detailing room capacity and associated penalty:
    //Room penalty has a value which is a positive integer. It should be noted that 0 represents no penalty. In relation to other
    //rooms a relative value may be present here which indicates that a penalty is added when this room is used by placing an exam.
    //The higher the relative value the least the room should be used.
    //E.g. 500, 7
    //Rooms are numbered in the order from the problem file, starting at zero

    //Period Related Hard Constraints
    //This section begins with the tag [PeriodHardConstraints] and provides data on conditions which are necessary for a feasible solution.
    //These are EXAM_COINCIDENCE, EXCLUSION, and AFTER.  It should be noted that not all these may be present for any one data set.
    //E.g.
    //0, EXAM_COINCIDENCE, 1   Exam ‘0' and Exam ‘1' should be timetabled in the same period. If two exams are set associated in
    //                         this manner yet 'clash' with each other due to student enrolment, this hard constraint is ignored.
    //0, EXCLUSION, 2          Exam ‘0' and Exam ‘2' should be not be timetabled in the same period
    //0, AFTER, 3              Exam ‘0' should be timetabled after Exam ‘3'

    //Room Related Hard Constraints
    //This section begins with the line [RoomHardConstraints] and provides data on conditions which are necessary for a feasible solution.
    //This is
    //ROOM_EXCLUSIVE.  An exam must be timetabled in a room by itself e.g.
    //2, ROOM_EXCLUSIVE      Exam ‘2' must be timetabled in a room by itself.


    //Institutional Model Weightings
    //This section begins with the line [InstitutionalWeightings] and provides information on values given
    //to 'global' soft constraints.

    //TWOINAROW, 7
    //TWOINADAY, 5
    //PERIODSPREAD, 3
    //NONMIXEDDURATIONS, 10
    //FRONTLOAD, 100, 30, 5

    //These are all fully explained and illustrated in the evaluation section.

    //It should be noted that the format of the input file should not be altered in any way.
    //In addition, it is recomended that competitors should ignore unknown lines in the provided format.

    std::string filename = this->getRootDirectory() + "/" + this->getName();
    std::string wholeFile = read_from_file(filename.c_str());
    // Get line tokenizer
    tokenizer<escaped_list_separator<char> > tok(wholeFile, escaped_list_separator<char>('\\', '\r')); // Lines terminate with \r\n
    // Get line iterator
    tokenizer<escaped_list_separator<char> >::iterator it = tok.begin();
    // Read exams and students
    readExams(it, tok);
    // Read periods
    readPeriods(it, tok);
    // Read rooms
    readRooms(it, tok);
    // Read constraints and weightings
    readConstraints(it, tok);
}


/////////////////////////////////////
// Read exams and students
/////////////////////////////////////

////
/// \brief ITC2007TestSet::readExams
/// \param it
/// \param tok
///
void ITC2007TestSet::readExams(tokenizer<escaped_list_separator<char> >::iterator& it,
                               tokenizer<escaped_list_separator<char> > const& tok) {
    //
    // The problem instance files have the following format;
    // Number of Exams:
    // Number of exams to be timetabled e.g. [Exams:2176]. As with all other entities the numbering of Exams starts at 0.
    // Sequence of lines detailing information regarding each exam:
    // The first number is the duration of the examination (specified in minutes). This is followed by the student numbers of
    // those students taking the exam. Students numbers are integers starting with 0.
    // The line ends with a return character and line feed and is comma separated.
    ////

    // Number of exams
    int numExams = 0;
    if (it != tok.end()) {
        // Read number of exams
        std::string s(*it);
        boost::cmatch matches;
        boost::regex re("(\\[Exams:)(\\d+)(\\])");
        // If there's a match
        if (boost::regex_match(s.c_str(), matches, re)) {
             // Extract digits from std::string. The digits are the third subexpression, index = 2.
            std::string match(matches[2].first, matches[2].second);
            numExams = atoi(match.c_str());
#ifdef ITC2007TESTSET_DEBUG
            cout << "\tnumExams: " << numExams << endl;
#endif
            // Set number of exams
            timetableProblemData->setNumExams(numExams);
            //////////////////////////////////////////
            //
            // Build Student map, Conflict matrix, and exam graph
            //
            //////////////////////////////////////////
            //
            // Define student map containing the list of exams for each student
            //
            boost::unordered_map<int, std::vector<int> > studentMap;
            // Build Student map
            buildStudentMap(it, tok, studentMap);
            // Build Conflict matrix,
            buildConflictMatrix(studentMap);
            // Build exam graph representing exam relations
            buildExamGraph(timetableProblemData.get()->getConflictMatrix());

#ifdef ITC2007TESTSET_DEBUG
            //////////////////////////////////////////////////////////////////////////
            //
            // Verification of conflict matrix and exam graph integrity
            //
            //////////////////////////////////////////////////////////////////////////
            IntMatrix const& conflictMatrix = timetableProblemData.get()->getConflictMatrix();
            cout << "nlines = " << conflictMatrix.getNumLines() << endl;
            cout << "ncols = " << conflictMatrix.getNumCols() << endl;
            // Count the number of non-zero elements
            int nonZeroElements = 0;
            // Verify if it's symmetric
            for (int i = 0; i < conflictMatrix.getNumLines(); ++i) {
                for (int j = 0; j < conflictMatrix.getNumCols(); ++j) {
                    if (conflictMatrix.getVal(i,j) != 0)
                        ++nonZeroElements;

                    if (conflictMatrix.getVal(i, j) != conflictMatrix.getVal(j, i))
                        throw runtime_error("Not symmetric");
                }
            }
            // Print the conflict matrix density
            double conflictDensity = timetableProblemData.get()->getConflictMatrixDensity();
            cout << "conflictDensity = " << conflictDensity << endl;
            cout << "conflictDensity [%] = " << setprecision(3) << (conflictDensity * 100) << endl;

            // Verification of exam graph integrity.
            // # conflicts between exams. Should be equal to the number of non-zero elements in the conflict matrix
            int countNumConflicts = 0;
            // Get exam graph
            AdjacencyList const& examGraph = timetableProblemData.get()->getExamGraph();
            property_map<AdjacencyList, vertex_index_t>::type index_map = get(vertex_index, examGraph);
            graph_traits<AdjacencyList>::adjacency_iterator ai, a_end;
            // Iterate over all exams and check if the number of edges in the exam graph
            // correspond to the edeges in the conflict matrix
            for (int ei = 0; ei < numExams; ++ei) {
                // Get ei adjacent exams
                boost::tie(ai, a_end) = adjacent_vertices(ei, examGraph);
                for (; ai != a_end; ++ai) {
                    // Get adjacent exam
                    int ej = get(index_map, *ai);
                    // Verify if there's a conflict between ei and ej
                    if (conflictMatrix.getVal(ei, ej) > 0)
                        ++countNumConflicts;
                }
            }
            cout << "nonZeroElements = " << nonZeroElements << ", countNumConflicts = " << countNumConflicts << endl;
            if (nonZeroElements != countNumConflicts)
                throw runtime_error("Error in exam graph integrity");
#endif

        }
        else
        {
            cout << "[readExams] Error parsing input file. The regexp \"" << re << "\" does not match \"" << s << "\"" << endl;
            cin.get();
        }
    }
    else
        throw runtime_error("Error while reading ITC07 exams");
}


// Vector of pairs (exam idx, # students) for keeping course total students for each exam in a sorted manner


////
/// \brief ITC2007TestSet::buildConflictMatrix
/// \param it
/// \param tok
///
void ITC2007TestSet::buildStudentMap(boost::tokenizer<boost::escaped_list_separator<char> >::iterator &_it,
                                     const boost::tokenizer<boost::escaped_list_separator<char> > &_tok,
                                     boost::unordered_map<int, vector<int> > & _studentMap) {

    int numExams = timetableProblemData->getNumExams();
    // Vector for keeping course total students. Exams indexed from [0..numExams-1].
    boost::shared_ptr<vector<int> > courseClassSize(new vector<int>(numExams));
    // Vector of pairs (exam idx, # students) sorted by # students
    boost::shared_ptr<vector<pair<int,int> > > sortedCourseClassSize(new vector<pair<int,int> >(numExams));
    // Exam duration
    int examDuration;
    // For checking the # enrolments
    int numEnrolments1 = 0;
    // # enrolments
    int numEnrolments = 0;
    // Create *empty* exam vector
    boost::shared_ptr<vector<boost::shared_ptr<Exam> > > examVector(new vector<boost::shared_ptr<Exam> >());
    // Go to the first line of exams
    ++_it;
    // Parse lines
    for (int i = 0; i < numExams && _it != _tok.end(); ++i, ++_it) {
        std::string line(*_it); // Lines start with \n because it wasn't removed
#ifdef ITC2007TESTSET_DEBUG
//        cout << i << ": " << line << endl;
#endif
        // Get number tokenizer
        tokenizer<escaped_list_separator<char> > numberTok(line, escaped_list_separator<char>('\\', ','));
        // Get number iterator
        tokenizer<escaped_list_separator<char> >::iterator beg = numberTok.begin();
        examDuration = atoi((*beg).c_str());
#ifdef ITC2007TESTSET_DEBUG
//        cout << "examDuration: " << examDuration << " - Students: ";
#endif
        ++beg; // Go to the first student
        // Exam number (start at 0)
        int exam = i;
        // Number of students enrolled in each exam
        int numExamStudents = 0;

        for (; beg != numberTok.end(); ++beg) {
#ifdef ITC2007TESTSET_DEBUG
//            cout << *beg << " ";
#endif
            //////////////////////////////////////////
            // Build student map
            //////////////////////////////////////////
            // Current student
            int student = atoi((*beg).c_str());
            // Insert current exam, given by the index i, in the student map
            // If student doesn't exist yet, create entry <student, exam>
            unordered_map<int, vector<int> >::iterator mapIt = _studentMap.find(student);
            if (mapIt == _studentMap.end()) {
                vector<int> exams;
                exams.push_back(exam);
                _studentMap.insert(pair<int, vector<int> >(student, exams));
            }
            else {
                // Else add exam to the student exams
                mapIt->second.push_back(exam);
            }
            // Increment # students
            ++numExamStudents;
        }
//        cout << endl << endl;

        // Insert total number of students associated to each exam
        (*courseClassSize.get())[exam] = numExamStudents;
        // Insert the above same information in the sorted vector, to be sorted ahead
        (*sortedCourseClassSize.get())[exam] = make_pair(exam, numExamStudents);
        // Instantiate Exam object
        (*examVector.get()).push_back(boost::make_shared<Exam>(exam, numExamStudents, examDuration));
        // Update # enrolments
        numEnrolments1 += numExamStudents;
    }
    int numStudents = 0, numStudents1 = 0, smallest = INT_MAX, greatest = INT_MIN;
    for (auto const&s : _studentMap) {
        //                cout << "Student: " << s.first << " - total exams: " << s.second.size() << endl;
        ++numStudents1;
        smallest = min(smallest, s.first);
        greatest = max(greatest, s.first);
        numEnrolments += s.second.size();
    }
    // Update # students
    numStudents = greatest-smallest+1;

#ifdef ITC2007TESTSET_DEBUG
    // Print student info
    cout << "Print student info" << endl << endl;
    cout << "numStudents = " << numStudents << endl;
    cout << "numStudents1 = " << numStudents1 << endl;
    cout << "smallest = " << smallest << endl;
    cout << "greatest = " << greatest << endl;
    cout << "numEnrolments = " << numEnrolments << endl;

//    // Print total number of students associated to each exam
//    for (const auto& it : *courseClassSizeUnorderedMap.get()) {
//        cout << "Exam = " << it.first << ", # students = " << it.second << endl;
//    }
    cout << "numEnrolments1 = " << numEnrolments1 << endl;

    if (numEnrolments != numEnrolments1)
        throw runtime_error("numEnrolments != numEnrolments1");

    // Print exam vector
    for (const auto& ptrExam : *examVector.get()) {
        cout << "Exam = " << ptrExam->getId()
             << ", Period = " << ptrExam->getPeriod()
             << ", Room = " << ptrExam->getRoom()
             << ", # students = " << ptrExam->getNumStudents()
             << ", duration = " << ptrExam->getDuration()
             << endl;
    }
#endif
    //
    // and set TimetableProblemData fields
    //
    // Set exam vector
    timetableProblemData->setExamVector(examVector);
    // Set number of students
    timetableProblemData.get()->setNumStudents(numStudents);
    // Set number of enrolments
    timetableProblemData.get()->setNumEnrolments(numEnrolments);
    // Set course class size
    timetableProblemData->setCourseClassSize(courseClassSize);

    //-
    // sortedCourseClassSize vector
    //-
    // Sort course class sizes vector.
    // This is a vector of pairs (exam idx, # students) sorted by # students
    std::sort(sortedCourseClassSize.get()->begin(), sortedCourseClassSize.get()->end(),
              [](pair<int,int> const& p1, pair<int,int> const& p2) {
                    return p1.second > p2.second; // Sorted by decreasing order of # students
              });
    // Set sorted course class size
    timetableProblemData->setSortedCourseClassSize(sortedCourseClassSize);
#ifdef ITC2007TESTSET_DEBUG
    for (auto & p : *sortedCourseClassSize.get())
        cout << "exam idx = " << p.first << " - # students " << p.second << endl;
//    cin.get();
#endif
}



/**
 * @brief ITC2007TestSet::buildConflictMatrix Builds the Conflict matrix
 * @param _studentMap Student map containing the list of exams for each student
 * @return
 */
void ITC2007TestSet::buildConflictMatrix(boost::unordered_map<int, vector<int> > const& _studentMap) {
    // # exams
    int numExams = timetableProblemData->getNumExams();
    // Create shared ptr to manage Conflict Matrix
    boost::shared_ptr<IntMatrix> ptrConflictMatrix(new IntMatrix(numExams, numExams));
    // Get Conflict Matrix
    IntMatrix &conflictMatrix = *ptrConflictMatrix.get();
    // Now, fill the conflict matrix and exam graph for each student
    // (each map entry corresponds to one student enrolment data)
    int v1, v2;
    for (auto entry : _studentMap) {
        vector<int>& exams = entry.second;
        int examListSize = exams.size();
        for (int i = 0; i < examListSize; ++i) {
            for (int j = i+1; j < examListSize; ++j) {
                // Get vertices.
                v1 = exams[i];
                v2 = exams[j];
                // Increment by one (one student is enrolled in v1 and v2) arc's cost in
                // the conflict matrix for vertices v1 and v2
                conflictMatrix.incVal(v1, v2);
                conflictMatrix.incVal(v2, v1); // Conflict matrix is symmetric
            }
        }
    }
    // Set conflict matrix in the TimetableProblemData
    timetableProblemData.get()->setConflictMatrix(ptrConflictMatrix);
}



////
/// \brief ITC2007TestSet::buildExamGraph
/// \param conflictMatrix
///
void ITC2007TestSet::buildExamGraph(const IntMatrix &conflictMatrix) {
    // Instantiate graph with ncols vertices
    boost::shared_ptr<AdjacencyList> ptrGraphAux(new AdjacencyList(conflictMatrix.getNumCols()+1));
    // and set TimetableProblemData field
    timetableProblemData.get()->setExamGraph(ptrGraphAux);
    int cost;
    // Vertices start at 0 as specified by the ITC 2007 rules
    for (int v1 = 0; v1 < conflictMatrix.getNumLines(); ++v1) {
        for (int v2 = 0; v2 < conflictMatrix.getNumLines(); ++v2) {
            cost = conflictMatrix.getVal(v1, v2);
            if (cost != 0) {
                if (v2 > v1) {
                    add_edge(v1, v2, *ptrGraphAux.get());
                }
            }
        }
    }
}




/////////////////////////////////////
// Read periods
/////////////////////////////////////

////
/// \brief ITC2007TestSet::readPeriods
/// \param it
/// \param tok
///
void ITC2007TestSet::readPeriods(tokenizer<escaped_list_separator<char> >::iterator& it,
                                 tokenizer<escaped_list_separator<char> > const& tok) {
    //
    // The problem instance files have the following format;
    // Number of Periods:
    // Number of Periods specified within the Timetabling Session e.g. [Periods:42]
    // Sequence of lines detailing Period Dates, Times, Durations and associated Penalty:
    // E.g.  31:05:2005, 09:00:00, 180, 0.   The Date is in a standard date format. The time is in 24 hour format, the duration
    // is in minutes and the penalty has a value which is a positive integer. It should be noted that 0 represents no penalty.
    // In relation to other periods a relative value may be present here which indicates that a penalty is added when this period
    // is used by placing an exam. The higher the relative value the least the period should be used.
    ////

    boost::shared_ptr<vector<boost::shared_ptr<ITC2007Period> > > periodVector(
                new vector<boost::shared_ptr<ITC2007Period> >());

    if (it != tok.end()) {
        // Match period number
        int numPeriods = matchPeriods(it);
        int periodId = 0; // Period index
        ++it; // Read next line
        // Read periods info
        for (int i = 0; i < numPeriods; ++i) {
//            cout << "line " << (i+1) << ":" << endl;
            // Match a sequence line
            matchPeriodSequenceLine(periodId, it, periodVector);
            ++periodId; // Increment period index
            ++it; // Read next line
        }
        //
        // Set TimetableProblemData field
        //
        // Set number of periods
        timetableProblemData.get()->setNumPeriods(numPeriods);
        // Set period vector
        timetableProblemData->setPeriodVector(periodVector);
#ifdef ITC2007TESTSET_DEBUG
        // Processed period information
        cout << "Processed period information: " << endl;
        cout << "numPeriods = " << numPeriods << endl;
        cout << "period vector # entries = " << timetableProblemData->getPeriodVector().size() << endl;
#endif
    }
    else
        throw runtime_error("Error while reading ITC07 periods");
}




////
/// \brief ITC2007TestSet::matchPeriods
/// \param it
/// \return
///
int ITC2007TestSet::matchPeriods(tokenizer<escaped_list_separator<char> >::iterator& it) {
    /////////////////////////////////////////////////
    // Lines start with \n because it wasn't removed
    //
    // Remove first \n in the regexp
    /////////////////////////////////////////////////

    // Number of periods
    int numPeriods = 0;
    // Read number of periods specified within the Timetabling Session e.g. [Periods:42]
    std::string s(*it);
    boost::cmatch matches;
    //        boost::regex re("(\\[Periods:)(\\d+)(\\])");
    boost::regex re("(\\\n)(\\[Periods:)(\\d+)(\\])");

    if (boost::regex_match(s.c_str(), matches, re)) {
        // Extract digits from std::string. The digits are the third subexpression, index = 3.
        std::string match(matches[3].first, matches[3].second);
        numPeriods = atoi(match.c_str());
//        cout << "\tnumPeriods: " << numPeriods << endl;
    }
    else {
        cout << "The regexp \"" << re << "\" does not match \"" << s << "\"" << endl;
        cin.get();
    }
    return numPeriods;
}



////
/// \brief ITC2007TestSet::matchPeriodSequenceLine
/// \param _periodId
/// \param _it
/// \param _periodVector
/// \return
///
int ITC2007TestSet::matchPeriodSequenceLine(int _periodId,
                                            tokenizer<escaped_list_separator<char> >::iterator& _it,
                                            boost::shared_ptr<vector<boost::shared_ptr<ITC2007Period> > >& _periodVector) {

    // Read sequence of lines detailing Period Dates, Times, Durations and associated Penalty:
    //  E.g.  31:05:2005, 09:00:00, 180, 0.
    std::string s(*_it);
    //    std::std::string s("\n15:04:2005, 09:30:00");

    boost::cmatch matches;
    boost::regex re("(\\\n)(\\d{2}):(\\d{2}):(\\d{4}), (\\d{2}):(\\d{2}):(\\d{2}), (\\d+), (\\d+)");

    if (boost::regex_match(s.c_str(), matches, re)) {
        // Extract values from std::string. E.g., the day part is the second subexpression, index = 2.
        std::string matchDay(matches[2].first, matches[2].second);
        int day = atoi(matchDay.c_str());
        std::string matchMonth(matches[3].first, matches[3].second);
        int month = atoi(matchMonth.c_str());
        std::string matchYear(matches[4].first, matches[4].second);
        int year = atoi(matchYear.c_str());

        std::string matchHour(matches[5].first, matches[5].second);
        int hour = atoi(matchHour.c_str());
        std::string matchMinute(matches[6].first, matches[6].second);
        int minute = atoi(matchMinute.c_str());
        std::string matchSecond(matches[7].first, matches[7].second);
        int second = atoi(matchSecond.c_str());

        std::string matchDuration(matches[8].first, matches[8].second);
        int duration = atoi(matchDuration.c_str());

        std::string matchPenalty(matches[9].first, matches[9].second);
        int penalty = atoi(matchPenalty.c_str());
#ifdef ITC2007TESTSET_DEBUG
        cout << "\tday: " << day << endl;
        cout << "\tmonth: " << month << endl;
        cout << "\tyear: " << year << endl;
        cout << "\thour: " << hour << endl;
        cout << "\tminute: " << minute << endl;
        cout << "\tsecond: " << second << endl;
        cout << "\tduration: " << duration << endl;
        cout << "\tpenalty: " << penalty << endl;
        cout << endl;
#endif
        // Create Period instance for keeping period information
        boost::shared_ptr<ITC2007Period> period(new ITC2007Period(_periodId, Date(day, month, year),
                                                                  Time(hour, minute, second), duration, penalty));
        _periodVector->push_back(period);
    }
    else {
        cout << "The regexp \"" << re << "\" does not match \"" << s << "\"" << endl;
        cin.get();
    }
}



/////////////////////////////////////
// Read rooms
/////////////////////////////////////

////
/// \brief ITC2007TestSet::readRooms
/// \param it
/// \param tok
///
void ITC2007TestSet::readRooms(tokenizer<escaped_list_separator<char> >::iterator& it,
                               tokenizer<escaped_list_separator<char> > const& tok) {
    //
    // The problem instance files have the following format;
    // Number of Rooms:
    // Number of Rooms to be used e.g.. [Rooms:10]
    // Sequence of lines detailing room capacity and associated penalty:
    // Room penalty has a value which is a positive integer. It should be noted that 0 represents no penalty.
    // In relation to other rooms a relative value may be present here which indicates that a penalty is added
    // when this room is used by placing an exam.
    // The higher the relative value the least the room should be used.
    // E.g. 500, 7
    // Rooms are numbered in the order from the problem file, starting at zero
    ////

    boost::shared_ptr<vector<boost::shared_ptr<Room> > > roomVector(
                new vector<boost::shared_ptr<Room> >());

    if (it != tok.end()) {
        // Match rooms number
        int numRooms = matchRooms(it);
        int roomId = 0; // Room index
        ++it; // Read next line
        // Read rooms info
        for (int i = 0; i < numRooms; ++i) {
//            cout << "line " << (i+1) << ":" << endl;
            // Match a sequence line
            matchRoomSequenceLine(roomId, it, roomVector);
            ++roomId; // Increment room index
            ++it; // Read next line
        }
        //
        // Set TimetableProblemData field
        //
        // Set number of rooms
        (*timetableProblemData.get()).setNumRooms(numRooms);
        // Set room vector
        timetableProblemData->setRoomVector(roomVector);
#ifdef ITC2007TESTSET_DEBUG
        // Processed period information
        cout << "Processed room information: " << endl;
        cout << "numRooms = " << numRooms << endl;
        cout << "room vector # entries = " << timetableProblemData->getRoomVector().size() << endl;
#endif
    }
    else
        throw runtime_error("Error while reading ITC07 rooms");
}



////
/// \brief ITC2007TestSet::matchRooms
/// \param it
/// \return
///
int ITC2007TestSet::matchRooms(tokenizer<escaped_list_separator<char> >::iterator& it) {
    /////////////////////////////////////////////////
    // Lines start with \n because it wasn't removed
    //
    // Remove first \n in the regexp
    /////////////////////////////////////////////////

    // Number of rooms
    int numRooms = 0;
    // Read number of rooms used e.g. [Rooms:10]
    std::string s(*it);
    boost::cmatch matches;
    //        boost::regex re("(\\[Rooms:)(\\d+)(\\])");
    boost::regex re("(\\\n)(\\[Rooms:)(\\d+)(\\])");

    if (boost::regex_match(s.c_str(), matches, re)) {
        // Extract digits from std::string. The digits are the third subexpression, index = 3.
        std::string match(matches[3].first, matches[3].second);
        numRooms = atoi(match.c_str());
    }
    else {
        cout << "The regexp \"" << re << "\" does not match \"" << s << "\"" << endl;
        cin.get();
    }

    return numRooms;
}



////
/// \brief ITC2007TestSet::matchRoomSequenceLine
/// \param _roomId
/// \param _it
/// \param _roomVector
/// \return
///
int ITC2007TestSet::matchRoomSequenceLine(int _roomId, tokenizer<escaped_list_separator<char> >::iterator& _it,
                                          boost::shared_ptr<vector<boost::shared_ptr<Room> > > &_roomVector) {
    // Read sequence of lines detailing room capacity and associated penalty:
    //  E.g.  260, 0
    std::string s(*_it);
    boost::cmatch matches;
    boost::regex re("(\\\n)(\\d+), (\\d+)");

    if (boost::regex_match(s.c_str(), matches, re)) {
        // Extract values from std::string.
        std::string matchCapacity(matches[2].first, matches[2].second);
        int roomCapacity = atoi(matchCapacity.c_str());

        std::string matchPenalty(matches[3].first, matches[3].second);
        int penalty = atoi(matchPenalty.c_str());
#ifdef ITC2007TESTSET_DEBUG
//        cout << "\tcapacity: " << roomCapacity << endl;
//        cout << "\tpenalty: " << penalty << endl;
//        cout << endl;
#endif
        // Get number of periods
        int numPeriods = timetableProblemData.get()->getNumPeriods();
        // Create Room instance for keeping room information
        boost::shared_ptr<Room> room(new Room(_roomId, roomCapacity, penalty));
        _roomVector->push_back(room);
    }
    else {
        cout << "The regexp \"" << re << "\" does not match \"" << s << "\"" << endl;
        cin.get();
    }
}



/////////////////////////////////////
// Read constraints and weightings
/////////////////////////////////////

////
/// \brief ITC2007TestSet::readConstraints
/// \param it
/// \param tok
///
void ITC2007TestSet::readConstraints(tokenizer<escaped_list_separator<char> >::iterator& it,
                                     tokenizer<escaped_list_separator<char> > const& tok) {

    //
    // The problem instance files have the following format:
    //
    // Period Related Hard Constraints
    //
    // This section begins with the tag [PeriodHardConstraints] and provides data on conditions which are necessary for a feasible solution.
    // These are EXAM_COINCIDENCE, EXCLUSION, and AFTER.  It should be noted that not all these may be present for any one data set.
    // E.g.
    // 0, EXAM_COINCIDENCE, 1   Exam ‘0' and Exam ‘1' should be timetabled in the same period. If two exams are set associated in
    //                          this manner yet 'clash' with each other due to student enrolment, this hard constraint is ignored.
    // 0, EXCLUSION, 2          Exam ‘0' and Exam ‘2' should be not be timetabled in the same period
    // 0, AFTER, 3              Exam ‘0' should be timetabled after Exam ‘3'

    // Room Related Hard Constraints
    //
    // This section begins with the line [RoomHardConstraints] and provides data on conditions which are necessary for a feasible solution.
    // This is
    // ROOM_EXCLUSIVE.  An exam must be timetabled in a room by itself e.g.
    // 2, ROOM_EXCLUSIVE      Exam ‘2' must be timetabled in a room by itself.

    // Institutional Model Weightings
    //
    // This section begins with the line [InstitutionalWeightings] and
    // provides information on values given to 'global' soft constraints.
    //
    // TWOINAROW, 7
    // TWOINADAY, 5
    // PERIODSPREAD, 3
    // NONMIXEDDURATIONS, 10
    // FRONTLOAD, 100, 30, 5
    ////

    // Get hard and soft constraints vectors
    auto &hardConstraints = timetableProblemData.get()->getHardConstraints();
//    auto &softConstraints = timetableProblemData.get()->getSoftConstraints();
    InstitutionalModelWeightings model_weightings;

    if (it != tok.end()) {
        readPeriodHardConstraints(it, hardConstraints);
#ifdef ITC2007TESTSET_DEBUG
        cout << "Finished reading Period hard constraints" << endl;
#endif

        readRoomHardConstraints(it, hardConstraints);
#ifdef ITC2007TESTSET_DEBUG
        cout << "Finished reading Room hard constraints" << endl;
#endif

        // Read Institutional Weightings
        readInstitutionalWeightingsSoftConstraints(it, model_weightings);
#ifdef ITC2007TESTSET_DEBUG
        cout << "Finished reading Institutional Weightings soft constraints" << endl;
#endif

    // Set timetableProblemData Institutional Model Weightings
    timetableProblemData->setInstitutionalModelWeightings(model_weightings);

//        // Read Institutional Weightings soft constraints
//        readInstitutionalWeightingsSoftConstraints(it, softConstraints);
//#ifdef ITC2007TESTSET_DEBUG
//        cout << "Finished reading Institutional Weightings soft constraints" << endl;
//#endif
    }

//    // Add Soft Room penalty and Soft Period penalty constraints
//    boost::shared_ptr<Constraint> roomPenaltysoftConstr(
//                new RoomPenaltySoftConstraint()
//                );
//    softConstraints.push_back(roomPenaltysoftConstr);

//    boost::shared_ptr<Constraint> periodPenaltysoftConstr(
//                new PeriodPenaltySoftConstraint()
//                );
//    softConstraints.push_back(periodPenaltysoftConstr);
}



////
/// \brief ITC2007TestSet::readPeriodHardConstraints
/// \param it
/// \param hardConstraints
///
void ITC2007TestSet::readPeriodHardConstraints(tokenizer<escaped_list_separator<char> >::iterator& it,
                                               vector<boost::shared_ptr<Constraint> > &hardConstraints) {
    std::string s(*it);
    boost::cmatch matches;
    // Section begins with the tag [PeriodHardConstraints]
    boost::regex re("(\\\n)(\\[PeriodHardConstraints\\])");
    if (!boost::regex_match(s.c_str(), matches, re))
    {
        cout << "The regexp \"" << re << "\" does not match \"" << s << "\"" << endl;
        // throw exception
        throw runtime_error("No PeriodHardConstraints specified");
    }
#ifdef ITC2007TESTSET_DEBUG
    cout << "Read period hard constraints header" << endl;
#endif
    ++it; // Proceed to read the next line

    // Read constraints if any.
    // If line starts with a number then is a Period hard constraint
    while (readPeriodConstraint(it, hardConstraints))
        ++it;
}



////
/// \brief ITC2007TestSet::readPeriodConstraint
/// \param it
/// \param hardConstraints
/// \return
///
bool ITC2007TestSet::readPeriodConstraint(tokenizer<escaped_list_separator<char> >::iterator& it,
                                          vector<boost::shared_ptr<Constraint> >& hardConstraints) {
    std::string s(*it);
    boost::cmatch matches;
    boost::regex re("(\\\n)(\\d+), (AFTER|EXAM_COINCIDENCE|EXCLUSION), (\\d+)");

    if (boost::regex_match(s.c_str(), matches, re)) {
        // Extract values from std::string.
        // Read Exam 1
        std::string matchExam1(matches[2].first, matches[2].second);
        int exam1 = atoi(matchExam1.c_str());
        // Read Constraint type
        std::string constraintType(matches[3].first, matches[3].second);
        // Read Exam 2
        std::string matchExam2(matches[4].first, matches[4].second);
        int exam2 = atoi(matchExam2.c_str());
#ifdef ITC2007TESTSET_DEBUG
        cout << "\texam1: " << exam1 << endl;
        cout << "\ttype of constraint: " << constraintType << endl;
        cout << "\texam2: " << exam2 << endl;
        cout << endl;
#endif

///
/// TODO: Improve code generality
///

        auto const& examVector = this->getTimetableProblemData()->getExamVector();

        // Period Related Hard Constraints
        if (constraintType == "EXAM_COINCIDENCE") {
            boost::shared_ptr<Constraint> hardConstr(
                        new ExamCoincidenceConstraint(exam1, exam2)
                        );
            hardConstraints.push_back(hardConstr);
            // Insert this hard constraint into each associated exam, 'exam1' and 'exam2'
            examVector[exam1]->insertPeriodRelatedHardConstraint(hardConstr);
            examVector[exam2]->insertPeriodRelatedHardConstraint(hardConstr);
        }
        else if (constraintType == "EXCLUSION") {
            boost::shared_ptr<Constraint> hardConstr(
                        new ExamExclusionConstraint(exam1, exam2)
                        );
            hardConstraints.push_back(hardConstr);
            // Insert this hard constraint into each associated exam, 'exam1' and 'exam2'
            examVector[exam1]->insertPeriodRelatedHardConstraint(hardConstr);
            examVector[exam2]->insertPeriodRelatedHardConstraint(hardConstr);
        }
        else if (constraintType == "AFTER") {
            boost::shared_ptr<Constraint> hardConstr(
                        new AfterConstraint(exam1, exam2)
                        );
            hardConstraints.push_back(hardConstr);
            // Insert this hard constraint into each associated exam, 'exam1' and 'exam2'
            examVector[exam1]->insertPeriodRelatedHardConstraint(hardConstr);
            examVector[exam2]->insertPeriodRelatedHardConstraint(hardConstr);
        }
    }
    else {
//        cout << "The regexp \"" << re << "\" does not match \"" << s << "\"" << endl;
        //        cin.get();
        return false; // It couldn't read a period constraint.
        // There are two possibilities:
        // (1) the file format is incorrect,
        // (2) Read the header of Room hard constraint
    }
    return true;
}



/////
/// \brief ITC07TestSet::readRoomHardConstraints
/// \param it
/// \param hardConstraints
///
void ITC2007TestSet::readRoomHardConstraints(tokenizer<escaped_list_separator<char> >::iterator& it,
                                             vector<boost::shared_ptr<Constraint> >& hardConstraints) {
    std::string s(*it);
    boost::cmatch matches;
    // Room Related Hard Constraints
    //
    // This section begins with the line [RoomHardConstraints] and provides data on conditions which are
    // necessary for a feasible solution.
    // This is
    // ROOM_EXCLUSIVE.  An exam must be timetabled in a room by itself e.g.
    // 2, ROOM_EXCLUSIVE      Exam ‘2' must be timetabled in a room by itself.
    boost::regex re("(\\\n)(\\[RoomHardConstraints\\])");
    if (!boost::regex_match(s.c_str(), matches, re))
    {
        cout << "The regexp \"" << re << "\" does not match \"" << s << "\"" << endl;
        // throw exception
        throw runtime_error("No RoomHardConstraints specified");
    }
#ifdef ITC2007TESTSET_DEBUG
    cout << "Read room hard constraints header" << endl;
#endif
    ++it; // Proceed to read the next line

    // Read constraints if any.
    // If line starts with a number then is a Room hard constraint
    while (readRoomConstraint(it, hardConstraints))
        ++it;
}



/////
/// \brief ITC07TestSet::readRoomConstraint
/// \param it
/// \param hardConstraints
/// \return
///
bool ITC2007TestSet::readRoomConstraint(tokenizer<escaped_list_separator<char> >::iterator& it,
                                        vector<boost::shared_ptr<Constraint> >& hardConstraints) {
    std::string s(*it);
    boost::cmatch matches;
    boost::regex re("(\\\n)(\\d+), (ROOM_EXCLUSIVE)");

    if (boost::regex_match(s.c_str(), matches, re)) {
        // Extract values from std::string.
        // Read Exam
        std::string matchExam(matches[2].first, matches[2].second);
        int exam = atoi(matchExam.c_str());
        // Read Constraint type
        std::string constraintType(matches[3].first, matches[3].second);
#ifdef ITC2007TESTSET_DEBUG
        cout << "\texam: " << exam << endl;
        cout << "\ttype of constraint: " << constraintType << endl;
        cout << endl;
#endif
        auto const& examVector = this->getTimetableProblemData()->getExamVector();

        ///
        /// Improve code generality
        ///
        // Room Related Hard Constraints
        if (constraintType == "ROOM_EXCLUSIVE") {
            boost::shared_ptr<Constraint> hardConstr(
                        new RoomExclusiveConstraint(exam)
                        );
            hardConstraints.push_back(hardConstr);
            // Insert this hard constraint into the associated exam, 'exam'
            examVector[exam]->insertRoomRelatedHardConstraint(hardConstr);
        }
    }
    else {
//        cout << "The regexp \"" << re << "\" does not match \"" << s << "\"" << endl;
        return false; // It couldn't read a room constraint.
        // There are two possibilities:
        // (1) the file format is incorrect,
        // (2) Read the header of InstitutionalWeightings
    }
    return true;
}






////
/// \brief ITC2007TestSet::readInstitutionalWeightingsSoftConstraints
/// \param it
/// \param softConstraints
///
void ITC2007TestSet::readInstitutionalWeightingsSoftConstraints(tokenizer<escaped_list_separator<char> >::iterator& it,
                                                                InstitutionalModelWeightings &_model_weightings) {
    // Institutional Model Weightings
    //
    // This section begins with the line [InstitutionalWeightings] and
    // provides information on values given to 'global' soft constraints.
    //
    // TWOINAROW, 7
    // TWOINADAY, 5
    // PERIODSPREAD, 3
    // NONMIXEDDURATIONS, 10
    // FRONTLOAD, 100, 30, 5
    //
    std::string s(*it);
    boost::cmatch matches;
    // Section begins with the tag [InstitutionalWeightings]
    boost::regex re("(\\\n)(\\[InstitutionalWeightings\\])");
    if (!boost::regex_match(s.c_str(), matches, re))
    {
        cout << "The regexp \"" << re << "\" does not match \"" << s << "\"" << endl;
        // throw exception
        throw runtime_error("No InstitutionalWeightings specified");
    }
#ifdef ITC2007TESTSET_DEBUG
    cout << "Read [InstitutionalWeightings] header" << endl;
#endif
    ++it; // Proceed to read the next line

    // Read constraints if any.
    while (readInstitutionalWeightingsConstraint(it, _model_weightings))
        ++it;
}







////
/// \brief ITC2007TestSet::readInstitutionalWeightingsConstraint
/// \param it
/// \param softConstraints
/// \return
///
bool ITC2007TestSet::readInstitutionalWeightingsConstraint(tokenizer<escaped_list_separator<char> >::iterator& it,
                                                           InstitutionalModelWeightings &_model_weightings) {
    std::string s(*it);
    boost::cmatch matches;

    ///
    /// Improve code generality and regex parsing robustness
    ///

    // E. g.
    // TWOINAROW, 7
    // TWOINADAY, 5
    // PERIODSPREAD, 3
    // NONMIXEDDURATIONS, 10
    // FRONTLOAD, 100, 30, 5
    boost::regex re("(\\\n)(((TWOINAROW|TWOINADAY|PERIODSPREAD|NONMIXEDDURATIONS),(\\s*)(\\d+))|((FRONTLOAD),(\\s*)(\\d+),(\\s*)(\\d+),(\\s*)(\\d+)))");

    if (boost::regex_match(s.c_str(), matches, re)) {
        // Extract values from std::string.
        //
        // In the case of the constraints:
        //  TWOINAROW, 7
        //  TWOINADAY, 5
        //  PERIODSPREAD, 3
        //  NONMIXEDDURATIONS, 10
        //
        // matches[4] and matches[6] contain the <constraint type> and <id> fields.
        //
        // In the constraint:
        // FRONTLOAD, 100, 30, 5
        //
        // matches[8], matches[10], matches[12], and matches[14] contain the <constraint type> and <id1>,<id2>,<id3> fields.
        //
        //  Input line containing constraint info
        std::string inputLine(matches[2].first, matches[2].second);

        if (inputLine.find("FRONTLOAD") != std::string::npos) {
            // Constraint type
            std::string constraintType(matches[8].first, matches[8].second);
            // Id1
            std::string matchId1(matches[10].first, matches[10].second);
            int id1 = atoi(matchId1.c_str());
            // Id2
            std::string matchId2(matches[12].first, matches[12].second);
            int id2 = atoi(matchId2.c_str());
            // Id3
            std::string matchId3(matches[14].first, matches[14].second);
            int id3 = atoi(matchId3.c_str());
#ifdef ITC2007TESTSET_DEBUG
            cout << "\ttype of constraint: " << constraintType << endl;
            cout << "\tid1: " << id1 << endl;
            cout << "\tid2: " << id2 << endl;
            cout << "\tid3: " << id3 << endl;
            cout << endl;
#endif
            // FRONTLOAD soft constraint
            if (constraintType == "FRONTLOAD") {
                // First parameter  = number of largest exams. Largest exams are specified by class size
                // Second parameter = number of last periods to take into account
                // Third parameter  = the penalty or weighting
                int numberLargestExams = id1;
                int numberLastPeriods = id2;
                int weightFL = id3;
                // Set fields in _model_weightings instance
                _model_weightings.front_load = { numberLargestExams, numberLastPeriods, weightFL };
            }
        }
        else {
            // Constraint type
            std::string constraintType(matches[4].first, matches[4].second);
            // Id
            std::string matchId(matches[6].first, matches[6].second);
            int id = atoi(matchId.c_str());
#ifdef ITC2007TESTSET_DEBUG
            cout << "\ttype of constraint: " << constraintType << endl;
            cout << "\tid: " << id << endl;
#endif
            // TWOINAROW soft constraint
            if (constraintType == "TWOINAROW") {
                int weight2R = id;
                // Set fields in _model_weightings instance
                _model_weightings.two_in_a_row = weight2R;
            }
            else if (constraintType == "TWOINADAY") { // TWOINADAY soft constraint
                int weight2D = id;
                // Set fields in _model_weightings instance
                _model_weightings.two_in_a_day = weight2D;
            }
            else if (constraintType == "PERIODSPREAD") { // PERIODSPREAD soft constraint
                int weightPS = id;
                // Set fields in _model_weightings instance
                _model_weightings.period_spread = weightPS;
            }
            else if (constraintType == "NONMIXEDDURATIONS") { // NONMIXEDDURATIONS soft constraint
                int weightNMD = id;
                // Set fields in _model_weightings instance
                _model_weightings.non_mixed_durations = weightNMD;
            }
        }
    }
    else {
//        cout << "The regexp \"" << re << "\" does not match \"" << s << "\"" << endl;
        return false; // It couldn't read a period constraint.
        // There are two possibilities:
        // (1) the file format is incorrect,
        // (2) Reach the end of file
    }
    return true;
}

