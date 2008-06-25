#include "toolbar.h"
#include <Evas.h>
#include <Edje.h>
#include <Efreet.h>
#include <stdio.h>

char *toolbar1_tooltips[] = {"Previous page", 0, 0, "Next page"};

void
add_toolbar1_icon(Pdf_Toolbar1_Icon icon, Evas_Object *controls)
{
   Evas_Object *icon_object;
   Evas_Object *icon_image;
   Evas *evas = evas_object_evas_get(controls);
   char *icon_name;
   char *icon_path;
   char *icon_theme;
   char *icon_size;
   char *theme_file;
   int x0, y0, w, h;
   
   // Get icon filename
   switch (icon)
   {
   case PREV_PAGE:
      icon_name = "go-up";
      break;
   case NEXT_PAGE:
      icon_name = "go-down";
      break;
   default:
      icon_name = "dialog-error";
   }
   
   // TODO: toolbar.c: get theme and icon size from edje
   icon_theme = "oxygen";
   icon_size = "128x128";
      
   // Try to load image from edje file
   edje_object_file_get(controls, (const char **)&theme_file, NULL);
   icon_image = evas_object_image_add(evas);
   evas_object_image_file_set(icon_image, theme_file, icon_name);
   // If failed, try to load from icon set
   if (evas_object_image_load_error_get(icon_image) != EVAS_LOAD_ERROR_NONE)
   {
      icon_path = efreet_icon_path_find(icon_theme, icon_name, icon_size);
      if (!icon_path)
      {
         icon_path = efreet_icon_path_find(icon_theme, "dialog-error", icon_size);
      }
      evas_object_image_file_set(icon_image, icon_path, NULL);
   }
   if (evas_object_image_load_error_get(icon_image) != EVAS_LOAD_ERROR_NONE)
   {
      fprintf(stderr, "Failed to load icon %s\n", icon_name);  
   }

   // Load toolbar1_icon from theme
   icon_object = edje_object_add(evas);
   edje_object_file_set(icon_object, theme_file, "eyesight/toolbar1_icon");
   
   edje_object_part_swallow(icon_object, "eyesight/toolbar1_icon/icon", icon_image);
   
   // Move and resize icon
   edje_object_part_geometry_get(controls, "eyesight/main_window/controls/toolbar1_sw",
                                 &x0, &y0, NULL, NULL);
   edje_object_size_min_get(icon_object, &w, &h);
   evas_object_move(icon_object, x0 + w * icon, y0);
   evas_object_resize(icon_object, w, h);
   evas_object_image_fill_set(icon_image, 0, 0, w, h);
   evas_object_layer_set(icon_object, 100);
   
   // Set tooltip text
   edje_object_part_text_set(icon_object, "eyesight/toolbar1_icon/tooltip",
                             toolbar1_tooltips[icon]);
   
   evas_object_show(icon_image);
   evas_object_show(icon_object);
}
