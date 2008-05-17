#include <stdio.h>
#include <libintl.h>
#include <stdlib.h>
#include <string.h>

#include <Ecore_Evas.h>
#include <Evas.h>
#include <Edje.h>

#include "config.h"
#include "eyesight_private.h"

#include "conf.h"
#include "error.h"
#include "main_window.h"

void
main_window_resize_cb(Ecore_Evas *ee)
{
   int w, h;
   Main_Window *main_window = (Main_Window*)ecore_evas_data_get(ee, "main_window");
   ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
   evas_object_resize(main_window->main_window, w, h);
}

void
main_window_destroy_cb(Ecore_Evas *ee)
{
   /* Free */
   Main_Window *main_window = (Main_Window*)ecore_evas_data_get(ee, "main_window");
   evas_object_free(main_window->main_window);
   ecore_evas_free(ee);
   free(main_window);
   ecore_main_loop_quit();
}

void
main_window_create(Args *args, Evas_List **startup_errors)
{
   Main_Window *main_window = malloc(sizeof(Main_Window));
   Ecore_Evas *ee;
   char *errstr;

   ee = ecore_evas_software_x11_new(0, 0, 0, 0, 0, 0);
   main_window->evas = ecore_evas_get(ee);
   main_window->main_window = edje_object_add(main_window->evas);

   ecore_evas_data_set(ee, "main_window", (void *)main_window);
   ecore_evas_data_set(ee, "args", (void *)args);

   // Try to open theme supplied in arguments
   char theme_loaded = 0;
   if (args->theme_path)
   {
      if (!edje_object_file_set(main_window->main_window, args->theme_path,
                                "main_window"))
      {
         append_startup_error(startup_errors, ERROR_THEME, args->theme_path);
      }
      else
         theme_loaded = 1;
   }
   if (!theme_loaded)
      edje_object_file_set(main_window->main_window,
                           PACKAGE_DATA_DIR"/themes/docker/docker.edj",
                           "main_window");
   int w, h;
   evas_object_name_set(main_window->main_window, "main_window");
   edje_object_size_min_get(main_window->main_window, &w, &h);
   evas_object_move(main_window->main_window, 0, 0);
   evas_object_resize(main_window->main_window, w, h);
   ecore_evas_resize(ee, w, h);
   ecore_evas_size_min_set(ee, w, h);

   /* VM stuff */
   ecore_evas_title_set(ee, WM_TITLE);
   ecore_evas_name_class_set(ee, WM_NAME, WM_CLASS);

   /* Callbacks */
   ecore_evas_callback_resize_set(ee, main_window_resize_cb);
   ecore_evas_callback_destroy_set(ee, main_window_destroy_cb);
   edje_object_signal_callback_add(main_window->main_window, "load", "*",
                                   main_window_load_cb, startup_errors);
   
   evas_object_show(main_window->main_window);
   ecore_evas_show(ee);
   
   display_startup_error_dialog(ee, *startup_errors);
}

void
main_window_load_cb(void *data, Evas_Object *o, const char *emission, 
                    const char *source)
{
   printf("foo");
     if (!data) return; // don't do anything if there's no errors
     edje_object_signal_emit(o, "error_show", "eyesight");
}
