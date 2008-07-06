#ifndef PDF_H_
#define PDF_H_

#include <Evas.h>

typedef struct _Pdf_File_Data
{
   Evas_Object *border;
   Evas_Object *page;
   Evas_Object *tmp_border; // Hidden outside the window for animations
   Evas_Object *tmp_page;
}
Pdf_File_Data;

void identify(char **name, char **version, char **email);
char init(void **plugin_data);
char open_file(void **_plugin_data, char *filename, Evas_Object *main_window,
               Evas *evas);
void show(void **plugin_data, char *filename, Evas *evas);
void page_resize_cb(void *_data, Evas *evas, Evas_Object *controls, void *event_info);
int page_resize_cb_render_timer(void *data);

#endif /*PDF_H*/
