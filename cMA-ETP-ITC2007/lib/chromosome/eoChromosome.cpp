

#include "eoChromosome.h"
#include <iostream>

#include <boost/container/set.hpp>
#include "utils/Common.h"
#include <utils/eoRNG.h>
#include "data/Constraint.hpp"
#include "data/ITC2007Constraints.hpp"


using namespace std;


#define NUM_PROXIMITY_PERIODS 5


//#define EOCHROMOSOME_INCREMENTAL

//#define EOCHROMOSOME_DEBUG

//#define EOCHROMOSOME_DEBUG_1

//#define EOCHROMOSOME_DEBUG_ROOM_CAPACITY


//#define EOCHROMOSOME_DEBUG_FITNESS


//#define EOCHROMOSOME_VALIDATE  // LAST


//#define IMPL1
#define IMPL2


// For debugging purposes
//#define EOCHROMOSOME_DEBUG




////////////////////////////////////////////////////////////////////////////
//
// Soft constraints
//
// See http://www.cs.qub.ac.uk/itc2007/examtrack/exam_track_index_files/examevaluation.htm
// and McCollum's et al. Ann Oper Res 2012 article
//
//
// 1. Two exams in a row
//
// This calculation considers the number of occurrences where two examinations are taken by
// students one straight after another, i.e. back to back, and on the same day. Once this has been
// established, the number of students are summed and multiplied by the number provided in
// the ‘two in a row’ weighting within the ‘Institutional Model Index’. Note that if a student
// has an exam in the last period of one day and another the first period the next day, this does
// not incur a two in a row penalty. (See Sect. 4.9.1.)
//
// 2. Two exams in a day
//
// In the case where there are three periods or more in a day, the number of occurrences of
// students having two exams in a day which are not directly adjacent, i.e. not back to back,
// are calculated. The total number is subsequently multiplied by the ‘two in a day’ weighting
// provided within the ‘Institutional Model Index’. Therefore, two exams in a day are consid-
// ered as those which are not adjacent i.e. they have at least a free period between them. This
// is done to ensure a particular exam placing within a solution does not contribute twice to
// the overall penalty. For example, if two exams were in adjacent periods in the same day,
// the penalty would be counted only as part of the ‘Two exams in a row penalty’. It should
// be noted that where the examination session contains days with 2 periods, this component
// of the penalty, although present for continuity, always is zero and hence superfluous. (See
// Sect. 4.9.2.)
//
// 3. Period spread
//
// This soft constraint enables an organisation to ‘spread’ an individual’s examinations over
// a specified number of periods. This can be thought of as an extension of the two soft con-
// straints previously described. Within the ‘Institutional Model Index’, a figure is provided
// (with the “PERIODSPREAD” keyword) relating to how many periods the solution should
// be ‘spread’ over. The higher this figure, potentially the better the spread of examinations
// for individual students. In the authors’ commercial experience, constructing solutions while
// changing this setting has led to timetables with which the institution is much more satisfied.
// If, for example, PERIODSPREAD within the ‘Institutional Model Index’ is set at 7, for each
// exam we count all the occurrences of enrolled students who have to sit other exams after-
// wards but within 7 periods i.e. the desired period spread. This total is added to the overall
// penalty. It should be noted that the occurrences here will have contributed to the penalty
// calculated for the ‘two exams in a row’ and ‘two exams in a day’ penalties. Although, a
// single occurrence within the solution is effectively penalised twice, it is often necessary due
// to, as indicated above, many institutions requiring certain spreads to be minimised as an
// indication of solution quality. (See Sect. 4.9.3.)
//
// 4. No mixed durations
//
// This applies a penalty to a Room and Period (not Exam) where there are mixed durations.
// The intention here is to try and ensure that exams occur together which are of equal duration.
// In calculating this portion of the penalty, the mixed duration component of the ‘Institutional
// Model Index’ is calculated by the number of violations detected. (See Sect. 4.9.4.)
//
// 5. Front load
//
// It is often desirable that examinations with the largest numbers of students are timetabled
// at the beginning of the examination session. In order to take account of this the “FRONT-
// LOAD” expression is introduced. Within the ‘Institutional Model Index’ the FRONTLOAD
// expression has three parameters e.g. (100, 30, 5). The first of these is the number of largest
// exams that are to be considered. Largest exams are specified by class size. If there are ties
// by size then exams occurring first in the data file are chosen. The second parameter is the
// number of last periods to take into account and which should be ideally avoided by the large
// exams. The third parameter is the penalty or weighting that should be added each time the
// soft constraint is violated. This allows the institution to attempt to ensure that larger exams
// occur earlier in the examination session. This constraint is very popular in practice as exams
// with more students enrolled take longer to mark. (See Sect. 4.9.5.)
//
// 6. Soft room penalty
//
// Organisations often want to keep usage of certain rooms to a minimum. As with the ‘Mixed
// Durations’ component of the overall penalty, this part of the overall penalty should be calcu-
// lated on a period by period basis. For each period, if a room used within the solution has an
// associated penalty, then the penalty for that room for that period is calculated by multiplying
// the associated penalty by the number of exams using that room. (See Sect. 4.9.6.)
//
// 7. Soft period penalty
//
// Organisations often want to keep usage of certain time periods to a minimum. As with
// the ‘Mixed Durations’ and the ‘Room Penalty’ components of the overall penalty, this part
// of the overall penalty should be calculated on a period by period basis. For each period,
// the penalty is calculated by multiplying the associated penalty by the number of exams
// timetabled within that period. (See Sect. 4.9.7.)
// These constraints are relatively complex, and certainly more complicated than those in
// previous models. To reduce potential ambiguities due to the above natural language descrip-
// tion, in the next section, we provide mathematical programming definitions.
//
////////////////////////////////////////////////////////////////////////////


#ifdef IMPL1
/**
 * @brief eoChromosome::computeFitness
 */
