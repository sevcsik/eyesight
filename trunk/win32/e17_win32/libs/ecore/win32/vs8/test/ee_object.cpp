
#include <stdio.h>
#include <assert.h>

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
   TerminateThread(_thread, 0);
   CloseHandle(_thread);
   delete _mask;
}

bool EEObject::Create(Ecore_Evas *ee)
{
   _width = 120;
   _height = 160;
   _ee = ecore_evas_direct3d_new(ecore_evas_window_get(ee), 
      0, 0, _width, _height);
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

   evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_MOVE, mouse_move, this);

   ecore_evas_show(_ee);

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
   MoveWindow(0, 2000);
   ecore_win32_window_size_min_set(ecore_evas_window_get(_ee), _width, _height);
   ecore_win32_window_size_max_set(ecore_evas_window_get(_ee), _width, _height);
   ecore_win32_window_borderless_set(ecore_evas_window_get(_ee), 1);

   _thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc, 
      (LPVOID)this, CREATE_SUSPENDED, NULL);
   assert(_thread != NULL);

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
   _dragging = true;
   ResumeThread(_thread);
}

void EEObject::MoveWindow(int x, int y)
{
   ecore_win32_window_move(ecore_evas_window_get(_ee), x, y);
   _dragging = false;
   SuspendThread(_thread);
}

void EEObject::mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   //EEObject *_this = (EEObject *)data;
   //Evas_Event_Mouse_Move *ev = (Evas_Event_Mouse_Move *)event_info;
   //if (_this->_dragging)
   //{
   //   ecore_win32_window_move(ecore_evas_window_get(_this->_ee), ev->cur.output.x, ev->cur.output.y);
   //}
}

DWORD EEObject::ThreadFunc(LPVOID ptr)
{
   EEObject *_this = (EEObject *)ptr;

   while (true)
   {
      if (_this->_dragging)
      {
         POINT pnt;
         GetCursorPos(&pnt);
         ecore_win32_window_move_resize(ecore_evas_window_get(_this->_ee), 
            pnt.x + 5, pnt.y + 5, _this->_width, _this->_height);
      }
      Sleep(0);
   }
}
