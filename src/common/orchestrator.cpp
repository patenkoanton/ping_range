#include "orchestrator.h"
#include "subnet.h"
#include "custom_exception.h"
#include "factory.cpp"      // template module


Orchestrator::Orchestrator(OutputStream &output_stream, OutputStream &error_stream) :
    output_stream(output_stream),
    error_stream(error_stream)
{
    this->ping = Factory::make_unique<Ping, OutputStream&, OutputStream&>(output_stream, error_stream);
    this->running = false;
    this->cancelled = false;
};

void Orchestrator::start(const std::string &address_and_mask) noexcept
{
    this->cancelled = false;
    try {
        // Generate subnet (address range).
        auto subnet = Factory::make_unique<Subnet, const std::string&, OutputStream&, OutputStream&>(address_and_mask, this->output_stream, this->error_stream);
        this->output_stream << "Subnet: " << subnet->subnet->to_string() << std::endl;
        this->output_stream << "Broadcast: " << subnet->broadcast->to_string() << std::endl << std::endl;

        // Perform ping.
        this->running = true;
        this->ping->ping(std::move(subnet));
    } catch (const CustomException &exc) {
        this->error_stream << exc.what() << std::endl;
        this->cancelled = true;
    }

    this->running = false;
}

void Orchestrator::stop() noexcept
{
    this->running = false;
    this->ping->stop();
}


// Return 'ping' progress.
// Return 0 if 'ping' has been cancelled.
int Orchestrator::get_progress() noexcept
{
    // We shouldn't attempt to access 'ping' data after it's been cancelled.
    if (this->cancelled) {
        return 0;
    }
    return this->ping->get_progress();
}

bool Orchestrator::is_cancelled() noexcept
{
    return this->cancelled;
}

bool Orchestrator::is_running() noexcept
{
    return this->running;
}
