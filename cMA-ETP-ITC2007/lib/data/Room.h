#ifndef ROOM_H
#define ROOM_H

/**
 * @brief The Room class
 */
class Room {
public:
    // Constructor
    inline Room(int _id, int _capacity, int _penalty);

    // Public interface
    /**
     * @brief getId
     * @return
     */
    inline int getId() const;
    /**
     * @brief getCapacity
     * @return
     */
    inline int getCapacity() const;
    /**
     * @brief getPenalty
     * @return
     */
    inline int getPenalty() const;

private:
    // Private fields

    /**
     * @brief id Room id
     */
    int id;
    /**
     * @brief capacity Room capacity
     */
    int capacity;
    /**
     * @brief penalty Room penalty
     */
    int penalty;
};


// Constructor
Room::Room(int _id, int _capacity, int _penalty)
    : id(_id), capacity(_capacity), penalty(_penalty)
{ }

// Public interface
/**
 * @brief getId
 * @return
 */
int Room::getId() const { return id; }
/**
 * @brief getCapacity
 * @return
 */
int Room::getCapacity() const { return capacity; }
/**
 * @brief getPenalty
 * @return
 */
int Room::getPenalty() const { return penalty; }


#endif // ROOM_H


















