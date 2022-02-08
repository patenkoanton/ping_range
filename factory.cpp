#include <iostream>
#include <memory>
#include <string>
#include "factory.h"
#include "ping_subnet.h"
#include "address_range.h"
#include "icmp_socket.h"


/* Return class <T> object wrapped in unique_ptr. */
template<class T, class N> std::unique_ptr<T> factory_create_object(N arg)
{
    std::unique_ptr<T> object;
    try {
        object = std::unique_ptr<T>(new T(arg));
    } catch (std::string& exception) {
        std::cerr << "ERROR: " + exception << std::endl;
        return nullptr;
    }

    return object;
}

// Instance.
template std::unique_ptr<PingSubnet> factory_create_object<PingSubnet, char *>(char *);
template std::unique_ptr<AddressRange> factory_create_object<AddressRange, std::string&>(std::string&);
template std::unique_ptr<ICMPSocket> factory_create_object<ICMPSocket, int>(int);
