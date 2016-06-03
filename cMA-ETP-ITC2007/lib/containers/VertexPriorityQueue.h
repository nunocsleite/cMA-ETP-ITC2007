#ifndef VERTEX_PRIORITY_QUEUE
#define VERTEX_PRIORITY_QUEUE

#include <boost/heap/fibonacci_heap.hpp>
#include <iostream>

using namespace boost;
using namespace boost::heap;
using namespace std;


class VertexPriorityQueue {

public:
	struct heap_data {
		int vertex;
		int priority;

		heap_data(int v, int priority) : vertex(v), priority(priority) { }

		/*bool operator<(const heap_data& rhs) const {
			return priority < rhs.priority;
		}*/
        // This is a min-heap
		bool operator>(const heap_data& rhs) const {
			return priority > rhs.priority;
		}

		friend ostream& operator<<(ostream& os, const heap_data& t) {
			os << t.vertex << " - " << t.priority << ", ";
			return os;
		}
	};

    /**
     * @brief VertexPriorityQueue Default Ctor
     * @param nvertices
     */
    VertexPriorityQueue();

    /**
     * @brief VertexPriorityQueue
     * @param nvertices
     */
	VertexPriorityQueue(int nvertices);

    /**
     * @brief init
     * @param nvertices
     */
    void init(int nvertices);

    /**
     * @brief push
     * @param vertex
     * @param priority
     */
	void push(int vertex, int priority);

    /**
     * @brief top
     * @return
     */
    heap_data const& top() const;

    /**
     * @brief pop
     */
    void pop();

    /**
     * @brief update
     * @param vertex
     * @param newPriority
     */
    void update(int vertex, int newPriority);

    /**
     * @brief empty
     * @return
     */
    bool empty() const;

    /**
     * @brief printQueue
     */
    void printQueue() const ;

private:
	typedef boost::heap::fibonacci_heap<
		heap_data, 
		boost::heap::compare<std::greater<heap_data> > > PQ;
	typedef PQ::handle_type handle_t;
	
	// Priority queue
	PQ pq;
	// For storing handles
	vector<handle_t> handles;
};


#endif
