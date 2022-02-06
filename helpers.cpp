#include <iostream>
#include "helpers.h"


// Return false if argc is less than required minimum.
bool helpers_argc_is_incorrect(int argc, int min_argc)
{
    return argc < min_argc;
}

// Show info message for user.
void helpers_show_help()
{
    std::cout << "Usage: sudo ./ping_subnet [subnet_ip_address]/[mask]" << std::endl;
}