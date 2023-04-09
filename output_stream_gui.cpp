#include "output_stream_gui.h"
#include "gui_app.h"

// This class is redirecting output to our GUI main form.
// Because in wxwidgets the form can only be updated from the main thread, and this thing is running in a parallel thread,
// we can't just print to the GUI like normal people.
// Instead, we have to use ugly CallAfter() construction for 'async' output, otherwise everything will crash.
// See manual for more details.

void OutputStreamGUI::show_output(std::string &arg)
{
    wxGetApp().CallAfter([this, arg](){
        *this->stream << arg;
    });
}

void OutputStreamGUI::show_output(std::ostream& (*arg)(std::ostream&))
{
    wxGetApp().CallAfter([this, arg](){
        *this->stream << arg;
    });
}