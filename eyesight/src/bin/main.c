#include "Eyesight.h"

int
main(int argc, char **argv)
{
   // gettext stuff
   setlocale(LC_ALL, "");
   bindtextdomain(PACKAGE, LOCALEDIR);
   textdomain(PACKAGE);

   if (!init_libs(argc, argv))
      exit(-1);

   parse_args(argc, argv);
   main_window_create();

   ecore_main_loop_begin();
}
