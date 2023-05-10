#include "orchestrator.h"
#include "factory.h"
#include "subnet.h"


int Orchestrator::start(const std::string &address_and_mask)
{
    // Generate subnet (address range).
    auto subnet = Factory::make_unique<Subnet, const std::string&, OutputStream&>(address_and_mask, this->output_stream);

    // Show subnet info.
    this->output_stream << "Subnet: " << subnet->subnet->to_string() << std::endl;
    this->output_stream << "Broadcast: " << subnet->broadcast->to_string() << std::endl << std::endl;

    // Perform ping.
    // TODO: ping->ping() should return a code.
    this->ping->ping(std::move(subnet));
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