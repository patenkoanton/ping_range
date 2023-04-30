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
template<class T, class... Args> std::unique_ptr<T> factory_create_object(Args... args)
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
template std::unique_ptr<Ping> factory_create_object<Ping, OutputStream&>(OutputStream&);
template std::unique_ptr<Subnet> factory_create_object<Subnet, const std::string&, OutputStream&>(const std::string&, OutputStream&);
template std::unique_ptr<Socket> factory_create_object<Socket, OutputStream&>(OutputStream&);
template std::unique_ptr<IPAddress> factory_create_object<IPAddress, uint32_t>(uint32_t);
template std::unique_ptr<IPAddress> factory_create_object<IPAddress, const std::string&>(const std::string&);
template std::unique_ptr<IPAddress> factory_create_object<IPAddress, IPAddress>(IPAddress);
template std::unique_ptr<Orchestrator> factory_create_object<Orchestrator, OutputStream&>(OutputStream&);
