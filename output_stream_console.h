#pragma once
#include <string>
#include "output_stream.h"

class OutputStreamConsole: public OutputStream {
    virtual void show_output(std::string &arg);
    virtual void show_output(std::ostream& (*arg)(std::ostream&));
};
