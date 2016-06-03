
#include "TestSetDescription.h"


std::ostream& operator<<(std::ostream& _os, const TestSetDescription& _t) {
    _os << _t.getName();

    return _os;
}

