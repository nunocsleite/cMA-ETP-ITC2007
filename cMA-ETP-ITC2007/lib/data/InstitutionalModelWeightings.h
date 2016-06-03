#ifndef INSTITUTIONALMODELWEIGHTINGS_H
#define INSTITUTIONALMODELWEIGHTINGS_H


#include <vector>


class InstitutionalModelWeightings {

public:
    /**
     * @brief InstitutionalModelWeightings Default constructor
     */
    InstitutionalModelWeightings() { }

    /**
     * @brief InstitutionalModelWeightings
     * @param _two_in_a_row
     * @param _two_in_a_day
     * @param _period_spread
     * @param _front_load
     * @param _non_mixed_durations
     */
    InstitutionalModelWeightings(int _two_in_a_row, int _two_in_a_day, int _period_spread,
                                 std::vector<int> const &_front_load, int _non_mixed_durations)
        : two_in_a_row(_two_in_a_row), two_in_a_day(_two_in_a_day),
        period_spread(_period_spread), front_load(_front_load),
        non_mixed_durations(_non_mixed_durations)
    { }


public:
    /**
     * @brief two_in_a_row
     */
    int two_in_a_row;
    /**
     * @brief two_in_a_day
     */
    int two_in_a_day;
    /**
     * @brief period_spread
     */
    int period_spread;
    /**
     * @brief front_load
     * First element - numberLargestExams - number of largest exams. Largest exams are specified by class size
     * Second element - numberLastPeriods - number of last periods to take into account
     * Third element - weightFL -the penalty or weighting
     */
    std::vector<int> front_load;
    /**
     * @brief non_mixed_durations
     */
    int non_mixed_durations;
};

#endif // INSTITUTIONALMODELWEIGHTINGS_H












