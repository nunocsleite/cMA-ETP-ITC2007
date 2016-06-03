#include <eo>

#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include "testset/TestSetDescription.h"
#include "testset/ITC2007TestSet.h"
#include "init/ETTPInit.h"

#include "neighbourhood/ETTPneighborEval.h"
#include "eval/eoETTPEval.h"
#include "neighbourhood/ETTPneighbor.h"
#include "algorithms/eo/eoCellularEAMatrix.h"
#include "algorithms/eo/Mutation.h"
#include "algorithms/eo/Crossover.h"
#include "algorithms/eo/eoSelectBestOne.h"

#include "algorithms/eo/eoGenerationContinuePopVector.h"
#include "eoSelectOne.h"
#include "algorithms/eo/eoDeterministicTournamentSelectorPointer.h" // eoDeterministicTournamentSelector using boost::shared_ptr

// For counting the # evaluations
#include "eval/eoNumberEvalsCounter.h"

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>


#include <chrono>
#include <thread>


#include "containers/ConflictBasedStatistics.h"


using namespace std;




#define MAINAPP_DEBUG


extern int getSANumberEvaluations(double tmax, double r, double k, double tmin);


// These function is defined below
void runCellularEA(string const& _outputDir, TestSet const& _testSet);
void generateExamMoveStatistics(const string &_outputDir, const TestSet &_testSet);



////////////////////////////////////////////////////////
// ITC 2007 benchmarks
//
//
void runITC2007Datasets(int _datasetIndex, string const& _testBenchmarksDir, string const& _outputDir)
{
    /// ADDED 25-JAN-2016
    ///

    std::srand(std::time(0)); // Seed random generator used in random_shuffle


//    cout << endl << "Start Date/Time = " << currentDateTime() << endl;
    // Start time
    auto start = std::chrono::high_resolution_clock::now();

    vector<TestSetDescription> itc2007Benchmarks;
    itc2007Benchmarks.push_back(TestSetDescription("exam_comp_set1.exam", "ITC2007 Examination Track Dataset 1"));
    itc2007Benchmarks.push_back(TestSetDescription("exam_comp_set2.exam", "ITC2007 Examination Track Dataset 2"));
    itc2007Benchmarks.push_back(TestSetDescription("exam_comp_set3.exam", "ITC2007 Examination Track Dataset 3"));
    itc2007Benchmarks.push_back(TestSetDescription("exam_comp_set4.exam", "ITC2007 Examination Track Dataset 4"));
    itc2007Benchmarks.push_back(TestSetDescription("exam_comp_set5.exam", "ITC2007 Examination Track Dataset 5"));
    itc2007Benchmarks.push_back(TestSetDescription("exam_comp_set6.exam", "ITC2007 Examination Track Dataset 6"));
    itc2007Benchmarks.push_back(TestSetDescription("exam_comp_set7.exam", "ITC2007 Examination Track Dataset 7"));
    itc2007Benchmarks.push_back(TestSetDescription("exam_comp_set8.exam", "ITC2007 Examination Track Dataset 8"));
    // Hidden datasets
    itc2007Benchmarks.push_back(TestSetDescription("exam_comp_set9.exam",  "ITC2007 Examination Track Dataset 9"));
    itc2007Benchmarks.push_back(TestSetDescription("exam_comp_set10.exam", "ITC2007 Examination Track Dataset 10"));
    itc2007Benchmarks.push_back(TestSetDescription("exam_comp_set11.exam", "ITC2007 Examination Track Dataset 11"));
    itc2007Benchmarks.push_back(TestSetDescription("exam_comp_set12.exam", "ITC2007 Examination Track Dataset 12"));
    /////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef MAINAPP_DEBUG
//    copy(itc2007Benchmarks.begin(), itc2007Benchmarks.end(), ostream_iterator<TestSetDescription>(cout, "\n"));
#endif
    // Iterator to test set
    vector<TestSetDescription>::iterator it = itc2007Benchmarks.begin() + _datasetIndex;
    // Create TestSet instance
    ITC2007TestSet testSet((*it).getName(), (*it).getDescription(), _testBenchmarksDir);
    // Load dataset
    ITC2007TestSet* ptr = &testSet;
    ptr->load();
#ifdef MAINAPP_DEBUG
    // Print testset info
    cout << testSet << endl;
    // Print timetable problem data
    cout << *(ptr->getTimetableProblemData().get()) << endl;
#endif

    // Run test set
    runCellularEA(_outputDir, testSet);
}







