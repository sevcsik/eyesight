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
         errstr = malloc(strlen(ERROR_ARG_UNKNOWN)); // Don't need +1 because of %;
         snprintf(errstr, strlen(ERROR_ARG_UNKNOWN), ERROR_ARG_UNKNOWN, (char *)&optopt);
         startup_errors = evas_list_append(*startup_errors, errstr);
         break;
      /*case ':': // TODO: 
         errstr = malloc(strlen(ERROR_ARG_NOARG)); // Don't need +1 because of %;
         snprintf(errstr, strlen(ERROR_ARG_NOARG), ERROR_ARG_NOARG, (char *)&optopt);
         startup_errors = evas_list_append(startup_errors, errstr);
         break;*/
      }
   }

   // Check addational arguments (filenames) TODO: do this in one loop

   for (c = 1; c < argc; c++)
   {
      if (argv[c][0] != '-') // Skip options
      {
         printf("Appended: %d: %s\n", c, argv[c]);
         args->files = evas_list_append(args->files, argv[c]);
      }
   }
   
   return args;
}
