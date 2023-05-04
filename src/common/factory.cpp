#include <iostream>
#include <string>
#include "factory.h"
#include "ping.h"
#include "subnet.h"
#include "socket.h"
#include "ip_address.h"
#include "orchestrator.h"
#include "output_stream.h"


// Return class <T> object wrapped in unique_ptr.
// Throw std::string if allocation fails.
template<class T, class... Args> std::unique_ptr<T> factory_make_unique(Args... args)
{
    T *object_p;
    try {
        object_p = new T(args...);
    } catch (std::string& exception) {
        std::cerr << "ERROR: " + exception << ". Failed to allocate memory for application." << std::endl;
        throw exception;
    }

    return std::unique_ptr<T>(object_p);        // gets moved (magic semantics of std::unique_ptr)
}

// 'Unique' instances.
template std::unique_ptr<Ping> factory_make_unique<Ping, OutputStream&>(OutputStream&);
template std::unique_ptr<Subnet> factory_make_unique<Subnet, const std::string&, OutputStream&>(const std::string&, OutputStream&);
template std::unique_ptr<Socket> factory_make_unique<Socket, OutputStream&>(OutputStream&);
template std::unique_ptr<IPAddress> factory_make_unique<IPAddress, uint32_t>(uint32_t);
template std::unique_ptr<IPAddress> factory_make_unique<IPAddress, const std::string&>(const std::string&);
template std::unique_ptr<IPAddress> factory_make_unique<IPAddress, IPAddress>(IPAddress);
template std::unique_ptr<Orchestrator> factory_make_unique<Orchestrator, OutputStream&>(OutputStream&);

// GUI-specific 'unique' instances.
#ifdef __GUI__
#include "wx/wx.h"
#include "output_stream_gui.h"

template std::unique_ptr<std::ostream> factory_make_unique<std::ostream, wxTextCtrl*>(wxTextCtrl *);
template std::unique_ptr<OutputStreamGUI> factory_make_unique<OutputStreamGUI, std::ostream&>(std::ostream&);
#endif  // __GUI__


// Return class <T> object wrapped in shared_ptr.
// Throw std::string if allocation fails.
template<class T, class... Args> std::shared_ptr<T> factory_make_shared(Args... args)
{
    T *object_p;
    try {
        object_p = new T(args...);
    } catch (std::string& exception) {
        std::cerr << "ERROR: " + exception << ". Failed to allocate memory for application." << std::endl;
        throw exception;
    }

    return std::shared_ptr<T>(object_p);
}

// 'Shared' instances.
template std::shared_ptr<IPAddress> factory_make_shared<IPAddress, IPAddress>(IPAddress);