#include "Eyesight.h"

void
create_main_window()
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
   evas_object_show(main_window.main_window);
   ecore_evas_show(main_window.ee);
}
