#ifndef KEMPECHAIN_H
#define KEMPECHAIN_H

#include <boost/unordered_set.hpp>

/**
 * @brief The ETTPKempeChain class
 *
 * This class keeps information about a Kempe chain, namely: the selected exam ei to move,
 * the source and destination time slots where the exam is being moved to in a feasible way,
 *
 */
template <typename EOT>
class ETTPKempeChain {

public:

    typedef std::tuple<int, int> ExamRoomTuple;

    /**
     * @brief ETTPKempeChain Default constructor
     */
    ETTPKempeChain()
        : chrom(nullptr), ei(-1), ti(-1), tj(-1),
          originalTiPeriodExams(0), originalTjPeriodExams(0),
          finalTiPeriodExams(0), finalTjPeriodExams(0)  { }

    /**
     * @brief getSolution Get the solution where the Kempe chain information is associated
     * @return
     */
    EOT &getSolution() const;
    /**
     * @brief setSolution Set the solution where the Kempe chain information is associated
     * @param _chrom
     */
    void setSolution(EOT *_chrom);

    /**
     * @brief getEi Get the selected exam ei to move
     * @return
     */
    int getEi() const;
    /**
     * @brief setEi Set the selected exam ei to move
     * @param _ei
     */
    void setEi(int _ei);

    /**
     * @brief getRi Get the selected exam roomi
     * @return
     */
    int getRi() const;
    /**
     * @brief setRi Set the selected exam roomi
     * @param _roomi
     */
    void setRi(int _roomi);

    /**
     * @brief getRj Get the selected exam roomj
     * @return
     */
    int getRj() const;
    /**
     * @brief setRj Set the selected exam roomj
     * @param _roomi
     */
    void setRj(int _roomj);

    /**
     * @brief getTi Get the Ti time slot
     * @return
     */
    int getTi() const;
    /**
     * @brief setTi Set the Ti time slot
     * @param _ti
     */
    void setTi(int _ti);

    /**
     * @brief getTj Get the Tj time slot
     * @return
     */
    int getTj() const;
    /**
     * @brief setTj Set the Tj time slot
     * @param _tj
     */
    void setTj(int _tj);

    /**
     * @brief getOriginalTiPeriodExams Get original Ti time slot exams
     * @return
     */
    const std::vector<ExamRoomTuple> &getOriginalTiPeriodExams() const;
    /**
     * @brief setOriginalTiPeriodExams Set original Ti time slot exams
     * @param _value
     */
    void setOriginalTiPeriodExams(const std::vector<ExamRoomTuple> &_value);

    /**
     * @brief getOriginalTjPeriodExams Get original Tj time slot exams
     * @return
     */
    const std::vector<ExamRoomTuple> &getOriginalTjPeriodExams() const;
    /**
     * @brief setOriginalTjPeriodExams Set original Tj time slot exams
     * @param _value
     */
    void setOriginalTjPeriodExams(const std::vector<ExamRoomTuple> &_value);

    /**
     * @brief getFinalTiPeriodExams Get final Ti time slot exams
     * @return
     */
    const std::vector<ExamRoomTuple> &getFinalTiPeriodExams() const;
    /**
     * @brief setFinalTiPeriodExams Set final Ti time slot exams
     * @param _value
     */
    void setFinalTiPeriodExams(const std::vector<ExamRoomTuple> &_value);

