#include <iostream>
#include <string>
#include <arpa/inet.h>
#include "main.h"
#include "ping_subnet.h"

#define MIN_ARGC    (2)

bool argc_is_correct(int argc)
{
    return argc >= MIN_ARGC;
}

void show_help()
{
    std::cout << "Usage: ..." << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc_is_correct(argc) == false) {
        show_help();
        return 0;
    }

    PingSubnet *ping_subnet = NULL;
    try {
        ping_subnet = new PingSubnet(argv[1]);
    } catch (std::string& exception) {
        std::cout << "ERROR: " + exception << std::endl;
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

    // Cleanup
    delete ping_subnet;
    return 0;
}
