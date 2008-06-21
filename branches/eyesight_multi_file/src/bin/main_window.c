#include <stdio.h>
#include <libintl.h>
#include <stdlib.h>
#include <string.h>

#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Ecore_Data.h>
#include <Evas.h>
#include <Edje.h>

#include "config.h"
#include "eyesight_private.h"

#include "conf.h"
#include "error.h"
#include "main_window.h"
#include "plugin.h"
#include "file.h"

typedef struct _Nav_Cb_Data
{
   Ecore_List *plugins;
   Ecore_List *files;
} Nav_Cb_Data;

void
main_window_resize_cb(Ecore_Evas *ee)
{
   int w, h;
   Main_Window *main_window = (Main_Window*)ecore_evas_data_get(ee, "main_window");
   ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
   evas_object_resize(main_window->main_window_bg, w, h);
   evas_object_resize(main_window->main_window_fg, w, h);
}

void
main_window_delete_request_cb(Ecore_Evas *ee)
{
   /* Free */
   Main_Window *main_window = (Main_Window *)ecore_evas_data_get(ee, "main_window");
   Args *args = (Args *)ecore_evas_data_get(ee, "args");
   free(args->theme_path);
   if (args->files) ecore_list_destroy(args->files);
   evas_object_free(main_window->main_window_bg);
   evas_object_free(main_window->main_window_fg);
   evas_free(main_window->evas);
   free(main_window);
   ecore_evas_free(ee);
   ecore_main_loop_quit();
}

Main_Window *
main_window_create(Args *args, Evas_List **startup_errors, Ecore_List *plugins)
{
   Main_Window *main_window = malloc(sizeof(Main_Window));
   Ecore_Evas *ee;
   int w, h;
   char *theme_path;
   char theme_loaded = 0;
   Nav_Cb_Data *nav_cb_data;

   ee = ecore_evas_software_x11_new(0, 0, 0, 0, 0, 0);
   main_window->evas = ecore_evas_get(ee);
   main_window->main_window_bg = edje_object_add(main_window->evas);
   main_window->main_window_fg = edje_object_add(main_window->evas);

   ecore_evas_data_set(ee, "main_window", (void *)main_window);
   ecore_evas_data_set(ee, "args", (void *)args);

   // Try to open theme supplied in arguments (background group)
   if (args->theme_path)
   {
      if (!edje_object_file_set(main_window->main_window_bg, args->theme_path,
                                "eyesight/main_window/background"))
      {
         append_startup_error(startup_errors, ERROR_THEME, args->theme_path);
      }
      else
         theme_loaded = 1;
   }
   if (!theme_loaded) // Load default theme
      edje_object_file_set(main_window->main_window_bg,
                           PACKAGE_DATA_DIR"/themes/docker/docker.edj",
                           "eyesight/main_window/background");
   
   evas_object_name_set(main_window->main_window_bg, "main_window_bg");
   edje_object_size_min_get(main_window->main_window_bg, &w, &h);
   evas_object_move(main_window->main_window_bg, 0, 0);
   evas_object_resize(main_window->main_window_bg, w, h);
   evas_object_layer_set(main_window->main_window_bg, 0);
   ecore_evas_resize(ee, w, h);
   ecore_evas_size_min_set(ee, w, h);
   
   // Load foreground group
   edje_object_file_get(main_window->main_window_bg, (const char **)&theme_path, NULL);
   edje_object_file_set(main_window->main_window_fg, (const char *)theme_path, 
                        "eyesight/main_window/foreground");
   evas_object_name_set(main_window->main_window_fg, "main_window_fg");
   // Layers 1-10 are reserved for plugins
   evas_object_layer_set(main_window->main_window_fg, 11);
   evas_object_move(main_window->main_window_fg, 0, 0);
   evas_object_resize(main_window->main_window_fg, w, h);

   /* VM stuff */
   ecore_evas_title_set(ee, WM_TITLE);
   ecore_evas_name_class_set(ee, WM_NAME, WM_CLASS);

   /* Callbacks */
   ecore_evas_callback_resize_set(ee, main_window_resize_cb);
   ecore_evas_callback_delete_request_set(ee, main_window_delete_request_cb);
   edje_object_signal_callback_add(main_window->main_window_bg, "load", "*",
                                   main_window_load_cb, startup_errors);
   nav_cb_data = malloc(sizeof(Nav_Cb_Data));
   nav_cb_data->files = args->files;
   nav_cb_data->plugins = plugins;
   edje_object_signal_callback_add(main_window->main_window_fg, 
                                   "nav_next_clicked", "*",
                                   main_window_nav_next_cb, nav_cb_data); 
   
   ecore_animator_frametime_set(FRAMETIME);
   evas_object_show(main_window->main_window_bg);
   evas_object_show(main_window->main_window_fg);
   ecore_evas_show(ee);
   
   display_startup_error_dialog(ee, *startup_errors);
   
   //free(theme_path);
   return main_window;
}

void
main_window_load_cb(void *data, Evas_Object *o, const char *emission, 
                    const char *source)
{
     if (!evas_list_data(data)) return; // don't do anything if there's no errors
     edje_object_signal_emit(o, "error_show", "eyesight");
}

void
main_window_nav_next_cb(void *_data, Evas_Object *o, const char *emission,
                        const char *source)
{
   Ecore_Evas *ee = ecore_evas_ecore_evas_get(evas_object_evas_get(o));
   Evas *evas = evas_object_evas_get(o);
   Ecore_List *files = ((Nav_Cb_Data *)_data)->files;
   Ecore_List *plugins = ((Nav_Cb_Data *)_data)->plugins;
   char *current_file = ecore_evas_data_get(ee, "current_file");
   char *next_file = ecore_evas_data_get(ee, "next_file");
   char *current_prev_file = ecore_evas_data_get(ee, "prev_file");
   Plugin *current_plugin = ecore_evas_data_get(ee, "current_plugin");
   Plugin *next_plugin = ecore_evas_data_get(ee, "next_plugin");
   Plugin *prev_plugin = ecore_evas_data_get(ee, "prev_plugin");
   Next_Readable_File_Open_Data *idler_data;
   
   if (!current_file) return;
   
   // Hide current file
   current_file_hide(current_plugin, evas, o);
   
   // Show next file
   next_file_show(next_plugin, evas, o);
   
   // Free previous file
   prev_file_close(evas, prev_plugin);
   
   // Cache the next readable file in list in the background
   idler_data = malloc(sizeof(Next_Readable_File_Open_Data));
   idler_data->evas = evas;
   idler_data->main_window_fg = o;
   idler_data->plugins = plugins;
   idler_data->files = files;
   ecore_idle_enterer_add(next_readable_file_open, idler_data);
}
