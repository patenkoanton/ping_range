#pragma once

#include <string>

struct Main {
    static bool argc_is_incorrect(int argc);
    static bool help_message_requested(int argc, char *argv[]);
    static void show_help();

    // Constants.
    static const int min_argc = 2;
};
