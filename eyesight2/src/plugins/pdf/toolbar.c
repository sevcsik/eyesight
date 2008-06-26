#include "toolbar.h"
#include "pdf.h"
#include "stdlib.h"
#include <Evas.h>
#include <Ecore.h>
#include <Edje.h>
#include <Efreet.h>
#include <esmart_pdf.h>
#include <stdio.h>

typedef void (*Edje_Signal_Callback) (void *data, Evas_Object *o, 
                                      const char *emission, const char *source);

char *toolbar1_tooltips[] = {"Previous page", 0, 0, "Next page"};

Edje_Signal_Callback toolbar1_callbacks[] = {
                                               page_prev_clicked,
                                               0,
                                               0,
                                               page_next_clicked
                                            };
                                            
typedef struct _Pdf_Nav_Animator_Data
{
   Evas_Object *controls;
   Evas_Object *border;
   Evas_Object *tmp_border;
   int step;
   int top_margin;
   int bottom_margin;
} Pdf_Nav_Animator_Data;

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
   
   // Set callback
   edje_object_signal_callback_add(icon_object, "clicked", "toolbar1_icon",
                                   toolbar1_callbacks[icon], NULL);
   
   evas_object_show(icon_image);
   evas_object_show(icon_object);
}



void page_next_clicked(void *_data, Evas_Object *icon, const char *emission,
                       const char *source)
{
   Evas_Object *controls = evas_object_name_find(evas_object_evas_get(icon), 
                                                 "controls");
   int ww, wh; // window size
   int nw, nh; // page native size
   int top_margin;
   int bottom_margin;
   Pdf_Nav_Animator_Data *animdata = malloc(sizeof(Pdf_Nav_Animator_Data));
   Evas_Object *border = evas_object_name_find(evas_object_evas_get(icon), "border");
   Evas_Object *page = evas_object_name_find(evas_object_evas_get(icon), "page");
   Evas_Object *tmp_border;
   Evas_Object *tmp_page;
   
   double hscale, vscale;
   char *theme_file;
   
   edje_object_file_get(controls, (const char **)&theme_file, NULL);
   evas_object_geometry_get(controls, NULL, NULL, &ww, &wh);
   
   // Load next page
   tmp_page = esmart_pdf_add(evas_object_evas_get(icon));
   tmp_border = edje_object_add(evas_object_evas_get(icon));
   evas_object_name_set(tmp_page, "tmp_page");
   evas_object_name_set(tmp_border, "tmp_border");
   edje_object_file_set(tmp_border, theme_file, "eyesight/border_opaque");
   
   // Render pdf
   esmart_pdf_file_set(tmp_page, esmart_pdf_file_get(page));
   esmart_pdf_page_set(tmp_page, esmart_pdf_page_get(page) + 1);
   esmart_pdf_scale_get(page, &hscale, &vscale);
   esmart_pdf_scale_set(tmp_page, hscale, vscale);
   esmart_pdf_render(tmp_page);
   
   // Swallow tmp_page
   edje_object_part_swallow(tmp_border, "eyesight/border_opaque/content",
                            tmp_page);
   
   // Resize tmp_border
   esmart_pdf_size_get(tmp_page, &nw, &nh);
   evas_object_resize(tmp_border, (double)nw * hscale, (double)nh * vscale);
   
   // Position tmp_border
   bottom_margin = atoi(edje_file_data_get(theme_file, "bottom_margin"));
   top_margin = atoi(edje_file_data_get(theme_file, "top_margin"));
   evas_object_move(tmp_border, (ww / 2 - (nw * hscale / 2)), 
                    wh + bottom_margin);
   
   // Show
   evas_object_show(tmp_page);
   evas_object_show(tmp_border);
   
   // Set up animation
   animdata->controls = controls;
   animdata->border = border;
   animdata->tmp_border = tmp_border;
   animdata->step = 0;
   animdata->top_margin = top_margin;
   animdata->bottom_margin = bottom_margin;
   
   ecore_animator_add(page_next_animator, animdata);
}

int
page_next_animator(void *_data)
{
   Pdf_Nav_Animator_Data *data = _data;
   int x1, y1, w1, h1;
   int x2, y2, w2, h2;
   int ww, wh;
   int dy; // Delta Y
   
   evas_object_geometry_get(data->controls, NULL, NULL, &ww, &wh);
   evas_object_geometry_get(data->border, &x1, &y1, &w1, &h1);
   evas_object_geometry_get(data->tmp_border, &x2, &y2, &w2, &h2);
   
   dy = 2 * ++(data->step);
   
   //printf("ANIMATE: %d, %d, delta: %d\n", x2, y2, dy);
   
   // Move border1
   if (y1 - dy <= 0 - data->top_margin - h1)
      evas_object_move(data->border, x1, 0 - data->top_margin - h1);
   else
      evas_object_move(data->border, x1, y1 - dy);
      
   if (y2 - dy <= data->top_margin)
      evas_object_move(data->tmp_border, x2, data->top_margin);
   else
      evas_object_move(data->tmp_border, x2, y2-dy);
   
   if (y1 == 0 - data->top_margin - h1 || y2 == data->top_margin)
   {
      evas_object_del(edje_object_part_swallow_get(data->border, 
                                             "eyesight/border_opaque/content"));
      evas_object_del(data->border);
      evas_object_name_set(data->tmp_border, "border");
      evas_object_name_set(edje_object_part_swallow_get(data->tmp_border,
                                              "eyesight/border_opaque/content"),
                                               "page");
      return 0;   // Stop animation
   }
   else
      return 1;
}

void page_prev_clicked(void *data, Evas_Object *icon, const char *emission,
                       const char *source)
{
   printf("Previous page not implented :)\n");
}