void eoChromosome::computeFitness() {
    int two_exams_in_a_row = 0;
    int two_exams_in_a_day = 0;
    int period_spread = 0;
    int mixed_durations = 0;
    int front_load = 0;
    int room_penalty = 0;
    int period_penalty = 0;

    // Timetable container
    TimetableContainer const &timetableCont = getTimetableContainer();
    // Conflict matrix
    IntMatrix const &conflictMatrix = getConflictMatrix();
    // Period vector
    auto const & periodInfoVector = timetableProblemData->getPeriodVector();
    // Institutional model weightings
    InstitutionalModelWeightings const &model_weightings = timetableProblemData->getInstitutionalModelWeightings();
#ifdef EOCHROMOSOME_DEBUG_FITNESS
    cout << "eoChromosome::computeFitness()" << endl;
    cout << "model_weightings.two_in_a_row = " << model_weightings.two_in_a_row << endl;
    cout << "model_weightings.two_in_a_day = " << model_weightings.two_in_a_day << endl;
    cout << "model_weightings.period_spread = " << model_weightings.period_spread << endl;
    cout << "model_weightings.non_mixed_durations = " << model_weightings.non_mixed_durations << endl;
    /**
     * @brief front_load
     * First element - numberLargestExams - number of largest exams. Largest exams are specified by class size
     * Second element - numberLastPeriods - number of last periods to take into account
     * Third element - weightFL -the penalty or weighting
     */
    cout << "model_weightings.front_load: " << endl;
    cout << "numberLargestExams = " << model_weightings.front_load[0] << endl;
    cout << "numberLastPeriods = " << model_weightings.front_load[1] << endl;
    cout << "weightFL = " << model_weightings.front_load[2] << endl;
#endif
    //
    // Two examinations in a row/day
    //
    // For each period (except the last period, which don't have conflicts) do
    for (int period1_id = 0; period1_id < timetableCont.getNumPeriods() - 1; ++period1_id)
    {
        // Get adjacent period
        int period2_id = period1_id + 1;

        if (periodInfoVector[period1_id]->getDate().getDay() != periodInfoVector[period2_id]->getDate().getDay())
            continue;
        // For each exam of period period1_id do
        for (int exam1_id = 0; exam1_id < getNumExams(); ++exam1_id)
        {
            // 1. Two exams in a row
            //
            // This calculation considers the number of occurrences where two examinations are taken by
            // students one straight after another, i.e. back to back, and on the same day. Once this has been
            // established, the number of students are summed and multiplied by the number provided in
            // the ‘two in a row’ weighting within the ‘Institutional Model Index’. Note that if a student
            // has an exam in the last period of one day and another the first period the next day, this does
            // not incur a two in a row penalty. (See Sect. 4.9.1.)
            //
            // If exam1_id is scheduled in period period1_id
            if (timetableCont.isExamScheduled(exam1_id, period1_id))
            {
                for (int exam2_id = 0; exam2_id < getNumExams(); ++exam2_id)
                {
                    // If exam2_id is scheduled in period period2_id
                    if (timetableCont.isExamScheduled(exam2_id, period2_id))
                    {
                        // Get number of conflicts between exams
                        int no_conflicts = conflictMatrix.getVal(exam1_id, exam2_id);
                        if (no_conflicts == 1)
                        {
    //                        if (period_hard_constraints.GetByType(PeriodHardConstraint.types.EXCLUSION).Any(phc => phc.ex1 == exam1_id && phc.ex2 == exam2_id ||
    //                                                                                                               phc.ex1 == exam2_id && phc.ex2 == exam1_id))
    //                        {
    //                            --no_conflicts;
    //                        }
                        }

                        two_exams_in_a_row += no_conflicts * model_weightings.two_in_a_row;

                        // Period Spread (in a row)
                        if (period2_id - period1_id <= model_weightings.period_spread)
                            period_spread += no_conflicts;
                    }
                }



                // 2. Two exams in a day
                //
                // In the case where there are three periods or more in a day, the number of occurrences of
                // students having two exams in a day which are not directly adjacent, i.e. not back to back,
                // are calculated. The total number is subsequently multiplied by the ‘two in a day’ weighting
                // provided within the ‘Institutional Model Index’. Therefore, two exams in a day are consid-
                // ered as those which are not adjacent i.e. they have at least a free period between them. This
                // is done to ensure a particular exam placing within a solution does not contribute twice to
                // the overall penalty. For example, if two exams were in adjacent periods in the same day,
                // the penalty would be counted only as part of the ‘Two exams in a row penalty’. It should
                // be noted that where the examination session contains days with 2 periods, this component
                // of the penalty, although present for continuity, always is zero and hence superfluous. (See
                // Sect. 4.9.2.)
                //
                for (int periodj_id = period2_id + 1; periodj_id < timetableCont.getNumPeriods(); ++periodj_id)
                {
                    if (periodInfoVector[period1_id]->getDate().getDay() != periodInfoVector[periodj_id]->getDate().getDay())
                        break;

                    for (int exam2_id = 0; exam2_id < getNumExams(); ++exam2_id)
                    {
                        // If exam2_id is scheduled in period period2_id
                        if (timetableCont.isExamScheduled(exam2_id, periodj_id))
                        {
                            // Get number of conflicts between exams
                            int no_conflicts = conflictMatrix.getVal(exam1_id, exam2_id);

                            if (no_conflicts == 1)
                            {
        //                        if (period_hard_constraints.GetByType(PeriodHardConstraint.types.EXCLUSION).Any(phc => phc.ex1 == exam1_id && phc.ex2 == exam2_id ||
        //                                                                                                               phc.ex1 == exam2_id && phc.ex2 == exam1_id))
        //                        {
        //                            --no_conflicts;
        //                        }
                            }

                            //two_exams_in_a_day += no_conflicts * model_weightings.Get().two_in_a_day;
                            two_exams_in_a_day += no_conflicts * model_weightings.two_in_a_day;
                            // Period Spread (in the rest of the day)
                            if (periodj_id - period1_id <= model_weightings.period_spread)
                                period_spread += no_conflicts;
                        }
                    }
                }
            }
        }

    }


    //Console.WriteLine("Time1&2: " + watch.ElapsedMilliseconds);
    //watch.Restart();

    // Period Spread (in other days)
    //
    // 3. Period spread
    //
    // This soft constraint enables an organisation to ‘spread’ an individual’s examinations over
    // a specified number of periods. This can be thought of as an extension of the two soft con-
    // straints previously described. Within the ‘Institutional Model Index’, a figure is provided
    // (with the “PERIODSPREAD” keyword) relating to how many periods the solution should
    // be ‘spread’ over. The higher this figure, potentially the better the spread of examinations
    // for individual students. In the authors’ commercial experience, constructing solutions while
    // changing this setting has led to timetables with which the institution is much more satisfied.
    // If, for example, PERIODSPREAD within the ‘Institutional Model Index’ is set at 7, for each
    // exam we count all the occurrences of enrolled students who have to sit other exams after-
    // wards but within 7 periods i.e. the desired period spread. This total is added to the overall
    // penalty. It should be noted that the occurrences here will have contributed to the penalty
    // calculated for the ‘two exams in a row’ and ‘two exams in a day’ penalties. Although, a
    // single occurrence within the solution is effectively penalised twice, it is often necessary due
    // to, as indicated above, many institutions requiring certain spreads to be minimised as an
    // indication of solution quality. (See Sect. 4.9.3.)
    //
    for (int period1_id = 0; period1_id < timetableCont.getNumPeriods() - 1; ++period1_id)
    {
        for (int exam1_id = 0; exam1_id < getNumExams(); ++exam1_id)
        {
            // If exam1_id is scheduled in period period1_id
            if (timetableCont.isExamScheduled(exam1_id, period1_id))
            {

                for (int period2_id = period1_id + 1; period2_id < timetableCont.getNumPeriods(); ++period2_id)
                {
                    if (periodInfoVector[period1_id]->getDate().getDay() == periodInfoVector[period2_id]->getDate().getDay())
                        continue;

                    if (period2_id - period1_id > model_weightings.period_spread)
                        break;

                    for (int exam2_id = 0; exam2_id < getNumExams(); ++exam2_id)
                    {
                        // If exam2_id is scheduled in period period2_id
                        if (timetableCont.isExamScheduled(exam2_id, period2_id))
                        {
                            // Get number of conflicts between exams
                            int no_conflicts = conflictMatrix.getVal(exam1_id, exam2_id);

                            if (no_conflicts == 1)
                            {
        //                        if (period_hard_constraints.GetByType(PeriodHardConstraint.types.EXCLUSION).Any(phc => phc.ex1 == exam1_id && phc.ex2 == exam2_id ||
        //                                                                                                               phc.ex1 == exam2_id && phc.ex2 == exam1_id))
        //                        {
        //                            --no_conflicts;
        //                        }
                            }

                            period_spread += no_conflicts;
                        }
                    }
                }
            }
        }
    }
    //Console.WriteLine("Time3: " + watch.ElapsedMilliseconds);
    //watch.Restart();

    // Mixed Durations
    //
    // 4. No mixed durations
    //
    // This applies a penalty to a Room and Period (not Exam) where there are mixed durations.
    // The intention here is to try and ensure that exams occur together which are of equal duration.
    // In calculating this portion of the penalty, the mixed duration component of the ‘Institutional
    // Model Index’ is calculated by the number of violations detected. (See Sect. 4.9.4.)
    //
    // Get exam vector
    auto const &examVector = getExamVector();
    for (int period_id = 0; period_id < timetableCont.getNumPeriods(); ++period_id)
    {
        for (int room_id = 0; room_id < timetableCont.getNumRooms(); ++room_id)
        {
            vector<int> sizes;

            for(int exam_id = 0; exam_id < getNumExams(); ++exam_id)
            {
                // If exam is scheduled, see its duration
                if (timetableCont.isExamScheduled(exam_id, period_id)
                        && timetableCont.getRoom(exam_id, period_id) == room_id) {
                    int curr_duration = examVector[exam_id]->getDuration();
                    // See if duration is different from the distinct ones contained in vector sizes
                    if (std::find_if(sizes.begin(), sizes.end(),
                             [curr_duration] (int duration) {
                                return duration == curr_duration;
                            }) == sizes.end()) {
                        // If it's different, register it
                        sizes.push_back(curr_duration);
                    }
                }
            }
            if (sizes.size() != 0)
                mixed_durations += (sizes.size() - 1)*model_weightings.non_mixed_durations;
        }
    }

    //Console.WriteLine("Time4: " + watch.ElapsedMilliseconds);
    //watch.Restart();

    // Front Load
    //
    // 5. Front load
    //
    // It is often desirable that examinations with the largest numbers of students are timetabled
    // at the beginning of the examination session. In order to take account of this the “FRONT-
    // LOAD” expression is introduced. Within the ‘Institutional Model Index’ the FRONTLOAD
    // expression has three parameters e.g. (100, 30, 5). The first of these is the number of largest
    // exams that are to be considered. Largest exams are specified by class size. If there are ties
    // by size then exams occurring first in the data file are chosen. The second parameter is the
    // number of last periods to take into account and which should be ideally avoided by the large
    // exams. The third parameter is the penalty or weighting that should be added each time the
    // soft constraint is violated. This allows the institution to attempt to ensure that larger exams
    // occur earlier in the examination session. This constraint is very popular in practice as exams
    // with more students enrolled take longer to mark. (See Sect. 4.9.5.)
    //
//    List<Examination> exams_front_load =
//        examinations.GetAll()
//            .OrderByDescending(ex => ex.students_count)
//            .ToList()
//            .GetRange(0, model_weightings.Get().front_load[0]);
//    List<Period> periods_front_load = periods.GetAll()
//        .OrderByDescending(pe => pe.id)
//        .ToList()
//        .GetRange(0, model_weightings.Get().front_load[1] < periods.EntryCount() ? model_weightings.Get().front_load[1] : periods.EntryCount());

//    foreach (Examination exam in exams_front_load)
//    {
//        if (periods_front_load.Contains(periods.GetById(solution.GetPeriodFrom(exam.id))))
//            front_load += model_weightings.Get().front_load[2];
//    }

//    List<Examination> exams_front_load =
//        examinations.GetAll()
//            .OrderByDescending(ex => ex.students_count)
//            .ToList()
//            .GetRange(0, model_weightings.Get().front_load[0]);
//    List<Period> periods_front_load = periods.GetAll()
//        .OrderByDescending(pe => pe.id)
//        .ToList()
//        .GetRange(0, model_weightings.Get().front_load[1] < periods.EntryCount() ? model_weightings.Get().front_load[1] : periods.EntryCount());

//    foreach (Examination exam in exams_front_load)
//    {
//        if (periods_front_load.Contains(periods.GetById(solution.GetPeriodFrom(exam.id))))
//            front_load += model_weightings.Get().front_load[2];
//    }

    //Console.WriteLine("Time5: " + watch.ElapsedMilliseconds);
    //watch.Restart();

    // Room Penalty & Period Penalty
    //
    // 6. Soft room penalty
    //
    // Organisations often want to keep usage of certain rooms to a minimum. As with the ‘Mixed
    // Durations’ component of the overall penalty, this part of the overall penalty should be calcu-
    // lated on a period by period basis. For each period, if a room used within the solution has an
    // associated penalty, then the penalty for that room for that period is calculated by multiplying
    // the associated penalty by the number of exams using that room. (See Sect. 4.9.6.)
    //
    // 7. Soft period penalty
    //
    // Organisations often want to keep usage of certain time periods to a minimum. As with
    // the ‘Mixed Durations’ and the ‘Room Penalty’ components of the overall penalty, this part
    // of the overall penalty should be calculated on a period by period basis. For each period,
    // the penalty is calculated by multiplying the associated penalty by the number of exams
    // timetabled within that period. (See Sect. 4.9.7.)
    // These constraints are relatively complex, and certainly more complicated than those in
    // previous models. To reduce potential ambiguities due to the above natural language descrip-
    // tion, in the next section, we provide mathematical programming definitions.
    //
    // Get scheduled exams vector
    auto const &scheduledExamsVector = getScheduledExamsVector();
    // Get rooms vector
    auto const &roomVector = getRoomVector();
    for (int exam_id = 0; exam_id < getNumExams(); ++exam_id)
    {
        // Get scheduled exam
        ScheduledExam const &exam = scheduledExamsVector[exam_id];
        // Get exam period
        int tj = exam.getPeriod();
        // Get exam room
        int rk = exam.getRoom();

        room_penalty += roomVector[rk]->getPenalty();
        period_penalty += periodInfoVector[tj]->getPenalty();
    }

    //Console.WriteLine("Time6: " + watch.ElapsedMilliseconds);
    //watch.Restart();

#ifdef EOCHROMOSOME_DEBUG_FITNESS
    cout << "Two in a row: " << two_exams_in_a_row << endl;
    cout << "Two in a day: " << two_exams_in_a_day << endl;
    cout << "Period spread: " << period_spread << endl;
    cout << "Mixed durations: " << mixed_durations << endl;
    cout << "Front load: " << front_load << endl;
    cout << "Room penalty: " << room_penalty << endl;
    cout << "Period penalty: " << period_penalty << endl;
#endif
    solutionFitness = two_exams_in_a_row + two_exams_in_a_day + period_spread + mixed_durations
            + front_load + room_penalty + period_penalty;
}
#endif

#ifdef IMPL2
/**
 * @brief computeCost
 */
