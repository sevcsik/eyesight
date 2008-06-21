#include "plugin.h"
#include "animations.h"
#include <esmart_pdf.h>
#include <Ecore.h>
#include <Ecore_Data.h>
#include <Edje.h>
#include <stdlib.h>
#include <stdio.h>

#define API_VERSION_REQ 1

typedef struct _Pdf_File_Data
{
   Evas_Object *border;
   Evas_Object *page;
} Pdf_File_Data;

typedef struct _Pdf_Plugin_Data
{
   /* files[0] = prev file; files[1] = current file; files[3] = next file */
   Pdf_File_Data *files[3];
} Pdf_Plugin_Data;

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
   if (PLUGIN_API_VERSION != API_VERSION_REQ) return PLUGIN_INIT_API_MISMATCH;
   
   // No need to init epdf
   return PLUGIN_INIT_SUCCESS;
}

char
open_next(void **_plugin_data, char *filename, Evas_Object *main_window,
          Evas *evas)
{
   Pdf_Plugin_Data *plugin_data = *_plugin_data;
   Pdf_File_Data *file_data = NULL;
   Evas_Object *page;
   Evas_Object *border;
   char *themefile;
   int ew, eh, nw, nh;
   double scale;
   
   page = esmart_pdf_add(evas);
   esmart_pdf_init(page);
   
   // FIXME: pdf.c: EPDF api refresh to use return val of file_set
   esmart_pdf_file_set(page, filename);
   
   // Check that it's a valid file
   if (!esmart_pdf_pdf_document_get(page)) return 0;
   
   // Set up border
   edje_object_file_get(main_window, (const char **)&themefile, NULL);
   border = edje_object_add(evas);
   edje_object_file_set(border, themefile, "border_opaque");
   edje_object_part_swallow(border, "content", page);
   
   // Resize 
   evas_object_geometry_get(main_window, NULL, NULL, &ew, &eh);
   esmart_pdf_size_get(page, &nw, &nh);
   
   // Scale to fit window
   // TODO: pdf/pdf.c: Add more scaling options
   // TODO: pdf/pdf.c: Get margins from theme (not hardcoded 15 :))
   
   if (((double) ew - 15.0) / ((double) nw) < ((double) eh - 15.0) / ((double) nh))
      scale = ((double) ew - 15.0) / ((double) nw);
   else
      scale = ((double) eh - 15.0) / ((double) nh);
      
   esmart_pdf_scale_set(page, scale, scale);
   esmart_pdf_page_set(page, 1);
   evas_object_resize(border, nw * scale, nh * scale);
   
   // Move to left
   // TODO: pdf.c: Get margins from pdf AGAIN
   evas_object_move(border, ew + 50, eh / 2 - nh * scale / 2);
   
   // Render
   esmart_pdf_render(page);
   
   // Save work
   file_data = malloc(sizeof(Pdf_File_Data));
   file_data->page = page;
   file_data->border = border;
   
   plugin_data->files[2] = file_data;
   
   return 1; 
}

void 
close_prev(void **_plugin_data, Evas *evas)
{
   Pdf_Plugin_Data *plugin_data = *_plugin_data;
   Pdf_File_Data *file_data = plugin_data->files[0];
   evas_object_del(file_data->border);
   evas_object_del(file_data->page);
   free(file_data);
}

void
show_next(void **_plugin_data, Evas_Object *main_window, Evas *evas)
{
   Pdf_Plugin_Data *plugin_data = *_plugin_data;
   Pdf_File_Data *file_data = plugin_data->files[2];
   Evas_Object *border;
   Evas_Object *page;
   Pdf_Show_Hide_Anim_Data *show_anim_data;
   
   border = file_data->border;
   page = file_data->page;
   show_anim_data = malloc(sizeof(Pdf_Show_Hide_Anim_Data));
   show_anim_data->evas = evas;
   show_anim_data->object = border;
   
   evas_object_geometry_get(border, &(show_anim_data->start_x), NULL, NULL, NULL);
   evas_object_geometry_get(main_window, NULL, NULL, &(show_anim_data->ew), NULL);
   
   evas_object_show(border);
   evas_object_show(page);
   ecore_animator_add(show_anim, show_anim_data);
}

void
hide(void **_plugin_data, char *filename, Evas_Object *main_window, Evas *evas)
{
   Pdf_Plugin_Data *plugin_data = *_plugin_data;
   Pdf_File_Data *file_data = plugin_data->files[1];
   Evas_Object *border = NULL;
   Evas_Object *page = NULL;
   Pdf_Show_Hide_Anim_Data *hide_anim_data = NULL;
   
   border = file_data->border;
   page = file_data->page;
   hide_anim_data = malloc(sizeof(Pdf_Show_Hide_Anim_Data));
   hide_anim_data->evas = evas;
   hide_anim_data->object = border;
   
   evas_object_geometry_get(border, &(hide_anim_data->start_x), NULL, NULL, NULL);
   evas_object_geometry_get(main_window, NULL, NULL, &(hide_anim_data->ew), NULL);
   
   evas_object_show(border);
   evas_object_show(page);
   ecore_animator_add(hide_anim, hide_anim_data);
}
