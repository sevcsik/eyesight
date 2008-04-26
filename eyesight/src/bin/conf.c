#include "Eyesight.h"

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
   
   // Check addational arguments (TODO)
   /*if (argc == optind) return;
   while (argc */
}