void eoChromosome::computeCost() {
    int two_exams_in_a_row = 0;
    int two_exams_in_a_day = 0;
    int period_spread = 0;
    int mixed_durations = 0;
    int front_load = 0;
    int room_penalty = 0;
    int period_penalty = 0;

    // Timetable container
    TimetableContainer const &timetableCont = getTimetableContainer();
    // Conflict matrix
    IntMatrix const &conflictMatrix = getConflictMatrix();
    // Period vector
    auto const & periodInfoVector = timetableProblemData->getPeriodVector();
    // Institutional model weightings
    InstitutionalModelWeightings const &model_weightings = timetableProblemData->getInstitutionalModelWeightings();

//#ifdef EOCHROMOSOME_DEBUG_FITNESS
//    cout << "eoChromosome::computeFitness()" << endl;
//    cout << "model_weightings.two_in_a_row = " << model_weightings.two_in_a_row << endl;
//    cout << "model_weightings.two_in_a_day = " << model_weightings.two_in_a_day << endl;
//    cout << "model_weightings.period_spread = " << model_weightings.period_spread << endl;
//    cout << "model_weightings.non_mixed_durations = " << model_weightings.non_mixed_durations << endl;
//    /**
//     * @brief front_load
//     * First element - numberLargestExams - number of largest exams. Largest exams are specified by class size
//     * Second element - numberLastPeriods - number of last periods to take into account
//     * Third element - weightFL -the penalty or weighting
//     */
//    cout << "model_weightings.front_load: " << endl;
//    cout << "numberLargestExams = " << model_weightings.front_load[0] << endl;
//    cout << "numberLastPeriods = " << model_weightings.front_load[1] << endl;
//    cout << "weightFL = " << model_weightings.front_load[2] << endl;
//#endif
    //
    // Two examinations in a row/day
    //
    // For each period (except the last period, which don't have conflicts) do
    for (int period1_id = 0; period1_id < timetableCont.getNumPeriods() - 1; ++period1_id)
    {
        // Get adjacent period
        int period2_id = period1_id + 1;

        if (periodInfoVector[period1_id]->getDate().getDay() != periodInfoVector[period2_id]->getDate().getDay())
            continue;
        // Get period1_id exams
        auto const &period1Exams = timetableCont.getPeriodExams(period1_id);
        // Get period2_id exams
        auto const &period2Exams = timetableCont.getPeriodExams(period2_id);

        // For each exam of period period1_id do
        for (auto const& exam1Room1Tuple : period1Exams)
        {
            // Get exam1_id
            int exam1_id = std::get<0>(exam1Room1Tuple);
            //
            // 1. Two exams in a row
            //
            // This calculation considers the number of occurrences where two examinations are taken by
            // students one straight after another, i.e. back to back, and on the same day. Once this has been
            // established, the number of students are summed and multiplied by the number provided in
            // the ‘two in a row’ weighting within the ‘Institutional Model Index’. Note that if a student
            // has an exam in the last period of one day and another the first period the next day, this does
            // not incur a two in a row penalty. (See Sect. 4.9.1.)
            //
            // For each exam of period period2_id do
            for (auto const& exam2Room2Tuple : period2Exams) {
                // Get exam2_id
                int exam2_id = std::get<0>(exam2Room2Tuple);
                // Get number of conflicts between exams
                int no_conflicts = conflictMatrix.getVal(exam1_id, exam2_id);
                if (no_conflicts == 1)
                {
//                        if (period_hard_constraints.GetByType(PeriodHardConstraint.types.EXCLUSION).Any(phc => phc.ex1 == exam1_id && phc.ex2 == exam2_id ||
//                                                                                                               phc.ex1 == exam2_id && phc.ex2 == exam1_id))
//                        {
//                            --no_conflicts;
//                        }
                }

                two_exams_in_a_row += no_conflicts * model_weightings.two_in_a_row;

                // Period Spread (in a row)
                if (period2_id - period1_id <= model_weightings.period_spread)
                    period_spread += no_conflicts;
            }

            for (int periodj_id = period2_id + 1; periodj_id < timetableCont.getNumPeriods(); ++periodj_id)
            {
                if (periodInfoVector[period1_id]->getDate().getDay() != periodInfoVector[periodj_id]->getDate().getDay())
                    break;

                // Get periodj_id exams
                auto const &periodjExams = timetableCont.getPeriodExams(periodj_id);
                // For each exam of period periodj_id do
                for (auto const &examjRoomjTuple : periodjExams)
                {
                    // Get examj_id
                    int examj_id = std::get<0>(examjRoomjTuple);
                    //
                    // 2. Two exams in a day
                    //
                    // In the case where there are three periods or more in a day, the number of occurrences of
                    // students having two exams in a day which are not directly adjacent, i.e. not back to back,
                    // are calculated. The total number is subsequently multiplied by the ‘two in a day’ weighting
                    // provided within the ‘Institutional Model Index’. Therefore, two exams in a day are consid-
                    // ered as those which are not adjacent i.e. they have at least a free period between them. This
                    // is done to ensure a particular exam placing within a solution does not contribute twice to
                    // the overall penalty. For example, if two exams were in adjacent periods in the same day,
                    // the penalty would be counted only as part of the ‘Two exams in a row penalty’. It should
                    // be noted that where the examination session contains days with 2 periods, this component
                    // of the penalty, although present for continuity, always is zero and hence superfluous. (See
                    // Sect. 4.9.2.)
                    //
                    // Get number of conflicts between exams
                    int no_conflicts = conflictMatrix.getVal(exam1_id, examj_id);

                    if (no_conflicts == 1)
                    {
    //                        if (period_hard_constraints.GetByType(PeriodHardConstraint.types.EXCLUSION).Any(phc => phc.ex1 == exam1_id && phc.ex2 == exam2_id ||
    //                                                                                                               phc.ex1 == exam2_id && phc.ex2 == exam1_id))
    //                        {
    //                            --no_conflicts;
    //                        }
                    }

                    //two_exams_in_a_day += no_conflicts * model_weightings.Get().two_in_a_day;
                    two_exams_in_a_day += no_conflicts * model_weightings.two_in_a_day;
                    // Period Spread (in the rest of the day)
                    if (periodj_id - period1_id <= model_weightings.period_spread)
                        period_spread += no_conflicts;
                }
            }
        }
    }

    // Period Spread (in other days)
    //
    // 3. Period spread
    //
    // This soft constraint enables an organisation to ‘spread’ an individual’s examinations over
    // a specified number of periods. This can be thought of as an extension of the two soft con-
    // straints previously described. Within the ‘Institutional Model Index’, a figure is provided
    // (with the “PERIODSPREAD” keyword) relating to how many periods the solution should
    // be ‘spread’ over. The higher this figure, potentially the better the spread of examinations
    // for individual students. In the authors’ commercial experience, constructing solutions while
    // changing this setting has led to timetables with which the institution is much more satisfied.
    // If, for example, PERIODSPREAD within the ‘Institutional Model Index’ is set at 7, for each
    // exam we count all the occurrences of enrolled students who have to sit other exams after-
    // wards but within 7 periods i.e. the desired period spread. This total is added to the overall
    // penalty. It should be noted that the occurrences here will have contributed to the penalty
    // calculated for the ‘two exams in a row’ and ‘two exams in a day’ penalties. Although, a
    // single occurrence within the solution is effectively penalised twice, it is often necessary due
    // to, as indicated above, many institutions requiring certain spreads to be minimised as an
    // indication of solution quality. (See Sect. 4.9.3.)
    //
    for (int period1_id = 0; period1_id < timetableCont.getNumPeriods() - 1; ++period1_id)
    {
        // Get period1_id exams
        auto const &period1Exams = timetableCont.getPeriodExams(period1_id);

        // For each exam of period period1_id do
        for (auto const& exam1Room1Tuple : period1Exams)
        {
            // Get exam1_id
            int exam1_id = std::get<0>(exam1Room1Tuple);
            for (int period2_id = period1_id + 1; period2_id < timetableCont.getNumPeriods(); ++period2_id)
            {
                if (periodInfoVector[period1_id]->getDate().getDay() == periodInfoVector[period2_id]->getDate().getDay())
                    continue;

                if (period2_id - period1_id > model_weightings.period_spread)
                    break;

                // Get period2_id exams
                auto const &period2Exams = timetableCont.getPeriodExams(period2_id);
                // For each exam of period period2_id do
                for (auto const& exam2Room2Tuple : period2Exams) {
                    // Get exam2_id
                    int exam2_id = std::get<0>(exam2Room2Tuple);
                    // Get number of conflicts between exams
                    int no_conflicts = conflictMatrix.getVal(exam1_id, exam2_id);

                    if (no_conflicts == 1)
                    {
//                        if (period_hard_constraints.GetByType(PeriodHardConstraint.types.EXCLUSION).Any(phc => phc.ex1 == exam1_id && phc.ex2 == exam2_id ||
//                                                                                                               phc.ex1 == exam2_id && phc.ex2 == exam1_id))
//                        {
//                            --no_conflicts;
//                        }
                    }

                    period_spread += no_conflicts;
                }
            }
        }
    }

    // Mixed Durations
    //
    // 4. No mixed durations
    //
    // This applies a penalty to a Room and Period (not Exam) where there are mixed durations.
    // The intention here is to try and ensure that exams occur together which are of equal duration.
    // In calculating this portion of the penalty, the mixed duration component of the ‘Institutional
    // Model Index’ is calculated by the number of violations detected. (See Sect. 4.9.4.)
    //
    // Get exam vector
    auto const &examVector = getExamVector();
    for (int period_id = 0; period_id < timetableCont.getNumPeriods(); ++period_id)
    {
        // Get period_id exams
        auto const &periodExams = timetableCont.getPeriodExams(period_id);

        for (int room_id = 0; room_id < timetableCont.getNumRooms(); ++room_id)
        {
            vector<int> sizes;

            for(auto const &examRoomTuple : periodExams)
            {
                // Get exam_id
                int exam_id = std::get<0>(examRoomTuple);
                // See the scheduled exam duration
                if (timetableCont.getRoom(exam_id, period_id) == room_id) {
                    int curr_duration = examVector[exam_id]->getDuration();
                    // See if duration is different from the distinct ones contained in vector sizes
                    if (std::find_if(sizes.begin(), sizes.end(),
                             [curr_duration] (int duration) {
                                return duration == curr_duration;
                            }) == sizes.end()) {
                        // If it's different, register it
                        sizes.push_back(curr_duration);
                    }
                }
            }
            if (sizes.size() != 0)
                mixed_durations += (sizes.size() - 1)*model_weightings.non_mixed_durations;
        }
    }


    // Front Load
    //
    // 5. Front load
    //
    // It is often desirable that examinations with the largest numbers of students are timetabled
    // at the beginning of the examination session. In order to take account of this the “FRONT-
    // LOAD” expression is introduced. Within the ‘Institutional Model Index’ the FRONTLOAD
    // expression has three parameters e.g. (100, 30, 5). The first of these is the number of largest
    // exams that are to be considered. Largest exams are specified by class size. If there are ties
    // by size then exams occurring first in the data file are chosen. The second parameter is the
    // number of last periods to take into account and which should be ideally avoided by the large
    // exams. The third parameter is the penalty or weighting that should be added each time the
    // soft constraint is violated. This allows the institution to attempt to ensure that larger exams
    // occur earlier in the examination session. This constraint is very popular in practice as exams
    // with more students enrolled take longer to mark. (See Sect. 4.9.5.)
    //
    // Get scheduled exams vector
    auto const &scheduledExamsVector = getScheduledExamsVector();
    // Obtain course student counts sorted in decreasing order by student counts
    auto const &sortedExamsCountsPairs = getSortedCourseClassSize();
    // Get the number of largest exams that are to be considered.
    int numberOfLargestExams = model_weightings.front_load[0];
    // Get the number of last periods to take into account and which
    // should be ideally avoided by the large exams.
    int numberOfLastPeriodsToAvoid = model_weightings.front_load[1];
    // Get the penalty or weighting that should be added each time the
    // soft constraint is violated.
    int penalty = model_weightings.front_load[2];
    // Count violations of this soft constraint
    for (int i = 0; i < numberOfLargestExams; ++i) {
        // Get exam id
        int exam_id = sortedExamsCountsPairs[i].first;
        // Get ScheduledExam object
        ScheduledExam const &scheduledExam = scheduledExamsVector[exam_id];
        // If exam is scheduled in the last periods, add a violation
        if (scheduledExam.getPeriod() >= getNumPeriods() - numberOfLastPeriodsToAvoid) {
            front_load += penalty;
        }
    }

    // Room Penalty & Period Penalty
    //
    // 6. Soft room penalty
    //
    // Organisations often want to keep usage of certain rooms to a minimum. As with the ‘Mixed
    // Durations’ component of the overall penalty, this part of the overall penalty should be calcu-
    // lated on a period by period basis. For each period, if a room used within the solution has an
    // associated penalty, then the penalty for that room for that period is calculated by multiplying
    // the associated penalty by the number of exams using that room. (See Sect. 4.9.6.)
    //
    // 7. Soft period penalty
    //
    // Organisations often want to keep usage of certain time periods to a minimum. As with
    // the ‘Mixed Durations’ and the ‘Room Penalty’ components of the overall penalty, this part
    // of the overall penalty should be calculated on a period by period basis. For each period,
    // the penalty is calculated by multiplying the associated penalty by the number of exams
    // timetabled within that period. (See Sect. 4.9.7.)
    // These constraints are relatively complex, and certainly more complicated than those in
    // previous models. To reduce potential ambiguities due to the above natural language descrip-
    // tion, in the next section, we provide mathematical programming definitions.
    //
    // Get rooms vector
    auto const &roomVector = getRoomVector();

    for (int exam_id = 0; exam_id < getNumExams(); ++exam_id)
    {
        // Get scheduled exam
        ScheduledExam const &exam = scheduledExamsVector[exam_id];
        // Get exam period
        int tj = exam.getPeriod();
        // Get exam room
        int rk = exam.getRoom();

        room_penalty += roomVector[rk]->getPenalty();
        period_penalty += periodInfoVector[tj]->getPenalty();
    }

#ifdef EOCHROMOSOME_DEBUG_FITNESS
    cout << endl << "eoChromosome::computeCost()" << endl;
    cout << "Two in a row: " << two_exams_in_a_row << endl;
    cout << "Two in a day: " << two_exams_in_a_day << endl;
    cout << "Period spread: " << period_spread << endl;
    cout << "Mixed durations: " << mixed_durations << endl;
    cout << "Front load: " << front_load << endl;
    cout << "Room penalty: " << room_penalty << endl;
    cout << "Period penalty: " << period_penalty << endl;
#endif
    solutionCost = two_exams_in_a_row + two_exams_in_a_day + period_spread + mixed_durations
            + front_load + room_penalty + period_penalty;
}

#endif







/**
 * @brief eoChromosome::init
 * @param _timetableProblemData
 */
void eoChromosome::init(TimetableProblemData const* _timetableProblemData) {

    /// Implementation #1 - TimetableContainerMatrix
    ///
    // Instantiate timetable matrix with dimensions (# exams x # periods)
    timetableContainer = boost::make_shared<TimetableContainerMatrix>(
                _timetableProblemData->getNumExams(), _timetableProblemData->getNumPeriods(),
                _timetableProblemData->getNumRooms(), _timetableProblemData);

    // Set timetable problem data
    setTimetableProblemData(_timetableProblemData);

    // Chromosome is not feasible initially
    feasible = false;
}


//// DEBUG
///
#ifdef EOCHROMOSOME_INCREMENTAL
//    std::cout << "this == kempeChain.getSolution() ? " << (this == &_kempeChain.getSolution() ? "true" : "false") << std::endl;
//    std::cout << "Ti = " << _kempeChain.getTi() << std::endl;
//    std::cout << "Tj = " << _kempeChain.getTj() << std::endl;

