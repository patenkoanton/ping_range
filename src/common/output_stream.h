#pragma once

#include <iostream>
#include <string>
#include "ip_address.h"

// Interface class for redirecting text output into different stream (GUI/console in our case).
// How to use:
//      1) Inherit from this class
//      2) Redefine both virtual operator<< methods to do whatever you wish.
//      3) ?????????
//      4) Magically works.
class OutputStream {
protected:
    std::ostream &stream;
public:
    OutputStream(std::ostream &stream) : stream(stream) {};
    virtual OutputStream& operator<<(std::string arg) = 0;
    virtual OutputStream& operator<<(std::ostream& (*arg)(std::ostream&)) = 0;  // handle std::endl
};