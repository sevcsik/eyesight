#include "Eyesight.h"

#define THEME_OPT "--theme="
void
parse_args(int argc, char **argv)
{
   // Resetting args
   args.theme_path = NULL;
   char *tmp;
   int n, i;
   for (i = 0; i < argc; i++)
   {
      if (!strcmp(argv[i], "-t"))
      {
         if (argv[++i])
            args.theme_path = argv[i];
      }
      else if (strstr(argv[i], THEME_OPT))
      {
         tmp = calloc(sizeof(char), strlen(argv[i])-strlen(THEME_OPT));
         strncpy(tmp, strstr(argv[i], "=")+1, strlen(argv[i])-strlen(THEME_OPT));
         args.theme_path = tmp;
      }
   }
}
