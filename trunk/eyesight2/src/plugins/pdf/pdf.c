#include "pdf.h"

#include "plugin.h"
#include "animations.h"
#include "toolbar.h"
#include <esmart_pdf.h>
#include <Ecore.h>
#include <Ecore_Data.h>
#include <Edje.h>
#include <Efreet.h>
#include <stdlib.h>
#include <stdio.h>

#define API_VERSION_REQ 1

typedef struct _Pdf_Plugin_Data
{
   Ecore_Hash *files;
}
Pdf_Plugin_Data;

typedef struct _Pdf_Page_Resize_Cb_Data
{
   Evas_Object *page;
   Evas_Object *border;
   int top_margin;
   int bottom_margin;
   int left_margin;
   int right_margin;
   Ecore_Timer *render_timer;
}
Pdf_Page_Resize_Cb_Data;

void
identify(char **name, char **version, char **email)
{
   // TODO: pdf/pdf.c: i18n
   *name = "PDF viewer (using epdf)";
   *version = "0.1";
   *email = "sevcsik@gmail.com";
}

char
init(void **plugin_data)
{
   if (PLUGIN_API_VERSION != API_VERSION_REQ)
      return PLUGIN_INIT_API_MISMATCH;

   *plugin_data = malloc(sizeof(Pdf_Plugin_Data));
   ((Pdf_Plugin_Data *)*plugin_data)->files = ecore_hash_new(NULL, NULL);
   if (!((Pdf_Plugin_Data *)*plugin_data)->files)
      return PLUGIN_INIT_FAIL;

   efreet_init();
   // No need to init epdf
   return PLUGIN_INIT_SUCCESS;
}

char
open_file(void **_plugin_data, char *filename, Evas_Object *main_window,
          Evas *evas)
{
   Pdf_Plugin_Data *plugin_data = *_plugin_data;
   Pdf_File_Data *file_data = NULL;
   Evas_Object *page;
   Evas_Object *border;
   char *themefile;
   int ew, eh, nw, nh;
   double scale;
   int top_margin, bottom_margin, left_margin, right_margin;
   Pdf_Page_Resize_Cb_Data *resize_cb_data;
   Ecore_List *resize_callbacks;
   Controls_Resize_Cbdata *cbdata;
   char *tmp;

   page = esmart_pdf_add(evas);
   esmart_pdf_init(page);
   evas_object_name_set(page, "page");

   // FIXME: pdf.c: EPDF api refresh to use return val of file_set
   esmart_pdf_file_set(page, filename);

   // Check that it's a valid file
   if (!esmart_pdf_pdf_document_get(page))
      return 0;

   // Set up border
   edje_object_file_get(main_window, (const char **)&themefile, NULL);
   border = edje_object_add(evas);
   edje_object_file_set(border, themefile, "eyesight/border_opaque");
   edje_object_part_swallow(border, "eyesight/border_opaque/content", page);
   evas_object_name_set(border, "border");

   // Resize
   evas_object_geometry_get(main_window, NULL, NULL, &ew, &eh);
   esmart_pdf_size_get(page, &nw, &nh);

   // Scale to fit window
   // TODO: pdf/pdf.c: Add more scaling options
   top_margin = atoi(tmp = edje_file_data_get(themefile, "top_margin"));
   free(tmp);
   bottom_margin = atoi(tmp = edje_file_data_get(themefile, "bottom_margin"));
   free(tmp);
   left_margin = atoi(tmp = edje_file_data_get(themefile, "left_margin"));
   free(tmp);
   right_margin = atoi(tmp = edje_file_data_get(themefile, "right_margin"));
   free(tmp);

   // Add callback to resize callback list
   // TODO: pdf.c: Free resize_cb_data somewhere
   resize_cb_data = malloc(sizeof(Pdf_Page_Resize_Cb_Data));
   resize_cb_data->page = page;
   resize_cb_data->border = border;
   resize_cb_data->top_margin = top_margin;
   resize_cb_data->bottom_margin = bottom_margin;
   resize_cb_data->left_margin = left_margin;
   resize_cb_data->right_margin = right_margin;
   resize_cb_data->render_timer = NULL;

   cbdata = malloc(sizeof(Controls_Resize_Cbdata));
   cbdata->data = resize_cb_data;
   cbdata->func = page_resize_cb;

   resize_callbacks = evas_object_data_get(evas_object_name_find(evas, "controls"),
                                           "resize_callbacks");

   ecore_list_append(resize_callbacks, cbdata);

   // Resize page

   if ((double) ew - (left_margin + right_margin) / ((double) nw) <
         ((double) eh - (top_margin + bottom_margin) / ((double) nh)))
   {
      scale = (double) (eh - (top_margin + bottom_margin)) / (double) nh;
   }
   else
   {
      scale = (double) (eh - (top_margin + bottom_margin)) / (double) nh;
   }

   esmart_pdf_scale_set(page, scale, scale);
   esmart_pdf_page_set(page, 0);
   evas_object_resize(border, nw * scale, nh * scale);

   // Move to left
   evas_object_move(border, ew + left_margin, eh / 2 - nh * scale / 2);

   // Render
   esmart_pdf_render(page);

   // Save work
   file_data = malloc(sizeof(Pdf_File_Data));
   file_data->page = page;
   file_data->border = border;
   file_data->tmp_page = NULL;
   file_data->tmp_border = NULL;
   ecore_hash_set(plugin_data->files, filename, file_data);

   return 1;
}

