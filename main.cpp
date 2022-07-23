#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <set>
#include "main.h"
#include "ping_subnet.h"
#include "factory.h"

#define MIN_ARGC    (2)


int main(int argc, char *argv[])
{
    // Not enough args provided - show help and exit.
    if (Main::argc_is_incorrect(argc, MIN_ARGC)) {
        Main::show_help();
        return 0;
    }

    // Help requested - show help and exit.
    if (Main::help_message_requested(argc, argv)) {
        Main::show_help();
        return 0;
    }

    // Generate address range.
    std::string address_and_mask = argv[1];
    auto address_range = factory_create_object<AddressRange, std::string&>(address_and_mask);
    if (address_range == nullptr) {
        std::cerr << "ERROR: failed to create AddressRange object." << std::endl;
        Main::show_help();
        return 0;
    }

    // Create "pinger" object.
    auto ping_subnet = factory_create_object<PingSubnet>(address_range);
    if (ping_subnet == nullptr) {
        Main::show_help();
        return 0;
    }

    // Print the range of addresses.
    auto hosts = address_range->get_address_range();
    std::cout << "Range of IP addresses generated: " << std::endl;
    for (auto it = hosts.begin(); it != hosts.end(); it++) {
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


bool Main::argc_is_incorrect(int argc, int min_argc)
{
    return argc < min_argc;
}


/* Check if either --help or -help was passed. */
bool Main::help_message_requested(int argc, char **argv)
{
    std::set<std::string> args(argv, argv + argc);
    return args.count("--help") || args.count("-h");
}


/* Show info message for user. */
void Main::show_help()
{
    std::cout << std::endl;
    std::cout << "Usage: sudo ./ping_subnet [subnet_ip_address]/[mask]" << std::endl;
}
