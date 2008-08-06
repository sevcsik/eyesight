
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

extern "C"
{

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_Win32.h>
#include <../modules/engines/direct3d/Evas_Engine_Direct3D.h>

}

static Ecore_Evas *ee1;
static Ecore_Evas *ee2;

bool lmb_down1 = false;
bool dragging1 = false;

static void
mouse_button_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   lmb_down1 = false;
}

static void
mouse_button_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   lmb_down1 = true;
}

static void
mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   if (lmb_down1 && !dragging1)
   {
      dragging1 = true;
      if (ecore_win32_dnd_begin("I'm being dragged", -1) == 1)
         printf("Drag succeeded!\n");
      else
         printf("Drag failed\n");
      dragging1 = false;
      lmb_down1 = false;
   }
}

static int
dnd_callback(void *window, int event, int pt_x, int pt_y, void *data, int size)
{
   //Ecore_Win32_Window *wnd = (Ecore_Win32_Window *)window;
   if (event == ECORE_WIN32_DND_EVENT_DRAG_ENTER)
      printf("Drag enter %p\n", window);
   else if (event == ECORE_WIN32_DND_EVENT_DRAG_OVER)
      printf("Drag over %p\n", window);
   else if (event == ECORE_WIN32_DND_EVENT_DRAG_LEAVE)
      printf("Drag leave %p\n", window);
   else if (event == ECORE_WIN32_DND_EVENT_DROP)
      printf("Dropped to %p: %s\n", window, (const char *)data);
   return 1;
}

int
main (int argc, char *argv[])
{
   Evas *evas;
   Evas_Object *o;
   unsigned int *data;

   if (ecore_win32_dnd_initialize() == 0)
      return EXIT_FAILURE;

   if (!ecore_init())
      return EXIT_FAILURE;

   if (!ecore_win32_init()) {
     ecore_shutdown();
     return EXIT_FAILURE;
   }

   if (!ecore_evas_init()) {
     ecore_shutdown();
     return EXIT_FAILURE;
   }

   ee1 = ecore_evas_direct3d_new(NULL, 10, 10, 256, 256);
   
   if (ee1 == NULL)
   {
     ecore_shutdown();
     return EXIT_FAILURE;
   }

   ee2 = ecore_evas_direct3d_new(NULL, 310, 10, 256, 256);
   
   if (ee2 == NULL)
   {
     ecore_shutdown();
     return EXIT_FAILURE;
   }

   Evas_Object *rect = evas_object_rectangle_add(ecore_evas_get(ee1));
   evas_object_move(rect, 0, 0);
   evas_object_resize(rect, 256, 256);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_UP, mouse_button_up, ee1);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_DOWN, mouse_button_down, ee1);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_MOVE, mouse_move, ee1);
   evas_object_show(rect);

   rect = evas_object_rectangle_add(ecore_evas_get(ee2));
   evas_object_move(rect, 0, 0);
   evas_object_resize(rect, 256, 256);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_UP, mouse_button_up, ee2);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_DOWN, mouse_button_down, ee2);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_MOVE, mouse_move, ee2);
   evas_object_show(rect);

   ecore_evas_show(ee1);
   ecore_evas_show(ee2);

   ecore_win32_dnd_register_drop_target(ecore_evas_window_get(ee1), dnd_callback);
   ecore_win32_dnd_register_drop_target(ecore_evas_window_get(ee2), dnd_callback);

   ecore_main_loop_begin();

   ecore_win32_dnd_unregister_drop_target(ecore_evas_window_get(ee1));
   ecore_win32_dnd_unregister_drop_target(ecore_evas_window_get(ee2));

   ecore_evas_shutdown();
   ecore_shutdown();

   ecore_win32_dnd_uninitialize();

   return EXIT_SUCCESS;
}