//    /// Source period exams //////////////////////////
//    std::cout << "source Ti period size = " << timetableCont.getPeriodSize(_kempeChain.getTi()) << std::endl;
//    std::cout << "source Ti period exams: ";
//    // For each exam of period do
//    for (int ei = 0; ei < getNumExams(); ++ei) {
//        if (srcTiExams[ei] == SCHED_EXAM)
//            std::cout << ei << " ";
//    }
//    std::cout << std::endl;
//    std::cout << "Actual timetable Ti period exams: ";
//    // For each exam of period do
//    for (int ei = 0; ei < getNumExams(); ++ei) {
//        if (timetableCont.isExamScheduled(ei, _kempeChain.getTi()))
//            std::cout << ei << " ";
//    }
//    std::cout << std::endl;

//    std::cout << "source Tj period size = " << timetableCont.getPeriodSize(_kempeChain.getTj()) << std::endl;
//    std::cout << "source Tj period exams: ";
//    // For each exam of period do
//    for (int ei = 0; ei < getNumExams(); ++ei) {
//        if (srcTjExams[ei] == SCHED_EXAM)
//            std::cout << ei << " ";
//    }
//    std::cout << std::endl;
//    std::cout << "Actual timetable Tj period exams: ";
//    // For each exam of period do
//    for (int ei = 0; ei < getNumExams(); ++ei) {
//        if (timetableCont.isExamScheduled(ei, _kempeChain.getTj()))
//            std::cout << ei << " ";
//    }
//    std::cout << std::endl;

//    /// Dest period exams //////////////////////////
//    std::cout << "dest Ti period exams: ";
//    // For each exam of period do
//    for (int ei = 0; ei < getNumExams(); ++ei) {
//        if (destTiExams[ei] == SCHED_EXAM)
//            std::cout << ei << " ";
//    }
//    std::cout << std::endl;

//    std::cout << "dest Tj period exams: ";
//    // For each exam of period do
//    for (int ei = 0; ei < getNumExams(); ++ei) {
//        if (destTjExams[ei] == SCHED_EXAM)
//            std::cout << ei << " ";
//    }
//    std::cout << std::endl;

    std::cout << "///////////////////////////////// Incremental eval method /////////////////////////////////////////" << std::endl;
    std::cout << "Original solution # proximity conflicts = " << getProximityConflicts() << std::endl;

    std::cout << "Computing (again) the solution full eval..." << std::endl;
    computeExamProximityConflicts();
    std::cout << "Original solution # proximity conflicts after full eval = " << getProximityConflicts() << std::endl;

#endif

//////







/**
 * @brief computeSolutionCostIncremental Compute chromosome's proximity cost
 * performing an incremental evaluation based on the Kempe chain information.
 *
 * Pre-condition: The solution passed in the <this> parameter is the one prior to move
 *
 * @param _kempeChain
 */
void eoChromosome::computeSolutionCostIncremental(const ETTPKempeChain<eoChromosome> &_kempeChain) {
    // Source Ti and Tj exams
    std::vector<typename ETTPKempeChain<eoChromosome>::ExamRoomTuple> const &srcTiExams = _kempeChain.getOriginalTiPeriodExams();
    std::vector<typename ETTPKempeChain<eoChromosome>::ExamRoomTuple> const &srcTjExams = _kempeChain.getOriginalTjPeriodExams();
    // Dest Ti and Tj exams
    std::vector<typename ETTPKempeChain<eoChromosome>::ExamRoomTuple> const &destTiExams = _kempeChain.getFinalTiPeriodExams();
    std::vector<typename ETTPKempeChain<eoChromosome>::ExamRoomTuple> const &destTjExams = _kempeChain.getFinalTjPeriodExams();
    // The timetable container
    TimetableContainer &timetableCont = getTimetableContainer();

//    int two_exams_in_a_row = 0;
//    int two_exams_in_a_day = 0;
    int two_exams_in_a_day_and_row = 0;
    int period_spread = 0;
    int mixed_durations = 0;
    int front_load = 0;
    int room_penalty = 0;
    int period_penalty = 0;

#ifdef EOCHROMOSOME_VALIDATE

    long originalSolutionCost = getSolutionCost();

///
/// TO REMOVE
///
///
///
///
///

    // Compute solution cost
    computeCost();
    long originalComputedSolutionCost = getSolutionCost();

#endif


    ////////////////////// Move Kempe chain exams between Ti and Tj ///////////////////////////////////////////////
    ///
    long examProximityConflictsOfRemovedExams = 0;
    long examProximityConflictsOfInsertedExams = 0;

    // Get exams that belong to the Kempe chain in Ti, that is,
    // those which will be moved to time slot Tj
    std::vector<std::tuple<int, int, int>> examsToBeMovedFromTi;
    int tSource = _kempeChain.getTi();
    int tDest = _kempeChain.getTj();
    int roomSource = _kempeChain.getRi();
    int roomDest = _kempeChain.getRj();

    getExamsToBeMoved(srcTiExams, destTjExams, tSource, tDest, roomSource, examsToBeMovedFromTi); // Last is an out parameter
    // Get exams that belong to the Kempe chain in Tj, that is,
    // those which will be moved to time slot Ti
    std::vector<std::tuple<int, int, int>> examsToBeMovedFromTj;
    getExamsToBeMoved(srcTjExams, destTiExams, tDest, tSource, roomDest, examsToBeMovedFromTj); // Last is an out parameter

    // Institutional model weightings
    InstitutionalModelWeightings const &model_weightings = timetableProblemData->getInstitutionalModelWeightings();
    // Period vector
    auto const &periodInfoVector = timetableProblemData->getPeriodVector();
    // Get room vector
    auto const &roomVector = this->getRoomVector();
    //
    // Front load penalty computation
    //
    // Obtain course student counts sorted in decreasing order by student counts
    auto const &sortedExamsCountsPairs = getSortedCourseClassSize();
    // Get the number of largest exams that are to be considered.
    int numberOfLargestExams = model_weightings.front_load[0];
    // Get the number of last periods to take into account and which
    // should be ideally avoided by the large exams.
    int numberOfLastPeriodsToAvoid = model_weightings.front_load[1];
    // Get the penalty or weighting that should be added each time the
    // soft constraint is violated.
    int frontLoadPenalty = model_weightings.front_load[2];

    //
    // Move all *source* kempe chain exams and determine corresponding removal and insertion exam proximity costs
    //
    // O(number exams)
    for (auto const &examSrcDestRoomTuple : examsToBeMovedFromTi) {
        // Get exam id
        int exam_id = std::get<0>(examSrcDestRoomTuple);
        // Get source room
        int sourceRoom = std::get<1>(examSrcDestRoomTuple);
        // Get destination room
        int destRoom = std::get<2>(examSrcDestRoomTuple);
        // 1. and 2. Two exams in a row/day
//        examProximityConflictsOfRemovedExams += getConflictInADayAndRowFromDay(exam_id, _kempeChain.getTi());
        two_exams_in_a_day_and_row -= getConflictInADayAndRowFromDay(exam_id, _kempeChain.getTi());
        // 3. Period spread. Determine the proximity conflicts of exam_id in the source period
//        examProximityConflictsOfRemovedExams += getConflictPeriodSpreadBeforeAndAfterPeriod(exam_id, _kempeChain.getTi());
        period_spread -= getConflictPeriodSpreadBeforeAndAfterPeriod(exam_id, _kempeChain.getTi());
//        // 4. No mixed durations
//        examProximityConflictsOfRemovedExams += getConflictMixedDurationsFromPeriodAndRoom(_kempeChain.getTi(), sourceRoom);
        // 4. No mixed durations
//        examProximityConflictsOfRemovedExams += getConflictMixedDurationsFromPeriodAndRoom(_kempeChain.getTi(), sourceRoom);
        mixed_durations -= getConflictMixedDurationsFromPeriodAndRoom(_kempeChain.getTi(), sourceRoom);
        mixed_durations -= getConflictMixedDurationsFromPeriodAndRoom(_kempeChain.getTj(), destRoom);

#ifdef EOCHROMOSOME_DEBUG_FITNESS
        if (timetableCont.getRoom(exam_id, _kempeChain.getTi()) != sourceRoom) {
            stringstream sstream;
            sstream << "In method [eoChromosome::computeSolutionCostIncremental]: source room = " << sourceRoom
                    << " and getRoom(exam_id, Ti) = " << timetableCont.getRoom(exam_id, _kempeChain.getTi())
                    << " should be the same";
            string msg = sstream.str();
            throw runtime_error(msg);
        }
#endif
        // Unschedule the exam from the timetable
        timetableCont.unscheduleExam(exam_id, _kempeChain.getTi());
        //
        // Now, insert the exam in the Tj period
        //
        timetableCont.scheduleExam(exam_id, _kempeChain.getTj(), destRoom);

        // 1. and 2. Two exams in a row/day
//        examProximityConflictsOfInsertedExams += getConflictInADayAndRowFromDay(exam_id, _kempeChain.getTj());
        two_exams_in_a_day_and_row += getConflictInADayAndRowFromDay(exam_id, _kempeChain.getTj());
        // 3. Period spread. Determine the proximity conflicts of exam_id in the dest period
//        examProximityConflictsOfInsertedExams += getConflictPeriodSpreadBeforeAndAfterPeriod(exam_id, _kempeChain.getTj());
        period_spread += getConflictPeriodSpreadBeforeAndAfterPeriod(exam_id, _kempeChain.getTj());
        // 4. No mixed durations
//        examProximityConflictsOfInsertedExams += getConflictMixedDurationsFromPeriodAndRoom(_kempeChain.getTj(), destRoom);
        mixed_durations += getConflictMixedDurationsFromPeriodAndRoom(_kempeChain.getTi(), sourceRoom);
        mixed_durations += getConflictMixedDurationsFromPeriodAndRoom(_kempeChain.getTj(), destRoom);
///
/// COULD BE OPTIMIZED?
///
        // 5. Front load. Count violations of this soft constraint
        for (int i = 0; i < numberOfLargestExams; ++i) {
            // Get current large exam id
            int large_exam_id = sortedExamsCountsPairs[i].first;
            // If exam_id is a large exam, see if it's scheduled in the last periods. If it is, add a penalty.
            if (large_exam_id == exam_id) {
                // If exam is scheduled in the last periods, update violations
//                if (_kempeChain.getTi() >= getNumPeriods() - numberOfLastPeriodsToAvoid
//                        && !(_kempeChain.getTj() >= getNumPeriods() - numberOfLastPeriodsToAvoid)) {
//                    this->solutionCost -= frontLoadPenalty;
//                }
//                if (!(_kempeChain.getTi() >= getNumPeriods() - numberOfLastPeriodsToAvoid )
//                        && _kempeChain.getTj() >= getNumPeriods() - numberOfLastPeriodsToAvoid) {
//                    this->solutionCost += frontLoadPenalty;
//                }
                if (_kempeChain.getTi() >= getNumPeriods() - numberOfLastPeriodsToAvoid) {
                    front_load -= frontLoadPenalty;
                }
                if (_kempeChain.getTj() >= getNumPeriods() - numberOfLastPeriodsToAvoid) {
                    front_load += frontLoadPenalty;
                }
            }
        }

        // 6. Adjust room penalty
        room_penalty += roomVector[destRoom]->getPenalty() - roomVector[sourceRoom]->getPenalty();
        // 7. Adjust Period penalty
        period_penalty += periodInfoVector[_kempeChain.getTj()]->getPenalty() - periodInfoVector[_kempeChain.getTi()]->getPenalty();
    }

    //
    // Move all *dest* kempe chain exams and determine corresponding removal and insertion exam proximity costs
    //
    // O(number exams)
    for (auto const &examSrcDestRoomTuple : examsToBeMovedFromTj) {
        // Get exam id
        int exam_id = std::get<0>(examSrcDestRoomTuple);
        // Get source room
        int sourceRoom = std::get<1>(examSrcDestRoomTuple);
        // Get destination room
        int destRoom = std::get<2>(examSrcDestRoomTuple);
        // 1. and 2. Two exams in a row/day
//        examProximityConflictsOfRemovedExams += getConflictInADayAndRowFromDay(exam_id, _kempeChain.getTj());
        two_exams_in_a_day_and_row -= getConflictInADayAndRowFromDay(exam_id, _kempeChain.getTj());
        // 3. Period spread. Determine the proximity conflicts of exam_id in the source period
//        examProximityConflictsOfRemovedExams += getConflictPeriodSpreadBeforeAndAfterPeriod(exam_id, _kempeChain.getTj());
        period_spread -= getConflictPeriodSpreadBeforeAndAfterPeriod(exam_id, _kempeChain.getTj());
//        // 4. No Mixed durations
//        examProximityConflictsOfRemovedExams += getConflictMixedDurationsFromPeriodAndRoom(_kempeChain.getTj(), sourceRoom);
        // 4. No Mixed durations
//        examProximityConflictsOfRemovedExams += getConflictMixedDurationsFromPeriodAndRoom(_kempeChain.getTj(), sourceRoom);
        mixed_durations -= getConflictMixedDurationsFromPeriodAndRoom(_kempeChain.getTj(), sourceRoom);
        mixed_durations -= getConflictMixedDurationsFromPeriodAndRoom(_kempeChain.getTi(), destRoom);
#ifdef EOCHROMOSOME_DEBUG_FITNESS
        if (timetableCont.getRoom(exam_id, _kempeChain.getTj()) != sourceRoom) {
            stringstream sstream;
            sstream << "In method [eoChromosome::computeSolutionCostIncremental]: source room = " << sourceRoom
                    << " and getRoom(exam_id, Tj) = " << timetableCont.getRoom(exam_id, _kempeChain.getTj())
                    << " should be the same";
            string msg = sstream.str();
            throw runtime_error(msg);
        }
#endif

        // Unschedule the exam from the timetable
        timetableCont.unscheduleExam(exam_id, _kempeChain.getTj());

        //
        // Now, insert the exam in the Tj period
        //
        timetableCont.scheduleExam(exam_id, _kempeChain.getTi(), destRoom);
        // 1. and 2. Two exams in a row/day
//        examProximityConflictsOfInsertedExams += getConflictInADayAndRowFromDay(exam_id, _kempeChain.getTi());
        two_exams_in_a_day_and_row += getConflictInADayAndRowFromDay(exam_id, _kempeChain.getTi());
        // 3. Period spread. Determine the proximity conflicts of exam_id in the dest period
//        examProximityConflictsOfInsertedExams += getConflictPeriodSpreadBeforeAndAfterPeriod(exam_id, _kempeChain.getTi());
        period_spread += getConflictPeriodSpreadBeforeAndAfterPeriod(exam_id, _kempeChain.getTi());
        // 4. No Mixed durations
//        examProximityConflictsOfInsertedExams += getConflictMixedDurationsFromPeriodAndRoom(_kempeChain.getTi(), destRoom);
        mixed_durations += getConflictMixedDurationsFromPeriodAndRoom(_kempeChain.getTj(), sourceRoom);
        mixed_durations += getConflictMixedDurationsFromPeriodAndRoom(_kempeChain.getTi(), destRoom);

///
/// COULD BE OPTIMIZED
///
        // Count violations of this soft constraint
        for (int i = 0; i < numberOfLargestExams; ++i) {
            // Get current large exam id
            int large_exam_id = sortedExamsCountsPairs[i].first;
            // If exam_id is a large exam, see if it's scheduled in the last periods. If it is, add a penalty.
            if (large_exam_id == exam_id) {
                // If exam is scheduled in the last periods, update violations
//                if (_kempeChain.getTj() >= getNumPeriods() - numberOfLastPeriodsToAvoid
//                        && !(_kempeChain.getTi() >= getNumPeriods() - numberOfLastPeriodsToAvoid)) {
//                    this->solutionCost -= frontLoadPenalty;
//                }
//                if (!(_kempeChain.getTj() >= getNumPeriods() - numberOfLastPeriodsToAvoid )
//                        && _kempeChain.getTi() >= getNumPeriods() - numberOfLastPeriodsToAvoid) {
//                    this->solutionCost += frontLoadPenalty;
//                }
                if (_kempeChain.getTj() >= getNumPeriods() - numberOfLastPeriodsToAvoid) {
                    front_load -= frontLoadPenalty;
                }
                if (_kempeChain.getTi() >= getNumPeriods() - numberOfLastPeriodsToAvoid) {
                    front_load += frontLoadPenalty;
                }
            }
        }
        // 6. Adjust room penalty
        room_penalty += roomVector[destRoom]->getPenalty() - roomVector[sourceRoom]->getPenalty();
        // 7. Adjust Period penalty
        period_penalty += periodInfoVector[_kempeChain.getTi()]->getPenalty() - periodInfoVector[_kempeChain.getTj()]->getPenalty();

    }

//    // Now subtract to the solution proximity conflicts the conflicts of the removed exams
//    this->solutionCost -= examProximityConflictsOfRemovedExams;
//    // ... and add the conflicts of the inserted exams
//    this->solutionCost += examProximityConflictsOfInsertedExams;
    // Update room and period penalty
    this->solutionCost += two_exams_in_a_day_and_row + period_spread + mixed_durations  + front_load + room_penalty + period_penalty;


#ifdef EOCHROMOSOME_VALIDATE
    std::cout << "///////////////////////////////// Incremental eval method /////////////////////////////////////////" << std::endl;
    std::cout << "Original solution cost = " << originalSolutionCost << std::endl;
    std::cout << "Original computed solution cost = " << originalComputedSolutionCost << std::endl;

    long incrementalCost = solutionCost;
    std::cout << "Incremental cost = " << incrementalCost << std::endl;

//    cout << "Two in a row: " << two_exams_in_a_row << endl;
//    cout << "Two in a day: " << two_exams_in_a_day << endl;
    cout << "Two in a day/row: " << two_exams_in_a_day_and_row << endl;
    cout << "Period spread: " << period_spread << endl;
    cout << "Mixed durations: " << mixed_durations << endl;
    cout << "Front load: " << front_load << endl;
    cout << "Room penalty: " << room_penalty << endl;
    cout << "Period penalty: " << period_penalty << endl;

    int newSolutionCost = originalSolutionCost + two_exams_in_a_day_and_row + period_spread + mixed_durations
            + front_load + room_penalty + period_penalty;
    std::cout << "Original solution cost + incremental update = " << newSolutionCost << std::endl;

    if (newSolutionCost != solutionCost) {
        stringstream sstream;
        sstream << "In method [eoChromosome::computeSolutionCostIncremental]:" << endl
                << "newSolutionCost = " << newSolutionCost << " and solutionCost = " << solutionCost << " should be the same";
        string msg = sstream.str();
        throw runtime_error(msg);
    }

    if (originalSolutionCost != originalComputedSolutionCost) {
        stringstream sstream;
        sstream << "In method [eoChromosome::computeSolutionCostIncremental]:" << endl
                << "originalSolutionCost = " << originalSolutionCost
                << " and originalComputedSolutionCost = " << originalComputedSolutionCost << " should be the same";
        string msg = sstream.str();
        throw runtime_error(msg);
    }

    // Compute non-incremental cost
//    computeCost();

//    std::cout << "Solution cost after full eval = " << getSolutionCost() << std::endl;

//    cin.get();

//    validate();
#endif

}





