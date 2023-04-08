#include "output_stream_gui.h"

void OutputStreamGUI::show_output(std::string &arg)
{
    buffer += arg;
}

void OutputStreamGUI::show_output(std::ostream& (*arg)(std::ostream&))
{
    this->output->InsertItem(0, buffer);
    buffer.clear();
}