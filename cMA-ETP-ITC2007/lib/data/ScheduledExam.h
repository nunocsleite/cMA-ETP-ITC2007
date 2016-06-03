#ifndef SCHEDULEDEXAM_H
#define SCHEDULEDEXAM_H


class ScheduledExam {

public:
    // Constructors
    /**
     * @brief ScheduledExam
     */
    inline ScheduledExam();

    /**
     * @brief Exam
     * @param _id
     */
    inline ScheduledExam(int _id);

    // Public interface
    /**
     * @brief getId
     * @return
     */
    inline int getId() const;

    /**
     * @brief setId
     * @param _id
     */
    inline void setId(int _id);

    /**
     * @brief getPeriod
     * @return
     */
    inline int getPeriod() const;
    /**
     * @brief getRoom
     * @return
     */
    inline int getRoom() const;
    /**
     * @brief isScheduled
     * @return
     */
    inline bool isScheduled() const;
    /**
     * @brief schedule
     * @param _tj
     * @param _rk
     */
    inline void schedule(int _tj, int _rk);
    /**
     * @brief unschedule
     */
    inline void unschedule();

private:
    // Methods
    /**
     * @brief setPeriod
     * @param _value
     */
    inline void setPeriod(int _value);
    /**
     * @brief setRoom
     * @param _value
     */
    inline void setRoom(int _value);

    // Fields
    /**
     * @brief id
     */
    int id;
    /**
     * @brief period
     */
    int period;
    /**
     * @brief room
     */
    int room;
    /**
     * @brief scheduled
     */
    bool scheduled;
};


// Constructors
/**
 * @brief ScheduledExam
 */
ScheduledExam::ScheduledExam()
    : id(-1), period(-1), room(-1), scheduled(false)
{ }
/**
 * @brief Exam
 * @param _id
 */
ScheduledExam::ScheduledExam(int _id)
    : id(_id), period(-1), room(-1), scheduled(false)
{ }

// Public interface
/**
 * @brief getId
 * @return
 */
int ScheduledExam::getId() const {
    return id;
}

/**
 * @brief setId
 * @param _id
 */
void ScheduledExam::setId(int _id) {
    id = _id;
}

/**
 * @brief getPeriod
 * @return
 */
int ScheduledExam::getPeriod() const {
    return period;
}
/**
 * @brief getRoom
 * @return
 */
int ScheduledExam::getRoom() const {
    return room;
}

/**
 * @brief isScheduled
 * @return
 */
bool ScheduledExam::isScheduled() const {
    return scheduled;
}

/**
 * @brief schedule
 * @param _tj
 * @param _rk
 */
void ScheduledExam::schedule(int _tj, int _rk) {
    //
    // Schedule exam this in time slot 'tj' and room 'rk'
    //
    setPeriod(_tj);
    setRoom(_rk);
    // Set scheduled to true
    scheduled = true;
}

// Private methods
/**
 * @brief setPeriod
 * @param _value
 */
void ScheduledExam::setPeriod(int _value) {
    period = _value;
}
/**
 * @brief setRoom
 * @param _value
 */
void ScheduledExam::setRoom(int _value) {
    room = _value;
}

/**
 * @brief unschedule
 */
void ScheduledExam::unschedule() {
    //
    // Unschedule this exam
    //
    setPeriod(-1);
    setRoom(-1);
    // Set scheduled to false
    scheduled = false;
}


#endif // SCHEDULEDEXAM_H















