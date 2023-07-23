#include <iostream>
#include "output_stream_console.h"


OutputStream& OutputStreamConsole::operator<<(std::string arg)
{
    this->stream << arg;
    return *this;
}

OutputStream& OutputStreamConsole::operator<<(std::ostream& (*arg)(std::ostream&))
{
    this->stream << arg;
    return *this;
}