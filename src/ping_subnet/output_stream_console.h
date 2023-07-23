#pragma once

#include <string>
#include <iostream>
#include "output_stream.h"

// This class is redirecting output to console.
// Used by console version of the app aka ping_subnet.
struct OutputStreamConsole : public OutputStream {
    OutputStreamConsole(std::ostream &stream) : OutputStream(stream) {};
    virtual OutputStream& operator<<(std::string arg);
    virtual OutputStream& operator<<(std::ostream& (*arg)(std::ostream&));  // handle std::endl
};