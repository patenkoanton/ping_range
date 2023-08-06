#pragma once

#include <iostream>
#include <string>
#include "ip_address.h"

// Interface class for redirecting text output into different stream (GUI/console in our case).
class OutputStream {
    std::ostream &stream;
    template<class T> OutputStream& write_to_stream(T arg);
public:
    OutputStream(std::ostream &stream) : stream(stream) {};
    OutputStream& operator<<(std::string arg);
    OutputStream& operator<<(std::ostream& (*arg)(std::ostream&));  // handle std::endl
};