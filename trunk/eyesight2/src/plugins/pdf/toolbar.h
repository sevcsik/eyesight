#ifndef TOOLBAR_H_
#define TOOLBAR_H_

#include <Evas.h>

typedef enum _Pdf_Toolbar1_Icon
{  // Indexes are the positions in toolbar
   PREV_PAGE = 0,
   // Page no. input field will be here
   NEXT_PAGE = 3
}
Pdf_Toolbar1_Icon;

typedef enum _Pdf_Toolbar2_Icon
{
   FULLSCREEN = 0
}
Pdf_Toolbar2_Icon;

void add_toolbar1_icon(Pdf_Toolbar1_Icon icon, Evas_Object *controls);

void add_toolbar2_icon(Pdf_Toolbar2_Icon icon, Evas_Object *controls);

void toolbar_icon_resize_cb(void *data, Evas *evas, Evas_Object *obj,
                            void *event_info);

void page_next_clicked(void *data, Evas_Object *o, const char *emission,
                       const char *source);

void page_prev_clicked(void *data, Evas_Object *o, const char *emission,
                       const char *source);

void fullscreen_clicked(void *data, Evas_Object *icon, const char *emission,
                        const char *source);

void toolbar_button_resize_cb(void *_data, Evas *evas, Evas_Object *controls,
                              void *event_info);

void setup_toolbar(Evas_Object *controls);

#endif /* TOOLBAR_H_ */