/**
 * @brief eoChromosome::getExamsToBeMoved
 * @param _srcPeriodExams
 * @param _destPeriodExams
 * @param _tSource
 * @param _tDest
 * @param _sourceRoom
 * @param _examsToBeMovedFromSourcePeriod
 */
void eoChromosome::getExamsToBeMoved(std::vector<typename ETTPKempeChain<eoChromosome>::ExamRoomTuple> const &_srcPeriodExams,
                                     std::vector<typename ETTPKempeChain<eoChromosome>::ExamRoomTuple> const &_destPeriodExams,
                                     int _tSource, int _tDest, int _sourceRoom,
                                     std::vector<std::tuple<int, int, int>> &_examsToBeMovedFromSourcePeriod) {

    // Vector of exams to be moved. The first value is true if the exam is to be moved (initially all false).
    // The second value is the source room.
    std::vector<std::pair<bool, int>> examsToBeMoved(getNumExams());
    // Mark exams in s
    // O(number exams)
    for (auto const &examRoomTuple : _srcPeriodExams) {
        // Get exam id
        int exam_id = std::get<0>(examRoomTuple);
        // Get room id
        int room_id = std::get<1>(examRoomTuple);
        examsToBeMoved[exam_id].first = true;
        examsToBeMoved[exam_id].second = room_id;
    }

    // O(number exams)
    for (auto const &examRoomTuple : _destPeriodExams) {
        // Get exam id
        int exam_id = std::get<0>(examRoomTuple);

        // If exam_id belongs to the Kempe chain in source period and in the dest period,
        // then it is to be moved to dest time slot
        if (examsToBeMoved[exam_id].first == true) {
            // Get source room
            int sourceRoom = examsToBeMoved[exam_id].second;
            if (_tSource != _tDest || _tSource == _tDest  && _sourceRoom == sourceRoom) {
                // Get dest room
                int destRoom = std::get<1>(examRoomTuple);
                // Associate the dest room to the tuple to return
                _examsToBeMovedFromSourcePeriod.push_back(std::make_tuple(exam_id, sourceRoom, destRoom));
            }
        }
    }
}




/**
 * @brief eoChromosome::getConflictInADayAndRowFromDay
 * @param _examination
 * @param _period
 * @return
 */
int eoChromosome::getConflictInADayAndRowFromDay(int _examination, int _period) {
    // Timetable container
    TimetableContainer const &timetableCont = getTimetableContainer();
    // Conflict matrix
    IntMatrix const &conflict_matrix = getConflictMatrix();
    // Period vector
    auto const &periodInfoVector = timetableProblemData->getPeriodVector();
    // Institutional model weightings
    InstitutionalModelWeightings const &model_weightings = timetableProblemData->getInstitutionalModelWeightings();

    int conflict = 0;

    for (int period_id = 0; period_id < getNumPeriods(); ++period_id)
    {
        if (periodInfoVector[period_id]->getDate().getMonth() != periodInfoVector[_period]->getDate().getMonth())
            continue;

        if (periodInfoVector[period_id]->getDate().getDay() < periodInfoVector[_period]->getDate().getDay())
            continue;

        if (periodInfoVector[period_id]->getDate().getDay() > periodInfoVector[_period]->getDate().getDay())
            break;

        if (period_id == _period)
            continue;

        // Get period examinations
        auto const &exams = timetableCont.getPeriodExams(period_id);
        for (auto const &examRoomTuple : exams)
        {
            int exam = std::get<0>(examRoomTuple);
            int no_conflicts = conflict_matrix.getVal(exam, _examination);

            if (no_conflicts == 0)
                continue;

            if (no_conflicts == 1)
            {
            //               if (period_hard_constraints.GetByType(PeriodHardConstraint.types.EXCLUSION).Any(phc => phc.ex1 == exam && phc.ex2 == _examination ||
            //                                                                                                      phc.ex1 == _examination && phc.ex2 == exam))
            //               {
            //                   --no_conflicts;
            //               }

            }
            if (period_id == _period - 1 || period_id == _period + 1)
            {
                conflict += no_conflicts * model_weightings.two_in_a_row;
            }
            else
            {
                conflict += no_conflicts * model_weightings.two_in_a_day;
            }

        }
    }

    return conflict;
}





/**
 * @brief eoChromosome::getConflictPeriodSpreadBeforeAndAfterPeriod
 * @param _examination
 * @param _period
 * @return
 */
int eoChromosome::getConflictPeriodSpreadBeforeAndAfterPeriod(int _examination, int _period) {
    // Timetable container
    TimetableContainer const &timetableCont = getTimetableContainer();
    // Conflict matrix
    IntMatrix const &conflict_matrix = getConflictMatrix();
    // Period vector
//    auto const &periodInfoVector = timetableProblemData->getPeriodVector();
    // Institutional model weightings
    InstitutionalModelWeightings const &model_weightings = timetableProblemData->getInstitutionalModelWeightings();

    int fitness = 0;

    for (int period_id = 0; period_id < getNumPeriods(); ++period_id) {

        if (period_id >= _period - model_weightings.period_spread &&
            period_id <= _period + model_weightings.period_spread &&
            period_id != _period)
        {
            // Get period examinations
            auto const &exams = timetableCont.getPeriodExams(period_id);
            for (auto const &examRoomTuple : exams)
            {
                // Get exam
                int exam = std::get<0>(examRoomTuple);
                // Get number of conflicts
                int no_conflicts = conflict_matrix.getVal(exam, _examination);
                if (no_conflicts == 1)
                {
//                    if (period_hard_constraints.GetByType(PeriodHardConstraint.types.EXCLUSION)
//                            .Any(phc => phc.ex1 == exam && phc.ex2 == _examination ||
//                                        phc.ex1 == _examination && phc.ex2 == exam))
//                    {
//                        --no_conflicts;
//                    }
                }
                fitness += no_conflicts;
            }
        }
    }

    return fitness;
}




/**
 * @brief eoChromosome::getConflictMixedDurationsFromPeriodAndRoom
 * @param _period
 * @param _room
 * @return
 */