    /**
     * @brief getFinalTjPeriodExams Get final Tj time slot exams
     * @return
     */
    const std::vector<ExamRoomTuple> &getFinalTjPeriodExams() const;
    /**
     * @brief setFinalTjPeriodExams Set final Tj time slot exams
     * @param _value
     */
    void setFinalTjPeriodExams(const std::vector<ExamRoomTuple> &_value);

private:
    /**
     * @brief chrom The solution where the Kempe chain information is associated
     */
    EOT *chrom;
    /**
     * @brief ei The selected exam ei to move
     */
    int ei;
    /**
     * @brief ti Source time slot
     */
    int ti;
    /**
     * @brief roomi The selected source exam room
     */
    int roomi;
    /**
     * @brief roomj The selected source exam room. Used in room swap operator
     */
    int roomj;
    /**
     * @brief tj Destination time slot
     */
    int tj;
    /**
     * @brief originalTiPeriodExams Source Ti time slot exams
     * (original exams prior moving the solution)
     */
    std::vector<ExamRoomTuple> originalTiPeriodExams;
    /**
     * @brief originalTjPeriodExams Source Tj time slot exams
     * (original exams prior moving the solution)
     */
    std::vector<ExamRoomTuple> originalTjPeriodExams;
    /**
     * @brief finalTiPeriodExams Destination Ti time slot exams
     * (period contents after moving the solution)
     */
    std::vector<ExamRoomTuple> finalTiPeriodExams;
    /**
     * @brief finalTjPeriodExams Destination Tj time slot exams
     * (period contents after moving the solution)
     */
    std::vector<ExamRoomTuple> finalTjPeriodExams;
};


template <typename EOT>
EOT &ETTPKempeChain<EOT>::getSolution() const
{
    return *chrom;
}

template <typename EOT>
void ETTPKempeChain<EOT>::setSolution(EOT *_chrom)
{
    chrom = _chrom;
}

template <typename EOT>
int ETTPKempeChain<EOT>::getEi() const
{
    return ei;
}
template <typename EOT>
void ETTPKempeChain<EOT>::setEi(int _ei)
{
    ei = _ei;
}

template <typename EOT>
int ETTPKempeChain<EOT>::getRi() const
{
    return roomi;
}
template <typename EOT>
void ETTPKempeChain<EOT>::setRi(int _roomi)
{
    roomi = _roomi;
}

template <typename EOT>
int ETTPKempeChain<EOT>::getRj() const
{
    return roomj;
}
template <typename EOT>
void ETTPKempeChain<EOT>::setRj(int _roomj)
{
    roomj = _roomj;
}

template <typename EOT>
int ETTPKempeChain<EOT>::getTi() const
{
    return ti;
}
template <typename EOT>
void ETTPKempeChain<EOT>::setTi(int _ti)
{
    ti = _ti;
}

template <typename EOT>
int ETTPKempeChain<EOT>::getTj() const
{
    return tj;
}
template <typename EOT>
void ETTPKempeChain<EOT>::setTj(int _tj)
{
    tj = _tj;
}

template <typename EOT>
const std::vector<typename ETTPKempeChain<EOT>::ExamRoomTuple> &ETTPKempeChain<EOT>::getOriginalTiPeriodExams() const
{
    return originalTiPeriodExams;
}

template <typename EOT>
void ETTPKempeChain<EOT>::setOriginalTiPeriodExams(const std::vector<ExamRoomTuple> &_value)
{
    originalTiPeriodExams = _value;
}

template <typename EOT>
const std::vector<typename ETTPKempeChain<EOT>::ExamRoomTuple> &ETTPKempeChain<EOT>::getOriginalTjPeriodExams() const
{
    return originalTjPeriodExams;
}

template <typename EOT>
void ETTPKempeChain<EOT>::setOriginalTjPeriodExams(const std::vector<ExamRoomTuple> &_value)
{
    originalTjPeriodExams = _value;
}

template <typename EOT>
const std::vector<typename ETTPKempeChain<EOT>::ExamRoomTuple> &ETTPKempeChain<EOT>::getFinalTiPeriodExams() const
{
    return finalTiPeriodExams;
}

template <typename EOT>
void ETTPKempeChain<EOT>::setFinalTiPeriodExams(const std::vector<ExamRoomTuple> &_value)
{
    finalTiPeriodExams = _value;
}

template <typename EOT>
const std::vector<typename ETTPKempeChain<EOT>::ExamRoomTuple> &ETTPKempeChain<EOT>::getFinalTjPeriodExams() const
{
    return finalTjPeriodExams;
}

template <typename EOT>
void ETTPKempeChain<EOT>::setFinalTjPeriodExams(const std::vector<ExamRoomTuple> &_value)
{
    finalTjPeriodExams = _value;
}


#endif // KEMPECHAIN_H












