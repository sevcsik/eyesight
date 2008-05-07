#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#define WM_CLASS "Eyesight"
#define WM_NAME "eyesight"
#define WM_TITLE "Eyesight Document Viewer v0.1"

typedef struct _Main_Window
{
   //Ecore_Evas *ee;
   Evas *evas;
   Evas_Object *main_window;
}
Main_Window;

void main_window_create(Args *args, Evas_List **startup_errors);
void main_window_resize_cb(Ecore_Evas *ee);

#endif /*MAIN_WINDOW_H_*/
