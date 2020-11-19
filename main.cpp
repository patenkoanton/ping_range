#include <iostream>
#include <string>
#include "ping_range.h"

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

    PingRange range_ping;
    if (range_ping.initialize(argv[1]) == false) {
        show_help();
        return 0;
    }

    return 0;
}
