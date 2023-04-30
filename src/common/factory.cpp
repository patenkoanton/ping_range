#include <iostream>
#include <string>
#include "factory.h"
#include "ping.h"
#include "subnet.h"
#include "socket.h"
#include "ip_address.h"
#include "orchestrator.h"
#include "output_stream.h"


/* Returns class <T> object wrapped in unique_ptr. */
template<class T, class... Args> std::unique_ptr<T> factory_make_unique(Args... args)
{
    T *object_p;
    try {
        object_p = new T(args...);
    } catch (std::string& exception) {
        std::cerr << "ERROR: " + exception << std::endl;
        return nullptr;
    }

    return std::unique_ptr<T>(object_p);        // gets moved (magic semantics of std::unique_ptr)
}

// Instance.
template std::unique_ptr<Ping> factory_make_unique<Ping, OutputStream&>(OutputStream&);
template std::unique_ptr<Subnet> factory_make_unique<Subnet, const std::string&, OutputStream&>(const std::string&, OutputStream&);
template std::unique_ptr<Socket> factory_make_unique<Socket, OutputStream&>(OutputStream&);
template std::unique_ptr<IPAddress> factory_make_unique<IPAddress, uint32_t>(uint32_t);
template std::unique_ptr<IPAddress> factory_make_unique<IPAddress, const std::string&>(const std::string&);
template std::unique_ptr<IPAddress> factory_make_unique<IPAddress, IPAddress>(IPAddress);
template std::unique_ptr<Orchestrator> factory_make_unique<Orchestrator, OutputStream&>(OutputStream&);
