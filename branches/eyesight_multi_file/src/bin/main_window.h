#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#define WM_CLASS "Eyesight"
#define WM_NAME "eyesight"
#define WM_TITLE "Eyesight Document Viewer v0.1"

#include <Evas.h>
#include <Ecore_Evas.h>
#include "conf.h"

typedef struct _Main_Window
{
   //Ecore_Evas *ee;
   Evas *evas;
   Evas_Object *main_window_bg;
   Evas_Object *main_window_fg;
}
Main_Window;

Main_Window *main_window_create(Args *args, Evas_List **startup_errors,
                                Ecore_List *plugins);

void main_window_resize_cb(Ecore_Evas *ee);

/* Edje callbacks */
void main_window_load_cb(void *data, Evas_Object *o, const char *emission, 
                         const char *source);

void main_window_nav_next_cb(void *data, Evas_Object *o, const char *emission, 
                             const char *source);

void main_window_nav_prev_cb(void *data, Evas_Object *o, const char *emission, 
                             const char *source);


#endif /*MAIN_WINDOW_H_*/
