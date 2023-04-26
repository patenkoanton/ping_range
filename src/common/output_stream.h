#pragma once

#include <iostream>
#include <string>
#include "ip_address.h"

class OutputStream {
protected:
    std::ostream &stream;
public:
    OutputStream(std::ostream &stream) : stream(stream) {};
    virtual OutputStream& operator<<(const std::string &arg);
    virtual OutputStream& operator<<(std::ostream& (*arg)(std::ostream&));  // handle std::endl
};