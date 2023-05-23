#include "orchestrator.h"
#include "factory.h"
#include "subnet.h"
#include "custom_exception.h"


// Prepares and runs 'ping'.
// Keeps all the exceptions contained.
// Returns 0 for success, negative for error.
int Orchestrator::start(const std::string &address_and_mask) noexcept
{
    try {
        // Generate subnet (address range).
        auto subnet = Factory::make_unique<Subnet, const std::string&, OutputStream&>(address_and_mask, this->output_stream);
        this->output_stream << "Subnet: " << subnet->subnet->to_string() << std::endl;
        this->output_stream << "Broadcast: " << subnet->broadcast->to_string() << std::endl << std::endl;

        // Perform ping.
        this->ping = Factory::make_unique<Ping, OutputStream&>(this->output_stream);
        this->ping->ping(std::move(subnet));
    } catch (const CustomException &exc) {
        std::cerr << exc.what() << std::endl;
        return -1;
    }

    return 0;
}

void Orchestrator::stop() noexcept
{
    this->ping->stop();
}


int Orchestrator::get_progress() noexcept
{
    return this->ping->get_progress();
}