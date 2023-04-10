#pragma once

#include <iostream>
#include <string>
#include <memory>
#include "ip_address.h"

class OutputStream {
protected:
    virtual void show_output(std::string &arg) = 0;
    virtual void show_output(std::ostream& (*arg)(std::ostream&)) = 0;      // handle std::endl
public:
    OutputStream& operator<<(std::string arg);
    OutputStream& operator<<(std::ostream& (*arg)(std::ostream&));  // handle std::endl
};