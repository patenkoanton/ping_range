#include "output_stream.h"


OutputStream& OutputStream::operator<<(std::string arg)
{
    this->stream << arg;
    return *this;
}

OutputStream& OutputStream::operator<<(std::ostream& (*arg)(std::ostream&))
{
    this->stream << arg;
    return *this;
}