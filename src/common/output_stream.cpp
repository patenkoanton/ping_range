#include "output_stream.h"
#ifdef __GUI__
    // Because in wxwidgets the form can only be updated from the main thread, and this thing is most likely running in a parallel thread,
    // we can't just print to the GUI like normal people.
    // Instead, we have to use ugly CallAfter() construction for 'async' output, otherwise everything will crash.
    // Thus, we need to include a GUI version of "main.h" to be able to access wxGetApp().
    // See manual for more details.
    #include "main.h"
#endif // __GUI__

OutputStream& OutputStream::operator<<(std::string arg)
{
    return this->write_to_stream(arg);
}

// Handle std::endl.
OutputStream& OutputStream::operator<<(std::ostream& (*arg)(std::ostream&))
{
    return this->write_to_stream(arg);
}

// Smart method to handle output from both console and GUI version.
template<class T>
OutputStream& OutputStream::write_to_stream(T arg)
{
#ifdef __GUI__
    wxGetApp().CallAfter([this, arg](){
        this->stream << arg;
    });
#else
    this->stream << arg;
#endif // __GUI__

    return *this;
}