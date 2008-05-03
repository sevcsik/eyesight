#include <stdio.h>
#include <unistd.h>
#include <Evas.h>

#include "conf.h"

void
parse_args(int argc, char **argv)
{
   int c;
   
   // Resetting args
   args.theme_path = NULL;
   
   // Check getopt switches
   while (c = getopt(argc, argv, OPTS))
   {
      if (c == -1) break; 
      switch (c)
      {
         case 't':
            args.theme_path = optarg;
            continue;
      }
   }
   
   // Check addational arguments (filenames)
   
   for (c = 1; c < argc; c++)
   {
      if (argv[c][0] != '-') // Skip options
      {
         printf("Appended: %d: %s\n", c, argv[c]);
         args.files = evas_list_append(args.files, argv[c]);  
      }   
   }
} 
