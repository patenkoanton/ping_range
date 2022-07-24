#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <set>
#include "main.h"
#include "ping.h"
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
    auto subnet = factory_create_object<Subnet, std::string&>(address_and_mask);
    if (subnet == nullptr) {
        std::cerr << "ERROR: failed to create Subnet object." << std::endl;
        Main::show_help();
        return 0;
    }

    // Create "pinger" object.
    auto ping_subnet = factory_create_object<PingSubnet>(subnet);
    if (ping_subnet == nullptr) {
        Main::show_help();
        return 0;
    }

    // Show subnet info including the list of hosts.
    std::cout << "Subnet: " << Main::network_order_to_host_order(subnet->subnet) << std::endl;
    std::cout << "Broadcast: " << Main::network_order_to_host_order(subnet->broadcast) << std::endl;
    std::cout << "Hosts: " << std::endl;
    auto hosts = subnet->hosts;
    for (auto it = subnet->hosts.begin(); it != subnet->hosts.end(); it++) {
        std::cout << Main::network_order_to_host_order(*it) << std::endl;
    }
    std::cout << std::endl;

    // Ping subnet.
    ping_subnet->ping();
    return 0;
}

std::string Main::network_order_to_host_order(uint32_t address_network_order)
{
    in_addr host_address = {
        .s_addr = address_network_order,
    };

    return std::string(inet_ntoa(host_address));
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
