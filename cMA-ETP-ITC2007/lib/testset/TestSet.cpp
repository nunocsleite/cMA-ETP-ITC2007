

#include "testset/TestSet.h"



///////////////////////////////////////////////////////////////////////////////
//  Helper function reading a file into a string
///////////////////////////////////////////////////////////////////////////////
std::string read_from_file(char const* infile)
{
    std::ifstream instream(infile);
    if (!instream.is_open()) {
        std::cerr << "Couldn't open file: " << infile << std::endl;
        exit(-1);
    }
    instream.unsetf(std::ios::skipws);      // No white space skipping!
    return std::string(std::istreambuf_iterator<char>(instream.rdbuf()),
                       std::istreambuf_iterator<char>());
}


/**
 * @brief operator <<
 * @param _os
 * @param _t
 * @return
 */
std::ostream& operator<<(std::ostream& _os, const TestSet& _t) {
    _os << "Testset info:" << std::endl
        << _t.getName() << std::endl
        << _t.getDescription() << std::endl
        << _t.getRootDirectory() << std::endl;
    return _os;
}
