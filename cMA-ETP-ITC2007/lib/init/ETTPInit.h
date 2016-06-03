#ifndef ETTPINIT_H
#define ETTPINIT_H

#include <eoInit.h>
#include <iostream>
#include "graphColouring/GraphColouringHeuristics.h"

//#include "init/SolutionConstruction.h"


// For debugging purposes
//#define ETTPINIT_DEBUG



template <typename EOT>
class ETTPInit : public eoInit<EOT> {

public:
    ETTPInit(TimetableProblemData const *_timetableProblemData)
        : timetableProblemData(_timetableProblemData) { }


    virtual void operator()(EOT &_chrom) {

#ifdef ETTPINIT_DEBUG
            std::cout << "ETTPInit::operator()" << std::endl;
#endif

            // Exams are then inserted into randomly selected periods in the order
            // determined by the graph colouring heuristic.
            // When it is not possible to schedule an exam without violating any of
            // the hard constraints, the chromosome is set to be infeasible and the
            // method returns to the caller.
            GCHeuristics<EOT>::saturationDegree(timetableProblemData, _chrom);

#ifdef ETTPINIT_DEBUG
            std::cout << "After invoking [GCHeuristics<EOT>::saturationDegree]: chromosome feasible? "
                      << _chrom.isFeasible() << std::endl;
//            std::cin.get();
#endif

    }

private:

    // Instance fields
    TimetableProblemData const *timetableProblemData;
};



#endif // ETTPINIT_H
