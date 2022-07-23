#ifndef _MAIN_H_
#define _MAIN_H_

#include <string>

struct Main {
    static bool argc_is_incorrect(int argc, int min_argc);
    static bool help_message_requested(int argc, char *argv[]);
    static void show_help();
    static std::string network_order_to_host_order(uint32_t);
};

#endif  // _MAIN_H_