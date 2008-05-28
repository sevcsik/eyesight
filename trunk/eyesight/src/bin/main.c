#include <libintl.h>
#include <stdlib.h>
#include <stdio.h>

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Edje.h>

#include "conf.h"
#include "plugin.h"
#include "config.h"

#define _(STRING) gettext(STRING)

int
main(int argc, char **argv)
{
   Args *args = NULL;
   char *errstr = NULL;
   Evas_List *startup_errors = NULL;
   Plist_Data *plist_data; // Will contain plugin list and plugin path group

   // Gettext stuff
   setlocale(LC_ALL, "");
   bindtextdomain(PACKAGE, LOCALEDIR);
   textdomain(PACKAGE);
	
   // Initialization

   if (!ecore_init())
      goto _shutdown;

   if (!ecore_evas_init())
      goto _ecore_shutdown;

   if (!edje_init())
      goto _ecore_evas_shutdown;

   args = parse_args(argc, argv, &startup_errors);

   plist_data = plugin_init(&startup_errors);

   main_window_create(args, &startup_errors);
   
   int i;
   
   if (startup_errors)
   {
      fprintf(stderr, _("During startup, the following errors occured:\n"));
      for (i = 0;; i++)
      {
         errstr = evas_list_nth(startup_errors, i);
         if (!errstr) break;
         else fprintf(stderr, "  %s\n", errstr);
      }
   }
   
   evas_list_free(startup_errors);

   ecore_main_loop_begin();

   // shutdown
   edje_shutdown();
   ecore_evas_shutdown();
   ecore_shutdown();

   return EXIT_SUCCESS;

_edje_shutdown:
   edje_shutdown();
   
_ecore_evas_shutdown:
   ecore_evas_shutdown();
   
_ecore_shutdown:
   ecore_shutdown();
   
_shutdown:
   return EXIT_FAILURE;
}
