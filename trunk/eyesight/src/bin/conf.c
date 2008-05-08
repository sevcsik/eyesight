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
   char *errstr;

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
            break;
            append_startup_error(startup_errors, ERROR_ARG_UNKNOWN, (char *)&optopt);
      }
   }

   // Check addational arguments (filenames) TODO: do this in one loop

   for (c = 1; c < argc; c++)
   {
      if (argv[c][0] != '-') // Skip options
      {
         args->files = evas_list_append(args->files, argv[c]);
      }
   }

   return args;
}
