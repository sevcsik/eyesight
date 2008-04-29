#include "Eyesight.h"

int
main(int argc, char **argv)
{
   // gettext stuff
   setlocale(LC_ALL, "");
   bindtextdomain(PACKAGE, LOCALEDIR);
   textdomain(PACKAGE);

   // initialization

   if (!ecore_init()) goto shutdown;

   if (!ecore_evas_init()) goto ecore_shutdown;

   if (edje_init()) goto ecore_evas_shutdown;

   parse_args(argc, argv);
   main_window_create();

   ecore_main_loop_begin();
   
   // shutdown
   edje_shutdown();
   ecore_evas_shutdown();
   ecore_shutdown();
   
   return EXIT_SUCCESS;
   
   edje_shutdown:       edje_shutdown();
   ecore_evas_shutdown: ecore_evas_shutdown();
   ecore_shutdown:      ecore_shutdown();
   shutdown:            return EXIT_FAILURE;
}
