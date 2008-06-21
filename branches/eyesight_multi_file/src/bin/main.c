#include <libintl.h>
#include <stdlib.h>
#include <stdio.h>

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Edje.h>

#include "conf.h"
#include "plugin.h"
#include "plugin_private.h"
#include "main_window.h"
#include "config.h"
#include "file.h"

#define _(STRING) gettext(STRING)

int
main(int argc, char **argv)
{
   Args *args = NULL;
   char *errstr = NULL;
   Evas_List *startup_errors = NULL;
   Plist_Data *plist_data; // Will contain plugin list and plugin path group
   Main_Window *main_window;
   Next_Readable_File_Open_Data *nrfo_data;

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

   // Parse arguments and put in args structure
   args = parse_args(argc, argv, &startup_errors);

   // Identify and init plugins
   plist_data = plugin_init(&startup_errors);

   // Set up GUI
   main_window = main_window_create(args, &startup_errors, plist_data->plugin_list);
   
   if (args->files)
   {
      // Open and show first file
      ecore_list_goto(args->files, ecore_list_first(args->files));
      nrfo_data = malloc(sizeof(Next_Readable_File_Open_Data));
      nrfo_data->evas = main_window->evas;
      nrfo_data->main_window_fg = main_window->main_window_fg;
      nrfo_data->plugins = plist_data->plugin_list;
      nrfo_data->files = args->files;
      args->files->current = args->files->first;
      if (next_readable_file_open(nrfo_data))
      {
         next_file_show(nrfo_data->plugin, main_window->evas, main_window->main_window_bg);
      }
   }
   
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
