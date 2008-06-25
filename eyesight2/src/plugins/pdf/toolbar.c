#include "toolbar.h"
#include <Evas.h>
#include <Edje.h>
#include <Efreet.h>
#include <stdio.h>

char *toolbar1_tooltips[] = {"Previous page", 0, 0, "Next page"};

void 
toolbar_icon_resize_cb(void *data, Evas *evas, Evas_Object *obj,
                       void *event_info)
{
   int w, h;
   evas_object_geometry_get(obj, NULL, NULL, &w, &h);
   evas_object_image_fill_set(obj, 0, 0, w, h);
}
      

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
   enum {ICON_TYPE_GROUP, ICON_TYPE_IMAGE} type;
   
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
   
   edje_object_file_get(controls, (const char **)&theme_file, NULL);
   
   // Load toolbar1_icon from theme
   
   icon_object = edje_object_add(evas);
   if (!edje_object_file_set(icon_object, theme_file, "eyesight/toolbar1_icon"))
   {
      fprintf(stderr, "Bad theme!, no eyesight/toolbar1_icon group!");
   }
   icon_theme = edje_file_data_get(theme_file, "icon_theme");
   icon_size = edje_file_data_get(theme_file, "icon_size");
   if (!icon_theme) 
      icon_theme = "Tango";
   if (!icon_size) 
      icon_size = "128x128";
      
   // Try to load image from edje file
   type = ICON_TYPE_GROUP;
   icon_image = edje_object_add(evas);
   if (!edje_object_file_set(icon_image, theme_file, icon_name))
   {
      // Failed to get icon from edje
      printf("ouadfsdf\n");
      evas_object_del(icon_image);
      type = ICON_TYPE_IMAGE;
      icon_image = evas_object_image_add(evas);
   }
   
   
   // If failed, try to load from icon set
   if (type == ICON_TYPE_IMAGE)
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
   
   edje_object_part_swallow(icon_object, "eyesight/toolbar1_icon/icon", icon_image);
   
   // Move and resize icon
   edje_object_part_geometry_get(controls, "eyesight/main_window/controls/toolbar1_sw",
                                 &x0, &y0, NULL, NULL);
   edje_object_size_min_get(icon_object, &w, &h);
   evas_object_move(icon_object, x0 + w * icon, y0);
   evas_object_resize(icon_object, w, h);
   evas_object_image_fill_set(icon_image, 0, 0, w, h);
   evas_object_layer_set(icon_object, 100);
   
   if (type == ICON_TYPE_IMAGE) // Set only if image is loded from file
      evas_object_event_callback_add(icon_image, EVAS_CALLBACK_RESIZE,
                                     toolbar_icon_resize_cb, NULL);
   
   // Set tooltip text
   edje_object_part_text_set(icon_object, "eyesight/toolbar1_icon/tooltip",
                             toolbar1_tooltips[icon]);
   
   evas_object_show(icon_image);
   evas_object_show(icon_object);
}
