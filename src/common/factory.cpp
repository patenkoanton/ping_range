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
// Throw if allocation fails.
template<class T, class... Args> std::unique_ptr<T> Factory::make_unique(Args... args)
{
    try {
        return std::make_unique<T>(args...);
    } catch (std::bad_alloc &exc) {
        std::cerr << "ERROR: failed to allocate memory for application. " << exc.what() << std::endl;
        throw exc;
    } catch (std::string &exc) {
        std::cerr << "ERROR: internal error. " << exc << std::endl;
        throw exc;
    }
}

// 'Unique' instances.
template std::unique_ptr<Ping> Factory::make_unique<Ping, OutputStream&>(OutputStream&);
template std::unique_ptr<Subnet> Factory::make_unique<Subnet, const std::string&, OutputStream&>(const std::string&, OutputStream&);
template std::unique_ptr<Socket> Factory::make_unique<Socket, OutputStream&>(OutputStream&);
template std::unique_ptr<IPAddress> Factory::make_unique<IPAddress, uint32_t>(uint32_t);
template std::unique_ptr<IPAddress> Factory::make_unique<IPAddress, const std::string&>(const std::string&);
template std::unique_ptr<IPAddress> Factory::make_unique<IPAddress, IPAddress>(IPAddress);
template std::unique_ptr<Orchestrator> Factory::make_unique<Orchestrator, OutputStream&>(OutputStream&);

// GUI-specific 'unique' instances.
#ifdef __GUI__
#include "wx/wx.h"
#include "output_stream_gui.h"

template std::unique_ptr<std::ostream> Factory::make_unique<std::ostream, wxTextCtrl*>(wxTextCtrl *);
template std::unique_ptr<OutputStreamGUI> Factory::make_unique<OutputStreamGUI, std::ostream&>(std::ostream&);
#endif  // __GUI__


// Return class <T> object wrapped in shared_ptr.
// Throw if allocation fails.
template<class T, class... Args> std::shared_ptr<T> Factory::make_shared(Args... args)
{
    try {
        return std::make_shared<T>(args...);
    } catch (std::bad_alloc &exc) {
        std::cerr << "ERROR: failed to allocate memory for application. " << exc.what() << std::endl;
        throw exc;
    } catch (std::string &exc) {
        std::cerr << "ERROR: internal error. " << exc << std::endl;
        throw exc;
    }

}

// 'Shared' instances.
template std::shared_ptr<IPAddress> Factory::make_shared<IPAddress, IPAddress>(IPAddress);