int eoChromosome::getConflictMixedDurationsFromPeriodAndRoom(int _period, int _room) {
    // Institutional model weightings
    InstitutionalModelWeightings const &model_weightings = timetableProblemData->getInstitutionalModelWeightings();
    std::vector<int> sizes;
    // Get examinations from period and room
    // Get exam vector
    auto &examVector = getExamVector();
    // Fitness
    int fitness = 0;

    // Timetable container
    TimetableContainer const &timetableCont = getTimetableContainer();

    // Get period_id exams
    auto const &periodExams = timetableCont.getPeriodExams(_period);


    for(auto const &examRoomTuple : periodExams)
    {
        // Get exam_id
        int exam_id = std::get<0>(examRoomTuple);
        // See the scheduled exam duration
        if (timetableCont.getRoom(exam_id, _period) == _room) {
            int curr_duration = examVector[exam_id]->getDuration();
            // See if duration is different from the distinct ones contained in vector sizes
            if (std::find_if(sizes.begin(), sizes.end(),
                     [curr_duration] (int duration) {
                        return duration == curr_duration;
                    }) == sizes.end()) {
                // If it's different, register it
                sizes.push_back(curr_duration);
            }
        }
    }
    if (sizes.size() != 0)
        fitness += (sizes.size() - 1)*model_weightings.non_mixed_durations;

    return fitness;
}






/**
 * @brief eoChromosome::getExamProximityConflicts Determine the proximity conflicts of exam _ei in period _period
 * This is done by suming up all the edge costs of conflicting exams five time slots from left and
 * five time slots to right.
 * @param _ei
 * @param _period
 * @return
 */
long eoChromosome::getExamProximityConflicts(int _ei, int _pi) {
    long sourceExamProximityConflicts = 0;
    // Sum up all the edge costs of conflicting exams five time slots from left and
    // five time slots to right

    int two_exams_in_a_row = 0;
    int two_exams_in_a_day = 0;
    int period_spread = 0;
    int mixed_durations = 0;
    int front_load = 0;
    int room_penalty = 0;
    int period_penalty = 0;

    // Get exam graph reference
    AdjacencyList const &examGraph = this->getExamGraph();
    // Timetable container
    TimetableContainer const &timetableCont = getTimetableContainer();
    // Conflict matrix
    IntMatrix const &conflictMatrix = getConflictMatrix();
    // Period vector
    auto const &periodInfoVector = timetableProblemData->getPeriodVector();
    // Institutional model weightings
    InstitutionalModelWeightings const &model_weightings = timetableProblemData->getInstitutionalModelWeightings();

    int pj;

/// TODO: USE GRAPH OR VECTOR INDEXING??
///
///

    // Get ei adjacent vertices
    boost::property_map<AdjacencyList, boost::vertex_index_t>::type index_map = get(boost::vertex_index, examGraph);
    boost::graph_traits<AdjacencyList>::adjacency_iterator ai, a_end;
    boost::tie(ai, a_end) = adjacent_vertices(_ei, examGraph);
    for (; ai != a_end; ++ai) {
        // Get adjacent exam
        int ej = get(index_map, *ai);
        //
        // Period spread
        //
        // If exam ej is scheduled five periods apart (left or right),
        // sum the corresponding weighted proximity cost
        for (int i = 0; i <= model_weightings.period_spread-1; ++i) {
            // Check to the right
            pj = _pi+i+1;
            if (pj < timetableCont.getNumPeriods()) {
                // If exam ej is scheduled in period pj
                if (timetableCont.isExamScheduled(ej, pj)) {
                    // Get number of conflicts between exams
                    int no_conflicts = conflictMatrix.getVal(_ei, ej);
                    sourceExamProximityConflicts += no_conflicts;
                    break; // Go to the next adjacent exam
                }
            }
            // If the exam ej was not in the right, check to the left
            pj = _pi-i-1;
            if (pj >= 0) {
                // If exam ej is scheduled in period pj
                if (timetableCont.isExamScheduled(ej, pj)) {
                    int no_conflicts = conflictMatrix.getVal(_ei, ej);
                    sourceExamProximityConflicts += no_conflicts;
                    break; // Go to the next adjacent exam
                }
            }
        }
    }
    return sourceExamProximityConflicts;
}






// ADDED METHODS



void eoChromosome::validate() const {

    cout << endl << "[validate() method]" << endl;

   // Count # exams, # enrolments, and verify exams unicity
    int numExams = 0, numEnrolments = 0;
    set<int> examsSet;
    bool unique = true;
    // Timetable container
    TimetableContainer const &timetableCont = this->getTimetableContainer();
    // Conflict matrix
    IntMatrix const &conflictMatrix = this->getConflictMatrix();

    int numExamsInPeriods = 0;

    // For each period do
    for (int pi = 0; pi < timetableCont.getNumPeriods(); ++pi) {
        // Count # exams in period pi
        int numExamsPeriodPi = timetableCont.getPeriodSize(pi);
        int numExamsPeriodPi2 = 0;
        for (int exam = 0; exam < getNumExams(); ++exam) {
            if (timetableCont.isExamScheduled(exam, pi))
                ++numExamsPeriodPi2;
        }
        if (numExamsPeriodPi != numExamsPeriodPi2) {
            cout << "numExamsPeriodPi != numExamsPeriodPi2 " << endl
                 << "numExamsPeriodPi = " << numExamsPeriodPi << ", numExamsPeriodPi2 = "
                 << numExamsPeriodPi2 << endl;
            cout << "pi = " << pi << ", # exams in period = " << numExamsPeriodPi << endl;
            ////// Print period contents /////////////////////
            cout << "pi = " << pi << " period contents: ";
            // For each exam of period pi do
            for (int ei = 0; ei < getNumExams(); ++ei) {
                // If exam ei is scheduled in period pi
                if (timetableCont.isExamScheduled(ei, pi)) {
                    cout << ei << " ";
                }
            }
            cout << endl;
            cin.get();

        }

        numExamsInPeriods += numExamsPeriodPi;
    }
    // Reset examsSet
    examsSet.clear();

    // For each period do
    for (int pi = 0; pi < timetableCont.getNumPeriods(); ++pi) {
        // For each exam of period pi do
        for (int ei = 0; ei < getNumExams(); ++ei) {
            // If exam ei is scheduled in period pi
            if (timetableCont.isExamScheduled(ei, pi)) {
                ++numExams;
                numEnrolments += this->getCourseClassSize()[ei];
                // Verify unicity of exams
                if (examsSet.find(ei) != examsSet.end()) {
                    unique = false;
                    cout << "There are duplicated exams" << endl;
                    // Print period contents
                    cout << "Period pi=" << pi << " contents: ";
                    for (auto const &examRoomTuple: timetableCont.getPeriodExams(pi)) {
                        int exam = std::get<0>(examRoomTuple);
                        cout << exam << " ";
                    }
                    cout << endl;

                    cin.get();

                    return;
                }
                // Insert exam in examsSet
                examsSet.insert(ei);
            }
        }
    }

//    cout << "numExams = " << numExams << endl;
//    cout << "numExamsInPeriods = " << numExamsInPeriods << endl;
//    cout << "numEnrolments = " << numEnrolments << endl << endl;
//    cout << "Original values" << endl;
//    cout << "numExams = " << this->getNumExams() << endl;
//    cout << "numEnrolments = " << this->getNumEnrolments() << endl;

//    if (numExams != this->getNumExams()) {
//        cout << "numExams is different" << endl;
//        cout << "counted: " << numExams << "; original: " <<  this->getNumExams() << endl;
//        cin.get();
//    }
//    else if (numExams != numExamsInPeriods) {
//        cout << "numExams != numExamsInPeriods" << endl;
//        cout << "numExams: " << numExams << "; numExamsInPeriods: " << numExamsInPeriods << endl;
//        cin.get();
//    }
//    else if (numEnrolments != this->getNumEnrolments()) {
//        cout << "numEnrolments is different" << endl;
//        cin.get();
//    }

    //
    // Verify feasibility
    //
    // For each period do
    for (int pi = 0; pi < timetableCont.getNumPeriods(); ++pi) {
        //
        // Verify feasibility within each period
        //
        // For each exam of period pi do
        for (int ei = 0; ei < getNumExams(); ++ei) {
            // If exam ei is scheduled in period pi
            if (timetableCont.isExamScheduled(ei, pi)) {
                // Verify feasibility between exam ei and the others exams of period pi
                for (int ej = ei+1; ej < getNumExams(); ++ej) {
                    // If exam ej is scheduled in period pi
                    if (timetableCont.isExamScheduled(ej, pi)) {
                        // Obtain conflicts by consulting the conflicting matrix
                        int n = conflictMatrix.getVal(ei, ej);
                        if (n > 0) {
                            cout << endl << "Found period with non feasible exams" << endl;
                            cout << "ei = " << ei << " and ej = " << ej << " have " << n << " students in common" << endl;
                            cin.get();
//                            cout << "Exiting..." << endl;
//                            exit(-1);
                        }
                    }
                }
            }
        }
    }
    //
    // Evaluate Hard Constraints
    //
    double distanceToFeasibily = 0;
    // For each period do
    for (int period_id = 0; period_id < timetableCont.getNumPeriods(); ++period_id)
    {
        // For every rooms of the current period, verify if no more seats are used than are available
        // for that room.
        if (!verifyRoomsCapacityConstraint(period_id)){
            cout << "Infeasible solution: Room capacity constraint were not verified" << endl;
            cin.get();
        }

        // Get period1_id exams
        auto const &periodExams = timetableCont.getPeriodExams(period_id);
        // For each exam of period period_id do
        for (auto const& examRoomTuple : periodExams)
        {
            // Get exam_id
            int exam_id = std::get<0>(examRoomTuple);
            // Verify Period-Related constraint for exam ei. Pre-condition: ei is scheduled
            if (!verifyPeriodRelatedConstraintsScheduled(exam_id, period_id)) {
                cout << "Infeasible solution: Period-Related constraint were not verified" << endl;
                cin.get();
            }
            // Verify Period-Utilisation constraint for exam ei.
            if (!verifyPeriodUtilisationConstraint(exam_id, period_id)) {
                cout << "Infeasible solution: Period-Utilisation constraint were not verified" << endl;
                cin.get();
            }
        }

        verifyRoomsCapacityConstraint(period_id);
    }


    //
    // Verify scheduled exam vector integrity
    //
    // Get scheduled exams vector
    auto const &scheduledExamsVector = getScheduledExamsVector();
    // For each period do
    for (int pi = 0; pi < timetableCont.getNumPeriods(); ++pi) {
        // For each exam of period pi do
        for (int ei = 0; ei < getNumExams(); ++ei) {
            // If exam ei is scheduled in period pi
            if (timetableCont.isExamScheduled(ei, pi)) {
                // Get scheduled exam object
                ScheduledExam const &exam = scheduledExamsVector[ei];
                if (!exam.isScheduled())
                    throw runtime_error("[Verify scheduled exam vector integrity] exam should be scheduled");
                else if (exam.getPeriod() != pi || exam.getRoom() != timetableCont.getRoom(ei, pi))
                    throw runtime_error("[Verify scheduled exam vector integrity] exam with different period or room");
            }
        }
    }
}




/**
 * @brief getFeasibleRoom
 * @param _ei Exam to schedule
 * @param _tj Period where exam _ei is to be scheduled
 * @param _rk Feasible room. _rk is set by reference.
 */
//bool eoChromosome::getFeasiblePeriodRoom(int _ei, int _tj, int &_rk) const
bool eoChromosome::getFeasiblePeriodRoom(int _ei, int _tj, int &_rk) {
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
    // No-Conflicts: Conflicting exams cannot be assigned to the same period. (As usual,
    //   two exams are said to conflict whenever they have some student taking them both.)
    // Room-Occupancy: For every room and period no more seats are used than are available
    //   for that room.
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
    //===

    //-
    // Verify Period-Utilisation constraint
    //-
    if (!verifyPeriodUtilisationConstraint(_ei, _tj))
        return false; // Period-Utilisation constraint was violated

    //-
    // Verify Period-Related constraint
    //-
    if (!verifyPeriodRelatedConstraints(_ei, _tj))
        return false; // Period-Related constraint was violated

    //-
    // Verify Room-Occupancy constraint and Room-Related constraint
    // and get a random feasible room
    //-
    if (!getFeasibleRoom(_ei, _tj, _rk))
        return false; // Period-Related constraint was violated


    return true;
}





/**
 * @brief verifyAfterConstraint
 * @param _ei
 * @param _tj
 * @return
 */
