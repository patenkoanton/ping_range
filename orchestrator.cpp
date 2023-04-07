#include "orchestrator.h"
#include "factory.h"
#include "subnet.h"
#include "ping.h"

int Orchestrator::start()
{
    // Generate address range.
    auto subnet = factory_create_object<Subnet, std::string&>(this->address_and_mask);
    if (subnet == nullptr) {
        return -1;
    }

    // Create ping object.
    auto ping = factory_create_object<Ping>(subnet);
    if (ping == nullptr) {
        return -1;
    }

    // Show subnet info.
    std::cout << "Subnet: " << *subnet->subnet << std::endl;
    std::cout << "Broadcast: " << *subnet->broadcast << std::endl;
    std::cout << std::endl;

    // Perform ping.
    // TODO: ping->ping() should return a code.
    ping->ping();
    return 0;
}