#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

struct
{
   Ecore_Evas *ee;
   Evas *evas;
   Evas_Object *main_window;
} main_window;

void create_main_window();

#endif /*MAIN_WINDOW_H_*/
