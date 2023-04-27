#pragma once

#include <string>
#include <iostream>
#include "output_stream.h"

struct OutputStreamGUI : public OutputStream {
    OutputStreamGUI(std::ostream &stream) : OutputStream(stream) {};
    virtual OutputStream& operator<<(std::string arg);
    virtual OutputStream& operator<<(std::ostream& (*arg)(std::ostream&));  // handle std::endl
};