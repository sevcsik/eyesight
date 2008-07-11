#include "toolbar.h"
#include "pdf.h"
#include "stdlib.h"
#include "animations.h"
#include "plugin.h"
#include <Evas.h>
#include <Ecore.h>
#include <Edje.h>
#include <Efreet.h>
#include <esmart_pdf.h>
#include <Esmart/Esmart_Container.h>
#include <Esmart/Esmart_Text_Entry.h>
#include <Epdf.h>
#include <Ecore_Evas.h>
#include <stdio.h>
#include <string.h>

typedef void (*Edje_Signal_Callback) (void *data, Evas_Object *o,
                                      const char *emission, const char *source);

// TODO: toolbar.c: localize tooltips
char *toolbar1_tooltips[] = {"Previous page", 0, 0, "Next page"};
char *toolbar2_tooltips[] = {"Go fullscreen"};

Edje_Signal_Callback toolbar1_callbacks[] = {
         page_prev_clicked,
         0,
         0,
         page_next_clicked
      };

Edje_Signal_Callback toolbar2_callbacks[] = {
         fullscreen_clicked
      };

typedef struct _Pdf_Toolbar1_Button_Resize_Cb_Data
{
   Evas_Object *button;
   Pdf_Toolbar1_Icon icon;
}
Pdf_Toolbar1_Button_Resize_Cb_Data;

void
setup_toolbar(Evas_Object *controls)
{
   /* Populating toolbar1 (navigation) */
   // TODO: pdf.c: resize toolbar to fit icons
   Evas_Object *container1;

   container1 = esmart_container_new(evas_object_evas_get(controls));

   // TODO: pdf.c: get toolbar direction from theme
   esmart_container_direction_set(container1, CONTAINER_DIRECTION_HORIZONTAL);
   esmart_container_alignment_set(container1, CONTAINER_ALIGN_CENTER);
   esmart_container_fill_policy_set(container1, CONTAINER_FILL_POLICY_KEEP_ASPECT);

   // Swallow to toolbar1_sw
   edje_object_part_swallow(controls, "eyesight/main_window/controls/toolbar1_sw",
                            container1);
   
   evas_object_repeat_events_set(container1, 1);

   // Fill with items
   add_toolbar1_icon(PREV_PAGE, container1);
   add_toolbar1_text_entry(controls, container1);
   add_toolbar1_icon(NEXT_PAGE, container1);

   evas_object_show(container1);


   /* Populating toolbar2 (view) */
   Evas_Object *container2;

   container2 = esmart_container_new(evas_object_evas_get(controls));

   // TODO: pdf.c: get toolbar direction from theme
   esmart_container_direction_set(container2, CONTAINER_DIRECTION_HORIZONTAL);
   esmart_container_alignment_set(container2, CONTAINER_ALIGN_CENTER);
   esmart_container_fill_policy_set(container2, CONTAINER_FILL_POLICY_KEEP_ASPECT);

   // Swallow to toolbar2_sw
   edje_object_part_swallow(controls, "eyesight/main_window/controls/toolbar2_sw",
                            container2);

   evas_object_repeat_events_set(container2, 1);

   // Fill with items
   add_toolbar2_icon(FULLSCREEN, container2);

   evas_object_show(container1);
}

void
toolbar_icon_resize_cb(void *data, Evas *evas, Evas_Object *obj,
                       void *event_info)
{
   int w, h;
   evas_object_geometry_get(obj, NULL, NULL, &w, &h);
   evas_object_image_fill_set(obj, 0, 0, w, h);
}

