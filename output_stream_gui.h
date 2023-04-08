#pragma once

#include <string>
#include <wx/listctrl.h>
#include "output_stream_base.h"

class OutputStreamGUI : public OutputStreamBase {
    wxListCtrl *output;
    std::string buffer;
protected:
    virtual void show_output(std::string &arg);
    virtual void show_output(std::ostream& (*arg)(std::ostream&));      // handle std::endl
public:
    OutputStreamGUI(wxListCtrl *output): output(output) {};
};