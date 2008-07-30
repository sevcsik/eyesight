
#include <stdio.h>

#include "ee_window.h"
#include "dnd.h"

EEObject::EEObject()
{
   _ee = NULL;
   _obj = NULL;
   _mask = NULL;
   _width = _height = _x = _y = 0;
   _dragging = false;
}

EEObject::~EEObject()
{
   delete _mask;
}

bool EEObject::Create(Ecore_Evas *ee)
{
   _width = 120;
   _height = 160;
   _ee = ecore_evas_direct3d_new(NULL, 0, 0, _width, _height);
   if (_ee == NULL)
      return false;

   _x = 100;
   _y = 100;

   _obj = evas_object_image_add(ecore_evas_get(ee));
   evas_object_image_file_set(_obj, "logo.png", NULL);
   evas_object_move(_obj, _x, _y);
   evas_object_image_fill_set(_obj, 0, 0, _width, _height);
   evas_object_resize(_obj, _width, _height);
   evas_object_show(_obj);

   Evas_Object *obj = evas_object_rectangle_add(ecore_evas_get(_ee));
   evas_object_move(obj, 0, 0);
   evas_object_resize(obj, _width, _height);
   evas_object_show(obj);

   obj = evas_object_image_add(ecore_evas_get(_ee));
   evas_object_image_file_set(obj, "logo.png", NULL);
   evas_object_move(obj, 0, 0);
   evas_object_image_fill_set(obj, 0, 0, _width, _height);
   evas_object_resize(obj, _width, _height);
   evas_object_show(obj);

   ecore_evas_hide(_ee);

   delete _mask;
   _mask = new unsigned char[_width * _height];
   for (int i = 0; i < _height; i++)
   {
      for (int j = 0; j < _width; j++)
      {
         int lensq = (i - _height / 2) * (i - _height / 2) +
            (j - _width / 2) * (j - _width / 2);
         float a = 1.f - float(lensq) / (70.f * 70.f);
         if (a < 0.f)
            _mask[i * _width + j] = 0;
         else
            _mask[i * _width + j] = (unsigned char)(255.f * a);
      }
   }
   ecore_evas_win32_window_shape_set(_ee, _width, _height, _mask);

   return true;
}

void EEObject::MoveTo(int x, int y)
{
   _x = x;
   _y = y;
   evas_object_move(_obj, _x, _y);
}

void EEObject::MakeWindow(int x, int y)
{
   printf("POS: %dx%d\n", x, y);
   ecore_win32_window_move(ecore_evas_window_get(_ee), x, y);
   ecore_evas_show(_ee);
   ecore_evas_raise(_ee);
}
