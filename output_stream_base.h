#pragma once

#include <iostream>
#include <string>
#include <memory>
#include "ip_address.h"

class OutputStreamBase {
protected:
    virtual void show_output(std::string &arg) = 0;
    virtual void show_output(std::ostream& (*arg)(std::ostream&)) = 0;      // handle std::endl
public:
    OutputStreamBase& operator<<(std::string arg);
    OutputStreamBase& operator<<(std::ostream& (*arg)(std::ostream&));  // handle std::endl
};