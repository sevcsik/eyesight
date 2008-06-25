#ifndef TOOLBAR_H_
#define TOOLBAR_H_

#include <Evas.h>

typedef enum _Pdf_Toolbar1_Icon
{  // Indexes are the positions in toolbar
   PREV_PAGE = 0,
   // Page no. input field will be here
   NEXT_PAGE = 3
} Pdf_Toolbar1_Icon;

void add_toolbar1_icon(Pdf_Toolbar1_Icon icon, Evas_Object *controls);

void toolbar_icon_resize_cb(void *data, Evas *evas, Evas_Object *obj,
                            void *event_info);

#endif /* TOOLBAR_H_ */