void
show(void **_plugin_data, char *filename, Evas *evas)
{
   Pdf_Plugin_Data *plugin_data = *_plugin_data;
   Pdf_File_Data *file_data = NULL;
   Ecore_Hash *hash = NULL;
   Evas_Object *border = NULL;
   Evas_Object *page = NULL;
   Pdf_Show_Anim_Data *show_anim_data = NULL;

   hash = plugin_data->files;
   file_data = ecore_hash_get(hash, filename);
   border = file_data->border;
   page = file_data->page;
   show_anim_data = malloc(sizeof(Pdf_Show_Anim_Data));
   show_anim_data->evas = evas;
   show_anim_data->object = border;

   evas_object_geometry_get(border, &(show_anim_data->start_x), NULL, NULL, NULL);
   evas_object_geometry_get(evas_object_name_find(evas, "main_window"),
                            NULL, NULL, &(show_anim_data->ew), NULL);

   setup_toolbar(evas_object_name_find(evas, "controls"));

   evas_object_show(border);
   evas_object_show(page);
   ecore_animator_add(show_anim, show_anim_data);
}

void
setup_toolbar(Evas_Object *controls)
{
   /* Populating toolbar1 (navigation) */
   // TODO: pdf.c: resize toolbar to fit icons

   add_toolbar1_icon(PREV_PAGE, controls);
   add_toolbar1_icon(NEXT_PAGE, controls);
}

void
page_resize_cb(void *_data, Evas *evas, Evas_Object *controls, void *event_info)
{
   Pdf_Page_Resize_Cb_Data *data = _data;
   int ew, eh, nw, nh, h_margins, v_margins;
   double scale;
   Evas_Object *page, *border;

   page = evas_object_name_find(evas, "page");
   border = evas_object_name_find(evas, "border");

   h_margins = data->left_margin + data->right_margin;
   v_margins = data->top_margin + data->bottom_margin;

   evas_object_geometry_get(controls, NULL, NULL, &ew, &eh);
   esmart_pdf_size_get(page, &nw, &nh);

   scale = (((double)eh - v_margins) / (double)nh <   \
            ((double)ew - h_margins) / (double)nw) ?  \
           (((double)eh - v_margins) / (double)nh) :  \
           (((double)ew - h_margins) / (double)nw);


   evas_object_resize(border, nw * scale, nh * scale);
   if (scale == ((double)eh - v_margins) / (double)nh)
      evas_object_move(border, ew / 2 - nw * scale / 2, data->top_margin);
   else
      evas_object_move(border, data->left_margin, eh / 2 - nh * scale / 2);

   esmart_pdf_scale_set(page, scale, scale);

   if (data->render_timer) // We're still resizing, delete timer set on prev resize
      ecore_timer_del(data->render_timer);

   data->render_timer = ecore_timer_add(0.1, page_resize_cb_render_timer,
                                        page);
}

int page_resize_cb_render_timer(void *data)
{
   esmart_pdf_render((Evas_Object *) data);
   return 0;
}
