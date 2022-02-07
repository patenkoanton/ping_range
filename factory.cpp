#include <iostream>
#include <memory>
#include "factory.h"
#include "ping_subnet.h"


// Return class <T> object wrapped in unique_ptr.
template<class T, class N> std::unique_ptr<T> factory_create_object(N arg)
{
    std::unique_ptr<T> object;
    try {
        object = std::unique_ptr<T>(new T(arg));
    } catch (std::string& exception) {
        std::cout << "ERROR: " + exception << std::endl;
        return nullptr;
    }

    return object;
}

// Instance.
template std::unique_ptr<PingSubnet> factory_create_object<PingSubnet, char *>(char *);
