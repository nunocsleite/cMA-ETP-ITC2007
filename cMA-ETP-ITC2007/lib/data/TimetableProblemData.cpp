
#include "data/TimetableProblemData.hpp"


using namespace std;


ostream& operator<<(ostream& _os, const TimetableProblemData& _timetableProblemData) {

    _os <<  "TimetableProblemData Info:" << endl
        << "NumStudents: " << _timetableProblemData.getNumStudents() << endl
        << "NumExams: " << _timetableProblemData.getNumExams() << endl
        << "NumRooms: " << _timetableProblemData.getNumRooms() << endl
        << "ConflictMatrixDensity: " << _timetableProblemData.getConflictMatrixDensity() << endl
        << "NumPeriods: " << _timetableProblemData.getNumPeriods() << endl
        << "NumEnrolments: " << _timetableProblemData.getNumEnrolments() << endl;

    _os << "CourseStudentCounts: " << endl;
    auto const& courseStudentCounts = _timetableProblemData.getCourseClassSize();
    _os << "CourseStudentCounts.size() = " << courseStudentCounts.size() << endl;
//    std::copy(courseStudentCounts.begin(), courseStudentCounts.end(),
//              ostream_iterator<int>(_os, "\n"));

    return _os;
}




