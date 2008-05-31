#include "plugin.h"
#include <esmart_pdf.h>
#include <Ecore_Data.h>
#include <Edje.h>
#include <stdlib.h>
#include <stdio.h>

#define API_VERSION_REQ 1

typedef struct _Pdf_Plugin_Data
{
   Ecore_Hash *files;
} Pdf_Plugin_Data;

void 
identify(char **name, char **version, char **email)
{
   // TODO: pdf.c: i18n
   *name = "PDF viewer (using epdf)";
   *version = "0.1";
   *email = "sevcsik@gmail.com";
}

char
init(void **plugin_data)
{
   if (PLUGIN_API_VERSION != API_VERSION_REQ) return PLUGIN_INIT_API_MISMATCH;
   
   *plugin_data = malloc(sizeof(Pdf_Plugin_Data));
   ((Pdf_Plugin_Data *)*plugin_data)->files = ecore_hash_new(NULL, NULL);
   if (!((Pdf_Plugin_Data *)*plugin_data)->files) return PLUGIN_INIT_FAIL;
   
   // No need to init epdf
   return PLUGIN_INIT_SUCCESS;
}

char
open_file(void **_plugin_data, char *filename, Evas_Object *main_window,
          Evas *evas)
{
   Pdf_Plugin_Data *plugin_data = *_plugin_data;
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
   
   ecore_hash_set(plugin_data->files, filename, page);
   
   // Set up border
   edje_object_file_get(main_window, (const char **)&themefile, NULL);
   border = edje_object_add(evas);
   edje_object_file_set(border, themefile, "border_opaque");
   edje_object_part_swallow(border, "content", page);
   
   // Resize 
   evas_object_geometry_get(main_window, NULL, NULL, &ew, &eh);
   esmart_pdf_size_get(page, &nw, &nh);
   
   // Scale to fit window
   // TODO: pdf.c: Add more scaling options
   // TODO: pdf.c: Get margins from theme (not hardcoded 15 :))
   
   if (((double) ew - 15.0) / ((double) nw) < ((double) eh - 15.0) / ((double) nh))
      scale = ((double) ew - 15.0) / ((double) nw);
   else
      scale = ((double) eh - 15.0) / ((double) nh);
      
   esmart_pdf_scale_set(page, scale, scale);
   esmart_pdf_page_set(page, 1);
   evas_object_resize(border, nw * scale, nh * scale);
   
   // Move to center
   evas_object_move(border, ew / 2 - nw * scale / 2 , eh / 2 - nh * scale / 2);
   
   // Render
   esmart_pdf_render(page); 
   
   // TEMPORARY CODE
   evas_object_show(border);
   evas_object_show(page);
  
   return 1;
}