void
add_toolbar1_icon(Pdf_Toolbar1_Icon icon, Evas_Object *container1)
{
   Evas_Object *icon_object;
   Evas_Object *icon_image;
   Evas *evas = evas_object_evas_get(container1);
   char *icon_name;
   char *icon_path;
   char *icon_theme;
   char *icon_size;
   char *theme_file;
   enum {ICON_TYPE_GROUP, ICON_TYPE_IMAGE} type;
   int w, h;

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

   edje_object_file_get(evas_object_name_find(evas, "controls"),
                        (const char **)&theme_file, NULL);

   // Load toolbar1_icon from theme

   icon_object = edje_object_add(evas);
   if (!edje_object_file_set(icon_object, theme_file, "eyesight/toolbar1_icon"))
   {
      fprintf(stderr, "Bad theme!, no eyesight/toolbar1_icon group!");
   }
   icon_theme = edje_file_data_get(theme_file, "icon_theme");
   icon_size = edje_file_data_get(theme_file, "icon_size");
   if (!icon_theme)
      icon_theme = "oxygen";
   if (!icon_size)
      icon_size = "128x128";

   // Try to load image from edje file
   type = ICON_TYPE_GROUP;
   icon_image = edje_object_add(evas);
   if (!edje_object_file_set(icon_image, theme_file, icon_name))
   {
      // Failed to get icon from edje
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
   edje_object_size_min_get(icon_object, &w, &h);
   evas_object_resize(icon_object, w, h);
   evas_object_layer_set(icon_object, 100);


   esmart_container_element_append(container1, icon_object);

   if (type == ICON_TYPE_IMAGE) // Set only if image is loded from file
      evas_object_event_callback_add(icon_image, EVAS_CALLBACK_RESIZE,
                                     toolbar_icon_resize_cb, NULL);

   // Set tooltip text
   edje_object_part_text_set(icon_object, "eyesight/toolbar1_icon/tooltip",
                             toolbar1_tooltips[icon]);

   // Set click icallback
   edje_object_signal_callback_add(icon_object, "clicked", "toolbar1_icon",
                                   toolbar1_callbacks[icon], NULL);

   evas_object_show(icon_image);
   evas_object_show(icon_object);
}

void
add_toolbar2_icon(Pdf_Toolbar2_Icon icon, Evas_Object *container2)
{
   Evas_Object *icon_object;
   Evas_Object *icon_image;
   Evas *evas = evas_object_evas_get(container2);
   char *icon_name;
   char *icon_path;
   char *icon_theme;
   char *icon_size;
   char *theme_file;
   enum {ICON_TYPE_GROUP, ICON_TYPE_IMAGE} type;
   int w, h;

   // Get icon filename
   switch (icon)
   {
   case FULLSCREEN:
      icon_name = "view-fullscreen";
      break;
   }

   edje_object_file_get(evas_object_name_find(evas, "controls"),
                        (const char **)&theme_file, NULL);

   // Load toolbar2_icon from theme

   icon_object = edje_object_add(evas);
   if (!edje_object_file_set(icon_object, theme_file, "eyesight/toolbar2_icon"))
   {
      fprintf(stderr, "Bad theme!, no eyesight/toolbar2_icon group!");
   }
   icon_theme = edje_file_data_get(theme_file, "icon_theme");
   icon_size = edje_file_data_get(theme_file, "icon_size");
   if (!icon_theme)
      icon_theme = "oxygen";
   if (!icon_size)
      icon_size = "128x128";

   // Try to load image from edje file
   type = ICON_TYPE_GROUP;
   icon_image = edje_object_add(evas);
   if (!edje_object_file_set(icon_image, theme_file, icon_name))
   {
      // Failed to get icon from edje
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

   edje_object_part_swallow(icon_object, "eyesight/toolbar2_icon/icon", icon_image);

   // Move and resize icon
   edje_object_size_min_get(icon_object, &w, &h);
   evas_object_resize(icon_object, w, h);

   if (type == ICON_TYPE_IMAGE) // Set only if image is loded from file
      evas_object_event_callback_add(icon_image, EVAS_CALLBACK_RESIZE,
                                     toolbar_icon_resize_cb, NULL);

   // Set tooltip text
   edje_object_part_text_set(icon_object, "eyesight/toolbar2_icon/tooltip",
                             toolbar2_tooltips[icon]);

   // Set click icallback
   edje_object_signal_callback_add(icon_object, "clicked", "toolbar2_icon",
                                   toolbar2_callbacks[icon], NULL);
   
   esmart_container_element_append(container2, icon_object);
   
   evas_object_show(icon_image);
   evas_object_show(icon_object);
}

void page_next_clicked(void *_data, Evas_Object *icon, const char *emission,
                       const char *source)
{
   // TODO: toolbar.c: prevent starting multiple animations at multiple clicks on navigation
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
   char *pageno_text;
   Evas_Object *pageno;

   // Check if it's the last page
   if (esmart_pdf_page_get(page) + 1 >= epdf_document_page_count_get(
            esmart_pdf_pdf_document_get(page)))
      return;

   edje_object_file_get(controls, (const char **)&theme_file, NULL);
   evas_object_geometry_get(controls, NULL, NULL, &ww, &wh);

   // Load next page
   tmp_page = esmart_pdf_add(evas_object_evas_get(icon));
   tmp_border = edje_object_add(evas_object_evas_get(icon));
   evas_object_name_set(tmp_page, "tmp_page");
   evas_object_name_set(tmp_border, "tmp_border");
   edje_object_file_set(tmp_border, theme_file, "eyesight/border_opaque");

   esmart_pdf_file_set(tmp_page, esmart_pdf_file_get(page));
   esmart_pdf_page_set(tmp_page, esmart_pdf_page_get(page) + 1);
   esmart_pdf_scale_get(page, &hscale, &vscale);
   esmart_pdf_scale_set(tmp_page, hscale, vscale);

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

   esmart_pdf_render(tmp_page);

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
   
   // Update page number display
   pageno = evas_object_name_find(evas_object_evas_get(tmp_page), "page_no_display");
   pageno_text = malloc(strlen("xxxx / xxxx") + 1);
   sprintf(pageno_text, "%d / %d", esmart_pdf_page_get(tmp_page) + 1, 
           epdf_document_page_count_get(esmart_pdf_pdf_document_get(tmp_page)));
   esmart_text_entry_text_set(pageno, pageno_text);
   free(pageno_text);
}

void page_prev_clicked(void *data, Evas_Object *icon, const char *emission,
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
   Evas_Object *pageno;
   char *pageno_text;

   double hscale, vscale;
   char *theme_file;

   if (esmart_pdf_page_get(page) == 0)
      return;

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
   esmart_pdf_page_set(tmp_page, esmart_pdf_page_get(page) - 1);
   esmart_pdf_scale_get(page, &hscale, &vscale);
   esmart_pdf_scale_set(tmp_page, hscale, vscale);

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
                    0 - bottom_margin - (double)nh * vscale);

   esmart_pdf_render(tmp_page);

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

   ecore_animator_add(page_prev_animator, animdata);
   
   // Update page number display
   pageno = evas_object_name_find(evas_object_evas_get(tmp_page), "page_no_display");
   pageno_text = malloc(strlen("xxxx / xxxx") + 1);
   sprintf(pageno_text, "%d / %d", esmart_pdf_page_get(tmp_page) + 1, 
           epdf_document_page_count_get(esmart_pdf_pdf_document_get(tmp_page)));
   esmart_text_entry_text_set(pageno, pageno_text);
   free(pageno_text);
}

void
fullscreen_clicked(void *data, Evas_Object *icon, const char *emission,
                   const char *source)
{
   Ecore_Evas *ee = ecore_evas_ecore_evas_get(evas_object_evas_get(icon));
   ecore_evas_fullscreen_set(ee, !ecore_evas_fullscreen_get(ee));
   // TODO: pdf/toolbar.c: change icon to view-restore when fullscreen
}

void
add_toolbar1_text_entry(Evas_Object *controls, Evas_Object *container)
{
   char *themefile, *text;
   Evas_Object *edje = edje_object_add(evas_object_evas_get(controls));
   Evas_Object *entry = esmart_text_entry_new(evas_object_evas_get(controls));
   Evas_Object *page = evas_object_name_find(evas_object_evas_get(controls),
                       "page");
   Epdf_Document *doc = esmart_pdf_pdf_document_get(page);

   edje_object_file_get(controls, (const char **)&themefile, NULL);
   edje_object_file_set(edje, themefile, "text_entry");
   
   evas_object_name_set(entry, "page_no_display");

   esmart_text_entry_is_password_set(entry, 0);
   esmart_text_entry_edje_part_set(entry, edje, "text");

   esmart_container_element_append(container, edje);

   evas_object_show(edje);
   evas_object_show(entry);

   evas_object_resize(edje, 100, 40);

   // Fill with text (current page / number of pages)

   text = malloc(strlen("xxxx / xxxx") + 1);
   sprintf(text, "%d / %d", esmart_pdf_page_get(page) + 1,
           epdf_document_page_count_get(doc));
   esmart_text_entry_text_set(entry, text);
   free(text);
}
