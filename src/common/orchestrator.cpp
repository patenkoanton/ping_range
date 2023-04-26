#include "orchestrator.h"
#include "factory.h"
#include "subnet.h"
#include "ping.h"

Orchestrator::Orchestrator(OutputStream &stream) : output_stream(stream), ping(factory_create_object<Ping, OutputStream&>(stream))
{
    if (this->ping == nullptr) {
        throw std::string("failed to initialize Ping.");
    }
}


int Orchestrator::start(const std::string &address_and_mask)
{
    // Generate subnet (address range).
    const auto subnet = factory_create_object<Subnet, const std::string&, OutputStream&>(address_and_mask, this->output_stream);
    if (subnet == nullptr) {
        return -1;
    }

    // Show subnet info.
    this->output_stream << "Subnet: " << subnet->subnet->to_string() << std::endl;
    this->output_stream << "Broadcast: " << subnet->broadcast->to_string() << std::endl << std::endl;

    // Perform ping.
    // TODO: ping->ping() should return a code.
    this->ping->ping(subnet);
    return 0;
}

void Orchestrator::stop()
{
    this->ping->stop();
}


int Orchestrator::get_progress()
{
    return this->ping->get_progress();
}