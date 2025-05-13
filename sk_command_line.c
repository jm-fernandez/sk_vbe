#include <string.h>
#include <stdio.h>

#include "sk_command_line.h"

void sk_show_help(const char* app_name)
{
  printf("Syntaxis: %s [/OPCION | -OPCION]\n", app_name);
  printf("\n");
  printf("Donde OPCION es una de las siguientes opciones:\n");
  printf("\tINSTALAR o I: Instala la aplicacion en memoria\n");
  printf("\tDESINSTALAR o D: Elimina la aplicacion de memoria\n");
  printf("\tAYUDA, A o ?: Muestra este menu de ayuda\n");
}


sk_error_t sk_parse_command_line(int argc, char* argv[], command_t* command)
{
  sk_error_t result = sk_error_command_line;
  if(argc == 1)
  {
    result = sk_success;
    *command = help;
  }
  else if((argc == 2) && (argv[1][0] == '/' || argv[1][0] == '-'))
  {
    if(!stricmp(&argv[1][1], "instalar")
    || !stricmp(&argv[1][1], "i"))
    {
      result = sk_success;
      *command = install;
    }
    else if(!stricmp(&argv[1][1], "desinstalar")
    || !stricmp(&argv[1][1], "d"))
    {
      result = sk_success;
      *command = uninstall;
    }
    else if(!stricmp(&argv[1][1], "ayuda")
    || !stricmp(&argv[1][1], "a")
    || !stricmp(&argv[1][1], "?"))
    {
      result = sk_success;
      *command = help;
    }
  }
  return result;
}
