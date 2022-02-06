#ifndef _HELPERS_H_
#define _HELPERS_H_

bool helpers_argc_is_incorrect(int argc, int min_argc);
bool helpers_help_message_requested(int argc, char *argv[]);
void helpers_show_help();

#endif  // _HELPERS_H_