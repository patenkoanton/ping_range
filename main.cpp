#include <iostream>
#include <string>
#include <arpa/inet.h>
#include "main.h"
#include "ping_subnet.h"
#include "helpers.h"
#include "factory.h"

#define MIN_ARGC    (2)


int main(int argc, char *argv[])
{
    // Not enough args provided - show help and exit.
    if (helpers_argc_is_incorrect(argc, MIN_ARGC)) {
        helpers_show_help();
        return 0;
    }

    // Help requested - show help and exit.
    if (helpers_help_message_requested(argc, argv)) {
        helpers_show_help();
        return 0;
    }

    // Create "pinger" object.
    auto ping_subnet = factory_create_object<PingSubnet>(argv[1]);
    if (ping_subnet == nullptr) {
        helpers_show_help();
        return 0;
    }

    // Print the range of addresses.
    auto address_range = ping_subnet->get_address_range();
    std::cout << "Range of IP addresses generated: " << std::endl;
    for (auto it = address_range.begin(); it != address_range.end(); it++) {
        in_addr host_address = {
            .s_addr = *it,
        };
        std::cout << inet_ntoa(host_address) << std::endl;
    }
    std::cout << std::endl;

    // Ping subnet.
    ping_subnet->ping();
    return 0;
}
