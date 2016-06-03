#ifndef ETTPNEIGHBOR_H
#define ETTPNEIGHBOR_H

#include <neighborhood/moNeighbor.h>
#include "kempeChain/ETTPKempeChainHeuristic.h"
#include <eoEvalFunc.h>
#include <boost/shared_ptr.hpp>


//#define ETTPNEIGHBOR_DEBUG


/**
 * Neighbor computation for the ETTP problem (used in local search)
 */
template<class EOT, class Fitness = typename EOT::Fitness>
class ETTPneighbor: virtual public moNeighbor<EOT, typename EOT::Fitness> {

public:
    using moNeighbor<EOT, typename EOT::Fitness>::fitness;

    /**
     * Return the class Name
     * @return the class name as a std::string
     */
    virtual std::string className() const {
        return "ETTPneighbor";
    }


    /**
     * +Overriden method
     *
     * Apply move on a solution
     * @param _sol the solution to move
     */
    virtual void move(EOT& _sol) override {
        //
        // Move current solution _sol to this neighbour using
        // information maintained in the KempeChain structure
        //
        (*kempeChainHeuristic.get())(_sol); // Polymorphic behaviour
    }


    /**
     * @brief build Build neighbour. This consists in using kempe chain heuristic
     * to create a kempe chain for a random move
     * @param _sol
     */
    virtual void build(EOT& _sol) {
        (*kempeChainHeuristic.get()).build(_sol);
    }

    /**
     * @brief getKempeChain
     * @return The Kempe chain object relating the neighbor and the current solution
     */
    ETTPKempeChain<EOT> const& getKempeChain() {
        return kempeChainHeuristic->getKempeChain();
    }

    /**
     * @brief setKempeChainHeuristic
     * @param _kempeChainHeuristic
     */
    void setKempeChainHeuristic(boost::shared_ptr<ETTPKempeChainHeuristic<EOT> > const &_kempeChainHeuristic) {
        kempeChainHeuristic = _kempeChainHeuristic;
    }

    /**
     * @brief getSolutionCost
     * @return
     */
    long getSolutionCost() const {
        return kempeChainHeuristic->getNeighborSolutionCost();
    }

    /**
     * @brief isFeasible
     * @return
     */
    bool isFeasible() const {
        // Return true if the neighbour built in the Kempe chain heuristic is feasible and return false otherwise.
        return kempeChainHeuristic->isFeasibleNeighbour();
    }

    /**
     * @brief evaluateMove Evaluate move of solution to the neighbour.
     * This envolves doing:
     *   - move, temporarily, the solution to the neighbour
     *   - record ti and tj final time slots
     *   - evaluate neighbour and set neighbour fitness
     *   - undo solution move
     * @param _sol
     */
    virtual void evaluateMove(EOT &_sol) {
        double neighFitness;
        // Evaluate move of solution to the neighbour
        kempeChainHeuristic->evaluateSolutionMove(_sol);
        // Get neighbor fitness
        neighFitness = kempeChainHeuristic->getNeighborFitness();
        // Set neighbor fitness
        fitness(neighFitness);
    }

protected:

    /**
     * @brief kempeChainHeuristic Kempe Chain neighbourhood
     */
    boost::shared_ptr<ETTPKempeChainHeuristic<EOT> > kempeChainHeuristic;
};


#endif // ETTPNEIGHBOR_H




