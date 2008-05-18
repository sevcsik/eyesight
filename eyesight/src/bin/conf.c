#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <Evas.h>

#include "error.h"
#include "conf.h"

Args *
parse_args(int argc, char **argv, Evas_List **startup_errors)
{
   int c;
   Args *args = malloc(sizeof(Args));

   // Shut up getopt
   opterr = 0;

   // Resetting args
   args->theme_path = NULL;

   // Check getopt switches
   while (c = getopt(argc, argv, OPTS))
   {
      if (c == -1)
         break;
      switch (c)
      {
         case 't':
            args->theme_path = optarg;
            break;
         case '?':
            append_startup_error(startup_errors, ERROR_ARG_UNKNOWN, (char *)&optopt);
            break;
      }
   }

   // TODO: conf.c: Optimize to use one loop during argument parsing

   for (c = 1; c < argc; c++)
   {
      if (argv[c][0] != '-') // Skip options
      {
         args->files = evas_list_append(args->files, argv[c]);
      }
   }

   return args;
}
