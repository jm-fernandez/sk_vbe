#ifndef SK_COMMAND_LINE_H_
#define SK_COMMAND_LINE_H_

#include "sk_error.h"

typedef enum command_t_
{
  help,
  install,
  uninstall,
} command_t;

void sk_show_help(const char* app_name);
sk_error_t sk_parse_command_line(int argc, char* argv[], command_t* command);

#endif // SK_COMMAND_LINE_H_
