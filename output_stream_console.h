#pragma once
#include <string>
#include "output_stream_base.h"

class OutputStreamConsole: public OutputStreamBase {
    virtual void show_output(std::string &arg);
    virtual void show_output(std::ostream& (*arg)(std::ostream&));
};
