
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

#include "array.h"
#include "dnd.h"
#include "ee_window.h"


/*
static Ecore_Evas *ee1;
static Ecore_Evas *ee2;

bool lmb_down1 = false;
bool dragging1 = false;

static void
mouse_button_up1(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   printf("mouse_button_up1\n");
   SetClipboard("Hello!");
   lmb_down1 = false;
}

static void
mouse_button_up2(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   printf("mouse_button_up2\n");
   char buf[256];
   GetClipboard(buf, 255);
   buf[255] = 0;
   printf(buf);
}

static void
mouse_button_down1(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   printf("mouse_button_down1\n");
   //lmb_down1 = true;
}

static void
mouse_button_down2(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   printf("mouse_button_down2\n");
}

static void
mouse_button_down_obj(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   printf("mouse_button_down_obj\n");
   lmb_down1 = true;
}

static void
mouse_move1(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   if (lmb_down1 && !dragging1)
   {
      POINT curp;
      GetCursorPos(&curp);
      Ecore_Evas *ee = ecore_evas_direct3d_new(NULL, curp.x, curp.y, 30, 40);
      
      ecore_evas_show(ee);

      dragging1 = true;
      BeginDnd("I'm being dragged");
      dragging1 = false;
   }
}

static void
mouse_move2(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
}
*/


int
main (int argc, char *argv[])
{
   Evas *evas;
   Evas_Object *o;
   unsigned int *data;

   if (!DndStartup())
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

   Ref<EEWindow> win1 = new EEWindow();
   Ref<EEWindow> win2 = new EEWindow();
   win1->Create(256, 256, true, false);
   win2->Create(256, 256, true, true);
   win1->MoveTo(10, 10);
   win2->MoveTo(300, 10);

/*
   ee1 = ecore_evas_direct3d_new(NULL, 10, 10, 512, 512);
   
   if (ee1 == NULL)
   {
     ecore_shutdown();
     return EXIT_FAILURE;
   }

   ee2 = ecore_evas_direct3d_new(NULL, 610, 10, 512, 512);
   
   if (ee2 == NULL)
   {
     ecore_shutdown();
     return EXIT_FAILURE;
   }

   Evas_Object *rect = evas_object_rectangle_add(ecore_evas_get(ee1));
   evas_object_move(rect, 0, 0);
   evas_object_resize(rect, 512, 512);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_UP, mouse_button_up1, ee1);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_DOWN, mouse_button_down1, ee1);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_MOVE, mouse_move1, ee1);
   evas_object_show(rect);

   rect = evas_object_rectangle_add(ecore_evas_get(ee2));
   evas_object_move(rect, 0, 0);
   evas_object_resize(rect, 512, 512);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_UP, mouse_button_up2, ee2);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_DOWN, mouse_button_down2, ee2);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_MOVE, mouse_move2, ee2);
   evas_object_show(rect);

   Evas_Object *image = evas_object_image_add(ecore_evas_get(ee1));
   evas_object_image_file_set(image, "logo.png", NULL);
   evas_object_move(image, 100, 100);
   evas_object_image_fill_set(image, 0, 0, 30, 40);
   evas_object_resize(image, 30, 40);
   evas_object_show(image);

   image = evas_object_image_add(ecore_evas_get(ee2));
   evas_object_image_file_set(image, "logo.png", NULL);
   evas_object_move(image, 100, 100);
   evas_object_image_fill_set(image, 0, 0, 30, 40);
   evas_object_resize(image, 30, 40);
   evas_object_event_callback_add(image, EVAS_CALLBACK_MOUSE_DOWN, mouse_button_down_obj, ee1);
   evas_object_hide(image);

   ecore_evas_show(ee1);
   ecore_evas_show(ee2);
*/
   ecore_main_loop_begin();

   win1->Destroy();
   win2->Destroy();

   ecore_evas_shutdown();
   ecore_shutdown();

   DndShutdown();

   return EXIT_SUCCESS;
}
