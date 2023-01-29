#include <iostream>
#include <string>
#include <set>
#include "main.h"
#include "ping.h"
#include "factory.h"

#define MIN_ARGC    (2)


int main(int argc, char *argv[])
{
    if (Main::argc_is_incorrect(argc, MIN_ARGC)) {
        Main::show_help();
        return 0;
    }

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

    // Create ping object.
    auto ping = factory_create_object<Ping>(subnet);
    if (ping == nullptr) {
        Main::show_help();
        return 0;
    }

    // Show subnet info including the list of hosts.
    std::cout << "Subnet: " << subnet->subnet->to_string() << std::endl;
    std::cout << "Broadcast: " << subnet->broadcast->to_string() << std::endl;
    std::cout << "Hosts[DIAGNOSTIC INFO]: " << std::endl;
    for (auto it : subnet->hosts){
        std::cout << it->to_string() << std::endl;
    }

    std::cout << std::endl;

    // Perform ping.
    ping->ping();
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
