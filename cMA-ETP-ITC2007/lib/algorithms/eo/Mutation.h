#ifndef MUTATION_H
#define MUTATION_H

#include "chromosome/eoChromosome.h"
#include <eoOp.h>
#include <string>

#include "neighbourhood/ETTPneighborhood.h"
#include "neighbourhood/ETTPneighborEval.h"

#include "kempeChain/ETTPKempeChainHeuristic.h"


template <typename EOT>
class Mutation : public eoMonOp<EOT>
{

public:

    /**
    * the class name (used to display statistics)
    */
    std::string className() const;

    /**
    * eoMon mutation - _chromosome is the chromosome to mutate.
    * @param _chromosome the chromosome
    */
    bool operator()(EOT& _chromosome);
};



template <typename EOT>
std::string Mutation<EOT>::className() const
{
    return "ETTP Mutation";
}


template <typename EOT>
bool Mutation<EOT>::operator()(EOT& _chrom)
{
//    cout << "Mutation" << endl;

    bool chromosomeIsModified = true;

    ETTPKempeChainHeuristic<EOT> kempe;
    kempe.build(_chrom);
    kempe.evaluateSolutionMove(_chrom);
    kempe(_chrom);


    if (!kempe.isFeasibleNeighbour()) {
        kempe.undoSolutionMove(_chrom);
    }
    else {
        _chrom.setSolutionCost(kempe.getNeighborSolutionCost());
        _chrom.fitness(_chrom.getSolutionCost());
    }

    // Return 'true' if at least one genotype has been modified
    return chromosomeIsModified;
}



#endif // MUTATION_H