void runCellularEA(const string &_outputDir, TestSet const& _testSet) {
    //
    // cEA parameters
    //


    ////// Pop size = 16  ////////
    // Rect
//    const int NLINES = 2;
//    const int NCOLS = 8;


/// COR2015 Paper
//    // Matrix
    const int NLINES = 4;
    const int NCOLS = 4;

    // Matrix
//    const int NLINES = 10;
//    const int NCOLS = 10;


//    // Ring
//    const int NLINES = 1;
//    const int NCOLS = 16;
    //////////////////////////////

    const int POP_SIZE = NLINES*NCOLS;  // Population size
    const int L = 5000000; // Number of generations

    // Crossover probability
    const double cp = 0;

    // Mutation probability
//    const double mp = 1;
//        const double mp = 0.5;
    const double mp = 0.1;
//    const double mp = 0.01;
//    const double mp = 0;

    // Improve probability
    const double ip = 0.1;
//    const double ip = 0.5;
//      const double ip = 1;
//      const double ip = 0;

    // TA parameters
    moSimpleCoolingSchedule<eoChromosome> coolSchedule(10, 0.001, 5, 2e-4);


    // Creating the output file in the specified output directory
    stringstream sstream;
    sstream << _outputDir << _testSet.getName() << "_NLINES_" << NLINES << "_NCOLS_" << NCOLS
            << "_cp_" << cp << "_mp_" << mp << "_ip_" << ip
            << "_cool_" << coolSchedule.initT << "_" << coolSchedule.alpha << "_"
            << coolSchedule.span << "_" << coolSchedule.finalT << ".txt";

    string filename;
    sstream >> filename;
    ofstream outFile(filename);

    cout << "Start Date/Time = " << currentDateTime() << endl;
    // Write Start time and algorithm parameters to file
    outFile << "Start Date/Time = " << currentDateTime() << endl;
    // Print Test set info
    cout << _testSet << endl;
    // Write testset info to file
    outFile << _testSet << endl;
    // getSANumberEvaluations(double tmax, double r, double k, double tmin)
    long numEvalsTA = getSANumberEvaluations(coolSchedule.initT, coolSchedule.alpha,
                                             coolSchedule.span, coolSchedule.finalT);
    /////////////////////////// Writing the cGA parameters ////////////////////////////////////////////////////////
    cout << "cGA parameters:" << endl;
    cout << "NLINES = " << NLINES << ", NCOLS = " << NCOLS << endl;
    cout << "cp = " << cp << ", mp = " << mp << ", ip = " << ip << endl;
    cout << "TA parameters:" << endl;
    cout << "cooling schedule: " << coolSchedule.initT << ", " << coolSchedule.alpha << ", "
            << coolSchedule.span << ", " << coolSchedule.finalT << endl;
    cout << "# evals per TA local search: " << numEvalsTA << endl;
    ///
    outFile << "cGA parameters:" << endl;
    outFile << "NLINES = " << NLINES << ", NCOLS = " << NCOLS << endl;
    outFile << "cp = " << cp << ", mp = " << mp << ", ip = " << ip << endl;
    outFile << "TA parameters:" << endl;
    outFile << "cooling schedule: " << coolSchedule.initT << ", " << coolSchedule.alpha << ", "
            << coolSchedule.span << ", " << coolSchedule.finalT << endl;
    outFile << "# evals per TA local search: " << numEvalsTA << endl;
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Solution initializer
    ETTPInit<eoChromosome> init(_testSet.getTimetableProblemData().get());
    // Generate initial population
//    eoPop<eoChromosome> pop(POP_SIZE, init);
      // We can't work with eoPop of shared_ptr because shared_ptr is not an EO
//    boost::shared_ptr<eoPop<boost::shared_ptr<eoChromosome> > > pop(new eoPop<boost::shared_ptr<eoChromosome> >());
    // Solution: Work with vector<shared_ptr<EOT> > directly
    boost::shared_ptr<vector<boost::shared_ptr<eoChromosome> > > pop(new vector<boost::shared_ptr<eoChromosome> >());

    auto &solutionPop = *pop.get();
    for (int i = 0; i < POP_SIZE; ++i) {
        // Create solution object and insert it in the vector
        solutionPop.push_back(boost::shared_ptr<eoChromosome>(new eoChromosome(_testSet.getTimetableProblemData().get())));
        // Initialize chromosome
        init(*solutionPop[i].get());
    }

    // # evaluations counter
    eoNumberEvalsCounter numEvalCounter;
    // Objective function evaluation
    eoETTPEval<eoChromosome> eval;
    // Objective function evaluation
    for (int i = 0; i < solutionPop.size(); ++i) {
        eval(*solutionPop[i].get());
    }


    /////////////////////////////////////////////////////////////////////////////////
    // Print population information to output and
    // save population information into file
    std::cout << std::endl << "Initial population" << std::endl;
    outFile << std::endl << "Initial population" << std::endl;
    int k = 0;
    for (int i = 0; i < NLINES; ++i) {
        for (int j = 0; j < NCOLS; ++j, ++k) {
            std::cout << (*solutionPop[k].get()).fitness() << "\t";
            outFile << (*solutionPop[k].get()).fitness() << "\t";
        }
        std::cout << std::endl;
        outFile << std::endl;
    }
    /////////////////////////////////////////////////////////////////////////////////

    //
    // Build CellularGA
    //
    // Terminate after concluding L time loops or 'Ctrl+C' signal is received
//    eoGenerationContinue<eoChromosome> terminator(L);
//    eoCheckPoint<eoChromosome> checkpoint(terminator);
    // The eoGenerationContinuePopVector object, instead of using an eoPop to represent the population,
    // uses a vector. A vector is used in order to swap offspring and population efficiently using pointers
    eoGenerationContinuePopVector<eoChromosome> terminator(L);

    // Declare 1-selectors
    //
    // Binary deterministic tournament selector used in neighbour selection
//    eoDetTournamentSelect<eoChromosome> detSelectNeighbour;
    // Work with pointers for efficiency
    eoDetTournamentSelectSharedPtr<eoChromosome> detSelectNeighbourPtr;

    // Crossover and mutation
    Mutation<eoChromosome> mutation;
    Crossover<eoChromosome> crossover;
    eoSelectBestOne<eoChromosome> selectBestOne;

    boost::shared_ptr<eoCellularEA<eoChromosome> > cGA;
    // Build the corresponding type of cGA object depending on the layout (Ring or Matrix)
    if (NLINES == 1) { // Ring cGA
        /// NOT IMPLEMENTED
    }
    else {
        cGA = boost::make_shared<eoCellularEAMatrix<eoChromosome> >(
                    outFile, NLINES, NCOLS,
                    cp, mp, ip, coolSchedule,
                    terminator,
//                    checkpoint,
                    eval,
//                    detSelectNeighbour,
                    detSelectNeighbourPtr, // Work with pointers for efficiency
                    crossover, mutation,
                    selectBestOne, // To choose one from the both children
                    selectBestOne,  // Which to keep between the new child and the old individual?
                    numEvalCounter
        );

    }

    // Run the algorithm
    (*cGA.get())(pop);


    //
    // Create solution file and run Rong Qu's exam evaluator
    //
    string solutionFilename = _testSet.getName() + ".sol";
    ofstream solutionFile(_outputDir + solutionFilename);
    // Write best solution to file
    solutionFile << (*(*cGA.get()).getBestSolution());
    // Close solution file
    solutionFile.close();

    // Write end Date/Time
    cout << "End Date/Time = " << currentDateTime() << endl;
    // Write to file
    outFile << "End Date/Time = " << currentDateTime() << endl;
    // Close output file
    outFile.close();

}





