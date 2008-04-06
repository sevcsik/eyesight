#include "Eyesight.h"

void
main_window_resize_cb(Ecore_Evas *ee)
{
   int w, h;
   ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
   evas_object_resize(main_window.main_window, w, h);
}

void
main_window_destroy_cb(Ecore_Evas *ee)
{
   evas_object_free(main_window.main_window);
   ecore_evas_free(main_window.ee);
   exit(EXIT_SUCCESS);
}

void
main_window_create()
{
   main_window.ee = ecore_evas_software_x11_new(0, 0, 0, 0, 0, 0);
   main_window.evas = ecore_evas_get(main_window.ee);
   main_window.main_window = edje_object_add(main_window.evas);
   
   // Try to open theme supplied in arguments
   char theme_loaded = 0;
   if (args.theme_path)
   {
      if (!edje_object_file_set(main_window.main_window, args.theme_path,
                         "main_window"))
      {
         printf(ERROR_THEME, args.theme_path);                     
      }
      else theme_loaded = 1;
   }
   if (!theme_loaded)
      edje_object_file_set(main_window.main_window,
                           PACKAGE_DATA_DIR"/themes/docker/docker.edj",
                           "main_window");
   int w, h;
   edje_object_size_min_get(main_window.main_window, &w, &h);
   evas_object_move(main_window.main_window, 0, 0);
   evas_object_resize(main_window.main_window, w, h);
   ecore_evas_resize(main_window.ee, w, h);
   
   /* Callbacks */
   ecore_evas_callback_resize_set(main_window.ee, main_window_resize_cb);
   ecore_evas_callback_destroy_set(main_window.ee, main_window_destroy_cb);
   
   evas_object_show(main_window.main_window);
   ecore_evas_show(main_window.ee);
}
