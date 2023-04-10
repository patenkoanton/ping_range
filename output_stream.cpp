#include "output_stream.h"


OutputStream& OutputStream::operator<<(std::string arg)
{
    this->show_output(arg);
    return *this;
}

OutputStream& OutputStream::operator<<(std::ostream& (*arg)(std::ostream&))
{
    this->show_output(arg);
    return *this;
}