#include "orchestrator.h"
#include "factory.h"
#include "subnet.h"
#include "ping.h"

int Orchestrator::start()
{
    // Generate address range.
    auto subnet = factory_create_object<Subnet, std::string&, OutputStreamBase&>(this->address_and_mask, this->output_stream);
    if (subnet == nullptr) {
        return -1;
    }

    // Create ping object.
    auto ping = factory_create_object<Ping, std::shared_ptr<Subnet>, OutputStreamBase&>(subnet, this->output_stream);
    if (ping == nullptr) {
        return -1;
    }

    // Show subnet info.
    this->output_stream << "Subnet: " << subnet->subnet->to_string() << std::endl;
    this->output_stream << "Broadcast: " << subnet->broadcast->to_string() << std::endl << std::endl;

    // Perform ping.
    // TODO: ping->ping() should return a code.
    ping->ping();
    return 0;
}