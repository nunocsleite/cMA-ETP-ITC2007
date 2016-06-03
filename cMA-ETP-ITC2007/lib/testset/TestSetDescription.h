#ifndef TESTSETDESCRIPTION_H
#define TESTSETDESCRIPTION_H

#include <string>
#include <ostream>



class TestSetDescription {
    std::string name;
    std::string description;
public:

    TestSetDescription(std::string _name, std::string _description) :
        name(_name), description(_description) { }

    std::string getName() const { return name; }
    std::string getDescription() const { return description; }

    friend std::ostream& operator<<(std::ostream& os, const TestSetDescription& t);
};


#endif // TESTSETDESCRIPTION_H
