#ifndef _MAIN_H_
#define _MAIN_H_

#include <string>

struct Main {
    static bool argc_is_incorrect(int argc, int min_argc);
    static bool help_message_requested(int argc, char *argv[]);
    static void show_help();
};

#endif  // _MAIN_H_