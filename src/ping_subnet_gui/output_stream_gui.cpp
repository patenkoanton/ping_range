#include "output_stream_gui.h"
#include "main.h"

// This class is redirecting output to our GUI main form.
// Because in wxwidgets the form can only be updated from the main thread, and this thing is running in a parallel thread,
// we can't just print to the GUI like normal people.
// Instead, we have to use ugly CallAfter() construction for 'async' output, otherwise everything will crash.
// See manual for more details.

OutputStream& OutputStreamGUI::operator<<(std::string arg)
{
    wxGetApp().CallAfter([this, arg](){
        this->stream << arg;
    });
    return *this;
}

OutputStream& OutputStreamGUI::operator<<(std::ostream& (*arg)(std::ostream&))
{
    wxGetApp().CallAfter([this, arg](){
        this->stream << arg;
    });
    return *this;
}