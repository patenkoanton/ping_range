#include <iostream>
#include <set>
#include "helpers.h"


// Return false if argc is less than required minimum.
bool helpers_argc_is_incorrect(int argc, int min_argc)
{
    return argc < min_argc;
}

// Check if either --help or -help was passed.
bool helpers_help_message_requested(int argc, char **argv)
{
    std::set<std::string> args(argv, argv + argc);
    return args.count("--help") || args.count("-h");
}

// Show info message for user.
void helpers_show_help()
{
    std::cout << std::endl;
    std::cout << "Usage: sudo ./ping_subnet [subnet_ip_address]/[mask]" << std::endl;
}