
#include <string>
#include <iostream>

// Handling Ctrl+c signal
#include <signal.h>

bool volatile keepRunning = true;

void intHandler(int dummy=0) {

//    std::cout << "Catch signal" << std::endl;
//    std::cin.get();
    keepRunning = false;
}

using namespace std;



// Toronto benchmarks
extern void runITC2007Datasets(int _datasetIndex, string const& _testBenchmarksDir, const string& _outputDir);


void runAlgo(int _datasetIndex, string const& _testBenchmarksDir, const string& _outputDir) {
    // Handling Ctrl+c signal
    signal(SIGINT, intHandler);

    // Run optimization algorithm on the ITC2007 datasets
    runITC2007Datasets(_datasetIndex, _testBenchmarksDir, _outputDir);
}