bool eoChromosome::verifyAfterConstraint(int _ei, int _tj) {
    // For any pair (e1, e2) of exams, the After constraint is defined by:
    //   - ‘AFTER’: e1 must take place strictly after e2
    // Get exam vector
    auto const &examVector = this->getExamVector();
    // Get scheduled exams vector
    auto const &scheduledExamsVector = this->getScheduledExamsVector();
    // Get TimetableContainer object
    TimetableContainer &timetableCont = this->getTimetableContainer();
    //---
    // We have to schedule temporarily exam 'ei' in order to verify the constraints
    int dummyRoom = 0;
    timetableCont.scheduleExam(_ei, _tj, dummyRoom);
    //---

    // Get exam 'ei' Period-Related Hard constraints
    auto const& periodRelatedHardConstraintsVector = examVector[_ei]->getPeriodRelatedHardConstraints();

    bool verifyConstraint = true;
    // Verify only if the After constraints are satisfied for exam 'ei' scheduled at period 'tj'
    for (auto const& ptrConstraint : periodRelatedHardConstraintsVector) {
        boost::shared_ptr<AfterConstraint> ptrAfterConstraint =
                boost::dynamic_pointer_cast<AfterConstraint, Constraint>(ptrConstraint);
        // Verify if exams are scheduled. Only if both exams are scheduled the constraint is verified.
        if (ptrAfterConstraint != nullptr && scheduledExamsVector[ptrAfterConstraint->getE1()].isScheduled() &&
            scheduledExamsVector[ptrAfterConstraint->getE2()].isScheduled()) {

            double distanceToFeasibility;
            // If the constraint is not satisfied return false
            if ( (distanceToFeasibility = ptrConstraint->operator()(*this)) != 0) {
                verifyConstraint = false;
                break;
            }
        }
    }
    //---
    // Unschedule exam
    timetableCont.unscheduleExam(_ei, _tj);
    //---
    return verifyConstraint;
}







/**
 * @brief verifyPeriodUtilisationConstraint Verify Period-Utilisation constraint
 * @param _ei
 * @param _tj
 * @return
 */
bool eoChromosome::verifyPeriodUtilisationConstraint(int _ei, int _tj) const {
    // Period-Utilisation: No exam assigned to a period uses more time than available for
    //   that period.

#ifdef EOCHROMOSOME_DEBUG
    cout << endl << "[verifyPeriodUtilisationConstraint]" << endl;
    cout << "_ei = " << _ei << ", _tj = " << _tj << endl;
#endif

    // Get exam vector
    auto const &examVector = this->getExamVector();
    // Get period vector
    auto const &periodVector = this->getPeriodVector();
    // Get period duration
    int periodDuration = periodVector[_tj]->getDuration();
    // Get exam duration
    int examDuration = examVector[_ei]->getDuration();
    // Verify period utilisation constraint
    if (examDuration > periodDuration) {
#ifdef EOCHROMOSOME_DEBUG
        cout << " > constraint [verifyPeriodUtilisationConstraint] FAILED" << endl;
#endif
        return false;
    }
    else {
#ifdef EOCHROMOSOME_DEBUG
        cout << " > constraint [verifyPeriodUtilisationConstraint] OK" << endl;
#endif
        return true;
    }
}





/**
 * @brief verifyPeriodUtilisationAndAfterConstraints
 * @param _ei
 * @param _tj
 * @return
 */
bool eoChromosome::verifyPeriodUtilisationAndAfterConstraints(int _ei, int _tj) {
    // First, verify the Period-Utilisation constraint
    if (!verifyPeriodUtilisationConstraint(_ei, _tj))
        return false;
    // Then, verify the After constraint
    if (!verifyAfterConstraint(_ei, _tj))
        return false;
    // If the constraints are verified, return true;
    return true;
}




/**
 * @brief verifyPeriodRelatedConstraintsScheduled Verify Period-Related constraint. Pre-condition: ei *is* scheduled
 * @param _ei
 * @param _tj
 * @return
 */
bool eoChromosome::verifyPeriodRelatedConstraintsScheduled(int _ei, int _tj) const {
    // Period-Related: All of a set of time-ordering requirements between pairs of exams
    //   are obeyed. Specifically, for any pair (e1, e2) of exams, one can specify any of:
    //   - ‘AFTER’: e1 must take place strictly after e2,
    //   - ‘EXAM_COINCIDENCE’: e1 must take place at the same time as e2,
    //   - ‘EXCLUSION’: e1 must not take place at the same time as e2.

#ifdef EOCHROMOSOME_DEBUG
    cout << endl << "[verifyPeriodRelatedConstraints]" << endl;
    cout << "_ei = " << _ei << ", _tj = " << _tj << endl;
#endif

    // Get exam vector
    auto const &examVector = this->getExamVector();
    // Get scheduled exams vector
    auto const &scheduledExamsVector = this->getScheduledExamsVector();
    // Get TimetableContainer object
    TimetableContainer const &timetableCont = this->getTimetableContainer();
//    //---
//    // We have to schedule temporarily exam 'ei' in order to verify the constraints
//    int dummyRoom = 0;
//    timetableCont.scheduleExam(_ei, _tj, dummyRoom);
//    //---

    // Get exam 'ei' Period-Related Hard constraints
    auto const& periodRelatedHardConstraintsVector = examVector[_ei]->getPeriodRelatedHardConstraints();

    bool verifyConstraint = true;

    // Verify if constraints are satisfied for exam 'ei' scheduled at period 'tj'
    for (auto const& ptrConstraint : periodRelatedHardConstraintsVector) {
        boost::shared_ptr<BinaryConstraint> ptrBinConstraint =
                boost::dynamic_pointer_cast<BinaryConstraint, Constraint>(ptrConstraint);
        // Verify if exams are scheduled. Only if both exams are scheduled the constraint is verified.
        if (scheduledExamsVector[ptrBinConstraint->getE1()].isScheduled() &&
            scheduledExamsVector[ptrBinConstraint->getE2()].isScheduled()) {

            double distanceToFeasibility;
            // If the constraint is not satisfied return false
            if ( (distanceToFeasibility = ptrConstraint->operator()(*this)) != 0) {
                verifyConstraint = false;
                break;
            }
        }
    }
//    //---
//    // Unschedule exam
//    timetableCont.unscheduleExam(_ei, _tj);
//    //---

#ifdef EOCHROMOSOME_DEBUG
    if (verifyConstraint) {
        cout << " > constraint [verifyPeriodRelatedConstraints] OK" << endl;
    }
    else
        cout << " > constraint [verifyPeriodRelatedConstraints] FAILED" << endl;
#endif
    return verifyConstraint;
}








/**
 * @brief verifyPeriodRelatedConstraints Verify Period-Related constraint. Pre-condition: ei is not scheduled
 * @param _ei
 * @param _tj
 * @return
 */
bool eoChromosome::verifyPeriodRelatedConstraints(int _ei, int _tj) {
    // Period-Related: All of a set of time-ordering requirements between pairs of exams
    //   are obeyed. Specifically, for any pair (e1, e2) of exams, one can specify any of:
    //   - ‘AFTER’: e1 must take place strictly after e2,
    //   - ‘EXAM_COINCIDENCE’: e1 must take place at the same time as e2,
    //   - ‘EXCLUSION’: e1 must not take place at the same time as e2.

#ifdef EOCHROMOSOME_DEBUG
    cout << endl << "[verifyPeriodRelatedConstraints]" << endl;
    cout << "_ei = " << _ei << ", _tj = " << _tj << endl;
#endif

    // Get exam vector
    auto const &examVector = this->getExamVector();
    // Get scheduled exams vector
    auto const &scheduledExamsVector = this->getScheduledExamsVector();
    // Get TimetableContainer object
    TimetableContainer &timetableCont = this->getTimetableContainer();
    //---
    // We have to schedule temporarily exam 'ei' in order to verify the constraints
    int dummyRoom = 0;
    timetableCont.scheduleExam(_ei, _tj, dummyRoom);
    //---

    // Get exam 'ei' Period-Related Hard constraints
    auto const& periodRelatedHardConstraintsVector = examVector[_ei]->getPeriodRelatedHardConstraints();

    bool verifyConstraint = true;

    // Verify if constraints are satisfied for exam 'ei' scheduled at period 'tj'
    for (auto const& ptrConstraint : periodRelatedHardConstraintsVector) {
        boost::shared_ptr<BinaryConstraint> ptrBinConstraint =
                boost::dynamic_pointer_cast<BinaryConstraint, Constraint>(ptrConstraint);
        // Verify if exams are scheduled. Only if both exams are scheduled the constraint is verified.
        if (scheduledExamsVector[ptrBinConstraint->getE1()].isScheduled() &&
            scheduledExamsVector[ptrBinConstraint->getE2()].isScheduled()) {

            double distanceToFeasibility;
            // If the constraint is not satisfied return false
            if ( (distanceToFeasibility = ptrConstraint->operator()(*this)) != 0) {
                verifyConstraint = false;
                break;
            }
        }
    }
    //---
    // Unschedule exam
    timetableCont.unscheduleExam(_ei, _tj);
    //---

#ifdef EOCHROMOSOME_DEBUG
    if (verifyConstraint) {
        cout << " > constraint [verifyPeriodRelatedConstraints] OK" << endl;
    }
    else
        cout << " > constraint [verifyPeriodRelatedConstraints] FAILED" << endl;
#endif
    return verifyConstraint;
}




/**
 * @brief verifyRoomCapacityConstraint Verify Room capacity constraint
 * @param _ei
 * @param _rk
 * @return
 */
bool eoChromosome::verifyRoomCapacityConstraint(int _ei, int _rk) const {
    // Get room vector
    auto const& roomVector = this->getRoomVector();
    // Get seat capacity for room _rk
    int roomSeatCapacity = roomVector[_rk]->getCapacity();
    // Get number of students for exam _ei
    auto const& examVector = this->getExamVector();
    int thisExamNumStudents = examVector[_ei]->getNumStudents();
    // Verify room capacity constraint
    if (thisExamNumStudents > roomSeatCapacity) {
        return false;
    }
    else {
        return true;
    }
}





bool eoChromosome::verifyRoomsCapacityConstraint(int _ti) const {
    // Room-Occupancy: For every room and period no more seats are used than are available
    //   for that room.

//    bool roomCapacitiesVerified = true;

#ifdef EOCHROMOSOME_DEBUG_ROOM_CAPACITY
    cout << endl << "[verifyRoomsCapacityConstraint]" << endl;
    cout << "_ti = " << _ti << endl;
#endif

    // Get scheduled rooms vector
    auto const& scheduledRoomVector = this->getScheduledRoomsVector();
    // Get room vector
    auto const& roomVector = this->getRoomVector();
    /////////////////////////////////////////////////////////////
    // Timetable container
    TimetableContainer const &timetableCont = getTimetableContainer();
    // Get _ti exams
    auto const &periodExams = timetableCont.getPeriodExams(_ti);
    // Get exam vector
//    auto const &examVector = getExamVector();
    /////////////////////////////////////////////////////////////

    for (int rk = 0; rk < getNumRooms(); ++rk) {
        // Get number of # occupied seats for room _rk
        int numOccupiedSeats = scheduledRoomVector[rk].getNumOccupiedSeats(_ti);

//#ifdef EOCHROMOSOME_DEBUG_ROOM_CAPACITY
//        /////////////////////////////////////////////////////////////
//        // Compute number of # occupied seats for room _rk
//        int computedNumOccupiedSeats = 0;
//        // Exam # students
//        int examStudents;
//        // For each exam of period _ti do
//        for (auto const& examRoomTuple : periodExams)
//        {
//            // Get exam_id
//            int exam_id = std::get<0>(examRoomTuple);
//            // Get room_id
//            int room_id = std::get<1>(examRoomTuple);
//            // If room_id equals rk, sum exam students
//            if (room_id == rk) {
//                examStudents = examVector[exam_id]->getNumStudents();
//                computedNumOccupiedSeats += examStudents;
//                // Print info
//                cout << "exam_id = " << exam_id << ", examStudents = " << examStudents
//                     << ", computedNumOccupiedSeats = " << computedNumOccupiedSeats << endl;
//            }
//        }
//        /////////////////////////////////////////////////////////////
//#endif

        // Get seat capacity for room _rk
        int roomSeatCapacity = roomVector[rk]->getCapacity();


        // Verify room occupancy constraint
        if (/*numOccupiedSeats != computedNumOccupiedSeats || */ numOccupiedSeats > roomSeatCapacity) {
            cout << "Room capacity error" << endl;
#ifdef EOCHROMOSOME_DEBUG_ROOM_CAPACITY
            cout << endl << "[verifyRoomsCapacityConstraint]" << endl;
            cout << "_ti = " << _ti << endl;
            cout << "Room " << rk << endl;
            cout << "numOccupiedSeats = " << numOccupiedSeats << ", roomSeatCapacity = " << roomSeatCapacity << endl;
//        cout << "Computed numOccupiedSeats = " << computedNumOccupiedSeats << ", roomSeatCapacity = " << roomSeatCapacity << endl;
#endif
            cin.get();
            return false;
        }
    }

    return true;
}






