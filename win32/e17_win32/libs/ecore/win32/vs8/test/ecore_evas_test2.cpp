
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

struct ObjectEntry
{
   Evas_Object *object;
   float dw, dh;
   bool image;
};

class ObjectEntryList : public TArray<ObjectEntry>
{
public:
   void AddEntry(Evas_Object *object, float dw, float dh, bool image)
   {
      ObjectEntry oe = {object, dw, dh, image};
      Add(oe);
   }
};

static ObjectEntryList ResizableObjects;
static Ecore_Evas *ee;

void setup_objects_sizes(int nx, int ny);

static void
mouse_button_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   //ecore_evas_win32_window_shape_set(ee, 0, 0, NULL);
   //ecore_win32_window_resize(ecore_evas_window_get(ee), 720, 720);

#if 0
   unsigned char mask[720 * 720];
   memset(mask, 0, sizeof(mask));

   for (int i = 0; i < 720; i++)
   {
      for (int j = 0; j < 720; j++)
      {
         if (j - i < 50 && j - i > -50)
            mask[i * 720 + j] = 255;
      }
   }

   ecore_evas_win32_window_shape_set(ee, 720, 720, mask);
#endif

   ecore_evas_fullscreen_set(ee, (ecore_evas_fullscreen_get(ee) != 0) ? 0 : 1);

   int w, h;
   ecore_win32_window_geometry_get(ecore_evas_window_get(ee), NULL, NULL, &w, &h);
   setup_objects_sizes(w, h);
}

static int
resize_event(void *data, int type, void *event)
{
    Ecore_Win32_Event_Window_Resize *ev = (Ecore_Win32_Event_Window_Resize *)event;
    setup_objects_sizes(ev->width, ev->height);
    return 1;
}

void setup_objects_sizes(int nx, int ny)
{
   for (int i = 0; i < ResizableObjects.Length(); i++)
   {
      {
         evas_object_resize(ResizableObjects[i].object,
            ResizableObjects[i].dw * float(nx), ResizableObjects[i].dh * float(ny));
      }
      if (ResizableObjects[i].image)
      {
         evas_object_image_fill_set(ResizableObjects[i].object, 0, 0, 
            ResizableObjects[i].dw * float(nx), ResizableObjects[i].dh * float(ny));
      }
   }
}

void init_evas(Evas *evas)
{
   Evas_Object *line = evas_object_line_add(evas);
   evas_object_color_set(line, 255, 0, 0, 255);
   evas_object_line_xy_set(line, 10, 20, 100, 200);
   evas_object_show(line);
   line = evas_object_line_add(evas);
   evas_object_color_set(line, 0, 255, 0, 255);
   evas_object_line_xy_set(line, 50, 50, 200, 100);
   evas_object_show(line);

   Evas_Object *rect = evas_object_rectangle_add(evas);
   evas_object_move(rect, 0, 0);
   evas_object_resize(rect, 512, 512);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_UP, mouse_button_up, ee);
   evas_object_show(rect);
   ResizableObjects.AddEntry(rect, 1, 1, false);

   Evas_Object *image = evas_object_image_add(evas);
   evas_object_image_file_set(image, "logo.png", NULL);
   evas_object_move(image, 100, 100);
   evas_object_image_fill_set(image, 0, 0, 120, 160);
   evas_object_resize(image, 120, 160);
   evas_object_show(image);

   image = evas_object_image_add(evas);
   evas_object_image_file_set(image, "border.png", NULL);
   evas_object_move(image, 20, 120);
   evas_object_image_fill_set(image, 0, 0, 100, 350);
   evas_object_resize(image, 100, 350);
   evas_object_image_border_set(image, 8, 8, 8, 8);
   evas_object_show(image);

   image = evas_object_image_add(evas);
   evas_object_image_file_set(image, "border.png", NULL);
   evas_object_move(image, 220, 120);
   evas_object_image_fill_set(image, 0, 0, 100, 350);
   evas_object_resize(image, 100, 350);
   evas_object_show(image);
   ResizableObjects.AddEntry(image, 100.f / 512.f, 350.f / 512.f, true);

 //  evas_font_path_append(evas, "../out");
 //  Evas_Object *font = evas_object_text_add(evas);
	//evas_object_text_font_set(font, "Vera-Bold", 20);
	//evas_object_text_text_set(font, "This is a test string");
	//evas_object_color_set(font, 0, 0, 0, 255);
 //  evas_object_move(font, 220, 10);
	//evas_object_show(font);
}

int
main (int argc, char *argv[])
{
   Evas *evas;
   Evas_Object *o;
   unsigned int *data;

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

   Ecore_Event_Handler *event_handler_resize =
     ecore_event_handler_add(ECORE_WIN32_EVENT_WINDOW_RESIZE, resize_event, NULL);

   //Ecore_Win32_Window *wnd = ecore_win32_window_new(NULL, 10, 10, 256, 256);
   ee = ecore_evas_direct3d_new(NULL, 10, 10, 512, 512);
   unsigned char mask[512 * 512];
   memset(mask, 0xff, sizeof(mask));

   for (int i = 0; i < 512; i++)
   {
      for (int j = 0; j < 512; j++)
      {
         int sq = (i - 256) * (i - 256) + (j - 256) * (j - 256);
         if (sq < 8000 || sq > 40000)
            mask[i * 512 + j] = 0;
         else if (sq < 12000)
            mask[i * 512 + j] = BYTE(255.f * float(sq - 8000) / 4000.f);
         else if (sq > 36000)
            mask[i * 512 + j] = BYTE(255.f * (1.f - float(sq - 36000) / 4000.f));
      }
   }

   ecore_evas_win32_window_shape_set(ee, 512, 512, mask);
   if (ee == NULL)
   {
     ecore_shutdown();
     return EXIT_FAILURE;
   }

   init_evas(ecore_evas_get(ee));
   ecore_evas_show(ee);

   //wnd = ecore_win32_window_new(NULL, 310, 10, 256, 256);
   //ee = ecore_evas_direct3d_new(wnd, 310, 10, 256, 256);
   //if (ee == NULL)
   //{
   //  ecore_shutdown();
   //  return EXIT_FAILURE;
   //}

   //init_evas(ecore_evas_get(ee));
   //ecore_evas_show(ee);


   ecore_main_loop_begin();

   ecore_evas_shutdown();
   ecore_shutdown();

   return EXIT_SUCCESS;
}
