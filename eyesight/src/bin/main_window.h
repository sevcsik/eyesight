#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

struct
{
   Ecore_Evas *ee;
   Evas *evas;
   Evas_Object *main_window;
} main_window;

void main_window_create();
void main_window_resize_cb(Ecore_Evas *ee);

#endif /*MAIN_WINDOW_H_*/
