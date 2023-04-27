#include <iostream>
#include <string>
#include "factory.h"
#include "ping.h"
#include "subnet.h"
#include "socket.h"
#include "ip_address.h"
#include "orchestrator.h"
#include "output_stream.h"


/* Return class <T> object wrapped in shared_ptr. */
template<class T, class... Args> std::shared_ptr<T> factory_create_object(Args... args)
{
    std::shared_ptr<T> object;
    try {
        object = std::shared_ptr<T>(new T(args...));
    } catch (std::string& exception) {
        std::cerr << "ERROR: " + exception << std::endl;
        return nullptr;
    }

    return object;
}

// Instance.
template std::shared_ptr<Ping> factory_create_object<Ping, OutputStream&>(OutputStream&);
template std::shared_ptr<Subnet> factory_create_object<Subnet, const std::string&, OutputStream&>(const std::string&, OutputStream&);
template std::shared_ptr<Socket> factory_create_object<Socket, OutputStream&>(OutputStream&);
template std::shared_ptr<IPAddress> factory_create_object<IPAddress, uint32_t>(uint32_t);
template std::shared_ptr<IPAddress> factory_create_object<IPAddress, const std::string&>(const std::string&);
template std::shared_ptr<Orchestrator> factory_create_object<Orchestrator, OutputStream&>(OutputStream&);
