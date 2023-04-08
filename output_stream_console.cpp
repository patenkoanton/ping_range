#include <iostream>
#include "output_stream_console.h"

void OutputStreamConsole::show_output(std::string &arg)
{
    std::cout << arg;
}

void OutputStreamConsole::show_output(std::ostream& (*arg)(std::ostream&))
{
    std::cout << arg;
}