#include "Eyesight.h"

int
main(int argc, char **argv)
{
   // gettext stuff
   setlocale(LC_ALL, "");
   bindtextdomain(PACKAGE, LOCALEDIR);
   textdomain(PACKAGE);

   // initialization

   if (!ecore_init()) goto _shutdown;

   if (!ecore_evas_init()) goto _ecore_shutdown;

   if (!edje_init()) goto _ecore_evas_shutdown;

   parse_args(argc, argv);
   main_window_create();

   ecore_main_loop_begin();
   
   // shutdown
   edje_shutdown();
   ecore_evas_shutdown();
   ecore_shutdown();
   
   return EXIT_SUCCESS;
   
   _edje_shutdown:       edje_shutdown();
   _ecore_evas_shutdown: ecore_evas_shutdown();
   _ecore_shutdown:      ecore_shutdown();
   _shutdown:            return EXIT_FAILURE;
}
