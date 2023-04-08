#include "output_stream_base.h"


OutputStreamBase& OutputStreamBase::operator<<(std::string arg)
{
    this->show_output(arg);
    return *this;
}

OutputStreamBase& OutputStreamBase::operator<<(std::ostream& (*arg)(std::ostream&))
{
    this->show_output(arg);
    return *this;
}