#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#define WM_CLASS "Eyesight"
#define WM_NAME "eyesight"
#define WM_TITLE "Eyesight Document Viewer v0.1"

struct _main_window
{
   Ecore_Evas *ee;
   Evas *evas;
   Evas_Object *main_window;
} main_window;

void main_window_create();
void main_window_resize_cb(Ecore_Evas *ee);

#endif /*MAIN_WINDOW_H_*/
