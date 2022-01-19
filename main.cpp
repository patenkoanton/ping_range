#include <iostream>
#include <string>

// user headers
#include "ping_range.h"

#define MIN_ARGC    (2)

// TODO: change "ping range" to "ping subnet".
// TODO: take address range out of ping range and let main handle the calls.

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

    // TODO: catch exception and show help in case of error.
    PingRange *range_ping = new PingRange(argv[1]);

    // Print the range of addresses.
    auto address_range = range_ping->get_address_range();
    std::cout << "Range of IP addresses generated: " << std::endl;
    for (auto it = address_range.begin(); it != address_range.end(); it++) {
        std::cout << *it << std::endl;
    }
    std::cout << std::endl;

    // Ping subnet.
    range_ping->ping();
    return 0;
}
