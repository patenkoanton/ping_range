#pragma once

#include <string>
#include <iostream>
#include <memory>
#include "output_stream.h"

class OutputStreamGUI : public OutputStream {
    std::shared_ptr<std::ostream> stream;
protected:
    virtual void show_output(std::string &arg);
    virtual void show_output(std::ostream& (*arg)(std::ostream&));      // handle std::endl
public:
    OutputStreamGUI(std::shared_ptr<std::ostream> stream) : stream(stream) {};
};