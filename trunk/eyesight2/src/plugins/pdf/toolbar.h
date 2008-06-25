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

#endif /* TOOLBAR_H_ */
