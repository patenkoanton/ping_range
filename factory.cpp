#include <iostream>
#include <memory>
#include <string>
#include "factory.h"
#include "ping.h"
#include "subnet.h"
#include "socket.h"


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
template std::shared_ptr<Ping> factory_create_object<Ping, std::shared_ptr<Subnet>>(std::shared_ptr<Subnet>);
template std::shared_ptr<Subnet> factory_create_object<Subnet, std::string&>(std::string&);
template std::shared_ptr<Socket> factory_create_object<Socket, int>(int);