/**
 * @brief verifyRoomOccupancyConstraint Verify Room-Occupancy constraint.  Pre-condition: ei is not scheduled
 * @param _ei
 * @param _tj
 * @param _rk
 * @return
 */
bool eoChromosome::verifyRoomOccupancyConstraint(int _ei, int _tj, int _rk) const {
    // Room-Occupancy: For every room and period no more seats are used than are available
    //   for that room.

#ifdef EOCHROMOSOME_DEBUG_1
    cout << endl << "[verifyRoomOccupancyConstraint]" << endl;
    cout << "_ei = " << _ei << ", _tj = " << _tj << ", _rk = " << _rk << endl;

    verifyRoomsCapacityConstraint(_tj);

#endif

    // Get scheduled rooms vector
    auto const& scheduledRoomVector = this->getScheduledRoomsVector();
    // Get room vector
    auto const& roomVector = this->getRoomVector();
    // Get number of currently occupied seats for room _rk
    int numOccupiedSeats = scheduledRoomVector[_rk].getNumOccupiedSeats(_tj);
    // Get seat capacity for room _rk
    int roomSeatCapacity = roomVector[_rk]->getCapacity();
    // Get number of students for exam _ei
    auto const& examVector = this->getExamVector();
    int thisExamNumStudents = examVector[_ei]->getNumStudents();
#ifdef EOCHROMOSOME_DEBUG_1
    cout << "numOccupiedSeats = " << numOccupiedSeats << ", thisExamNumStudents = " << thisExamNumStudents
         << ", roomSeatCapacity = " << roomSeatCapacity << endl;
#endif

    // Verify room occupancy constraint
    if (numOccupiedSeats + thisExamNumStudents > roomSeatCapacity) {
#ifdef EOCHROMOSOME_DEBUG_1
        cout << " > constraint [verifyRoomOccupancyConstraint] FAILED" << endl;
#endif
        return false;
    }
    else {
#ifdef EOCHROMOSOME_DEBUG_1
        cout << " > constraint [verifyRoomOccupancyConstraint] OK" << endl;
#endif
        return true;
    }
}





/**
* @brief getFeasibleRoom Verify Room-Occupancy and Room-Related constraints and return feasible random room
*                        Pre-condition: ei is not scheduled.
* @param _ei
* @param _tj
* @param _rk
* @return
*/
bool eoChromosome::getFeasibleRoom(int _ei, int _tj, int &_rk) {
    //
    // A set containing the feasible rooms is formed, and a random room is selected.
    //
    // Create an empty set of feasible rooms
    vector<int> feasibleRooms;
    // Get TimetableContainer object
//    TimetableContainer &timetableCont = this->getTimetableContainer();
    // Verify room feasibility
    for (int room = 0; room < this->getNumRooms(); ++room) {
#ifdef EOCHROMOSOME_DEBUG_ROOM
        cout << "Room = " << room << endl;
#endif

///
/// NOT EFFICIENT BECAUSE WE'RE SCHEDULING AND UNSCHEDULING EXAMS...
///
///
///
        // Verify Room-Occupancy constraint and Room-Related constraint
        if (verifyRoomOccupancyConstraint(_ei, _tj, room)) {
            if (verifyRoomRelatedConstraints(_ei, _tj, room)) {
                feasibleRooms.push_back(room); // A feasible room-period pair was found
            }
        }
    }
#ifdef EOCHROMOSOME_DEBUG_ROOM
    cout << "All rooms were searched" << endl;
#endif

    if (feasibleRooms.size() == 0) {
#ifdef EOCHROMOSOME_DEBUG_ROOM
        cout << "No feasible room was found" << endl;
#endif
        // Set rk to infeasible value
        _rk = -1;
        return false; // No feasible room was found
    }
    //
    // Otherwise, a feasible room exists
    // Generate random room index
    int idx = rng.uniform(feasibleRooms.size());
    _rk = feasibleRooms[idx];

#ifdef EOCHROMOSOME_DEBUG_ROOM
    cout << "feasibleRooms.size() = " << feasibleRooms.size() << endl;
    copy(feasibleRooms.begin(), feasibleRooms.end(), ostream_iterator<int>(cout, " "));
    //cout << "idx = " << idx << ", _rk = " << _rk << endl;
    cout << "Room _rk = " << _rk << " was selected" << endl;
    cin.get();
#endif
    // A feasible room was found
    return true;
}






/**
 * @brief verifyRoomRelatedConstraints Verify Room-Related constraint. Pre-condition: ei is not scheduled
 * @param _ei
 * @param _tj
 * @param _rk
 * @return
 */
bool eoChromosome::verifyRoomRelatedConstraints(int _ei, int _tj, int _rk) {
    //==
    // Room-Related: Room requirements are obeyed. Specifically, for any exam one can
    //   disallow the usual sharing of rooms and specify
    //   - ‘EXCLUSIVE’: Exam e must take place in a room on its own.
    //   E.g.: 2, ROOM_EXCLUSIVE      Exam '2' must be timetabled in a room by itself.
    //==
    // Get scheduled exams vector
    auto const &scheduledExamsVector = getScheduledExamsVector();
    // Get exam vector
    auto const &examVector = getExamVector();
    // Get exam
    Exam &exam_info = *examVector[_ei].get();
    // Get TimetableContainer object
    TimetableContainer &timetableCont = getTimetableContainer();
    //
    // There are two possible scenarios:
    // 1. 'ei' has ROOM_EXCLUSIVE constraint itself
    //     - If there are any exams already in the room then they are conflicting
    //
    // 2. The room where 'ei' is going to be inserted has one other exam
    //    which has ROOM_EXCLUSIVE constraint
    //     - that exam is conflicting with 'ei'
    //

    //
    // Scenario 1. 'ei' has ROOM_EXCLUSIVE constraint itself
    //     - If there are any exams already in the room then they are conflicting
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
        distanceToFeasibility = ptrConstraint->operator()(*this);

        //---
        // Unschedule exam
        timetableCont.unscheduleExam(_ei, _tj);
        //---

        if (distanceToFeasibility > 0)
            return false;
    }


    //
    // Scenario 2. The room where 'ei; is going to be inserted has one other exam
    //    which has ROOM_EXCLUSIVE constraint
    //     - that exam is conflicting with 'ei'
    //
    // Determine exams allocated to room rk in period tj
    vector<pair<int,int> > roomExams = getRoomExamsSorted(_tj, _rk);
    // Get scheduled rooms vector
    auto const &scheduledRoomsVector = getScheduledRoomsVector();
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
            // If the constraint is not satisfied return false
            distanceToFeasibility = ptrConstraint->operator()(*this);

            //---
            // Unschedule exam
            timetableCont.unscheduleExam(_ei, _tj);
            //---

            if (distanceToFeasibility > 0)
                return false;
        }
    }
    return true;
}





/**
 * @brief verifyRoomCapacity Verify room capacity for exam _ei
 * @param _ei
 * @param _rk
 * @return
 */
bool eoChromosome::verifyRoomCapacity(int _ei, int _rk) const
{
    // Get room vector
    auto const& roomVector = this->getRoomVector();
    // Get seat capacity for room _rk
    int roomSeatCapacity = roomVector[_rk]->getCapacity();
    // Get number of students for exam _ei
    auto const& examVector = this->getExamVector();
    int thisExamNumStudents = examVector[_ei]->getNumStudents();
#ifdef EOCHROMOSOME_DEBUG
    cout << "thisExamNumStudents = " << thisExamNumStudents
         << ", roomSeatCapacity = " << roomSeatCapacity << endl;
#endif

    // Verify room occupancy constraint
    if (thisExamNumStudents > roomSeatCapacity)
        return false;
    else
        return true;
}





/**
 * @brief getRoomExamsSorted
 * @param _tj
 * @param _rk
 * @return Vector containing exams allocated to room rk, sorted by non-decreasing order (lower to greater) of # occuppied seats
 */
vector<pair<int, int> > eoChromosome::getRoomExamsSorted(int _tj, int _rk) const {
    // Vector containing exams allocated to room rk, sorted by non-decreasing order (lower to greater) of # occuppied seats
    // Each pair keeps the information <exam, # occuppied seats>
    vector<pair<int, int> > roomExams;
    // Get number of students for exam _ei
    auto const& examVector = this->getExamVector();
    // Timetable container
    TimetableContainer const& timetableCont = getTimetableContainer();
    // Get period_id exams
    auto const &periodExams = timetableCont.getPeriodExams(_tj);
    // For each exam of period periodj_id do
    for (auto const &examRoomTuple : periodExams)
    {
        // Get exam_id
        int exam_id = std::get<0>(examRoomTuple);
        // Get room_id
        int room_id = std::get<1>(examRoomTuple);
        if (room_id == _rk) {
            // Get # student enrolled for exam_j
            int thisExamNumStudents = examVector[exam_id]->getNumStudents();
            // Add exam to vector
            roomExams.push_back(make_pair(exam_id, thisExamNumStudents));
        }
    }
    // Sort exams by non-decreasing order (lower to greater) of # occuppied seats
    std::sort(roomExams.begin(), roomExams.end(),
              [](pair<int,int> const& p1, pair<int,int> const& p2) {
                    return p1.second < p2.second;
              });

    return roomExams;
}






/**
 * @brief getRoomExams
 * @param _tj
 * @param _rk
 * @return Vector containing exams allocated to room rk
 */
vector<pair<int, int> > eoChromosome::getRoomExams(int _tj, int _rk) const {
    // Vector containing exams allocated to room rk, sorted by non-decreasing order (lower to greater) of # occuppied seats
    // Each pair keeps the information <exam, # occuppied seats>
    vector<pair<int, int> > roomExams;
    // Get number of students for exam _ei
    auto const &examVector = this->getExamVector();
    // Timetable container
    TimetableContainer const &timetableCont = getTimetableContainer();
    // Get period_id exams
    auto const &periodExams = timetableCont.getPeriodExams(_tj);
    // For each exam of period periodj_id do
    for (auto const &examRoomTuple : periodExams)
    {
        // Get exam_id
        int exam_id = std::get<0>(examRoomTuple);
        // Get room_id
        int room_id = std::get<1>(examRoomTuple);
        if (room_id == _rk) {
            // Get # student enrolled for exam_j
            int thisExamNumStudents = examVector[exam_id]->getNumStudents();
            // Add exam to vector
            roomExams.push_back(make_pair(exam_id, thisExamNumStudents));
        }
    }
    return roomExams;
}




////////// Chromosome Misc //////////////////////////////

/**
 * @brief operator << Print chromosome contents
 * @param _os
 * @param _chrom
 * @return
 */
ostream& operator<<(ostream& _os, const eoChromosome &_chrom) {
    // Output Format
    //
    // The best solution found by the algorithm must be written to an output file with extension .sln in the following format.
    //
    // One line should describe each exam. The exams should be in the same order as that given in the input file. The following information relating to an exam should be provided:
    //
    // The timeslot number, the room number.
    // The timeslot number is an integer between 0 and 44 representing the timeslots allocated to the event.
    // The room is the room number assigned to the event. Remember, rooms are numbered in the order from the problem file, starting at zero.
    // Example
    //
    // 11, 0
    //
    // 27, 0
    //
    // 24, 2
    //
    // Means the first exam is timetabled in Period number 11 and is in Room 0 and so on. Competitors should remember that Rooms and Exams are represented by Integers beginning at zero.
    //
    // Every line should end with carriage return + linefeed (AKA "\r\n").
    //
    // Get exam vector
    vector<ScheduledExam> const& examVec = _chrom.getScheduledExamsVector();
    bool firstLine = true;
    for (auto const &exam : examVec) {
        if (firstLine)
            firstLine = false;
        else
            _os << "\r\n"; // For printing carriage return + linefeed for all lines except the last one.
        int period = exam.getPeriod();
        int room = exam.getRoom();
        _os << period << ", " << room;
    }
    return _os;
}











