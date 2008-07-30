
#include "ee_window.h"
#include "dnd.h"

#include <stdio.h>

EEWindow::EEWindow()
{
   _ee = NULL;
   _rect = NULL;
   _lbdown = false;
   _dragging = false;
   _drag_dx = _drag_dy = 0;
   _secondary = false;
   _drop_target = NULL;
}

EEWindow::~EEWindow()
{
   delete _mask;
}

bool EEWindow::Create(int width, int height, bool enable_events, bool secondary)
{
   _width = width;
   _height = height;
   _ee = ecore_evas_direct3d_new(NULL, 0, 0, _width, _height);
   if (_ee == NULL)
      return false;

   _rect = evas_object_rectangle_add(ecore_evas_get(_ee));
   evas_object_move(_rect, 0, 0);
   evas_object_resize(_rect, _width, _height);
   evas_object_show(_rect);

   Evas_Object *rect_area = evas_object_rectangle_add(ecore_evas_get(_ee));
   evas_object_move(rect_area, 50, 50);
   evas_object_resize(rect_area, _width - 100, _height - 100);
   evas_object_color_set(rect_area, 220, 220, 220, 255);
   evas_object_show(rect_area);

   ecore_evas_show(_ee);

   _ee_object = new EEObject();
   if (!_ee_object->Create(_ee))
      return false;

   Evas_Object *obj = _ee_object->Object();
   _secondary = secondary;
   _ee_object->Show(!_secondary);

   if (enable_events)
   {
      evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_DOWN, mouse_left_down, this);
      evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_UP, mouse_left_up, this);
      evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_MOVE, mouse_move, this);
   }

   RegisterDropWindow(
      (HWND)ecore_win32_window_hwnd_get(ecore_evas_win32_window_get(_ee)),
      &_drop_target, drop_callback, (void *)this);

   return true;
}

void EEWindow::Destroy()
{
   UnregisterDropWindow(
      (HWND)ecore_win32_window_hwnd_get(ecore_evas_win32_window_get(_ee)),
      _drop_target);
}

void EEWindow::MoveTo(int x, int y)
{
   ecore_win32_window_move(ecore_evas_window_get(_ee), x, y);
}

void EEWindow::mouse_left_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   EEWindow *_this = (EEWindow *)data;
   Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down *)event_info;

   if (!_this->_ee_object->IsShown())
      return;

   _this->_lbdown = true;

   _this->_drag_dx = _this->_ee_object->GetX() - ev->output.x;
   _this->_drag_dy = _this->_ee_object->GetY() - ev->output.y;
}

void EEWindow::mouse_left_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   EEWindow *_this = (EEWindow *)data;
   _this->_lbdown = false;
}

void EEWindow::mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   EEWindow *_this = (EEWindow *)data;
   Evas_Event_Mouse_Move *ev = (Evas_Event_Mouse_Move *)event_info;

   int x = ev->cur.output.x;
   int y = ev->cur.output.y;
   if (_this->_lbdown && !_this->_dragging)
   {
      _this->_dragging = true;
      if (x < 50 || y < 50 || x > _this->_width - 50 || y > _this->_height - 50)
      {
         _this->_ee_object->Show(false);
         POINT pnt = {x + _this->_drag_dx, y + _this->_drag_dy};
         ClientToScreen(
            (HWND)ecore_win32_window_hwnd_get(ecore_evas_win32_window_get(_this->_ee)),
            &pnt);
         _this->_ee_object->MakeWindow(pnt.x, pnt.y);
         BeginDnd("dragging dragging");
      }
      _this->_dragging = false;
   }


   //if (_this->_lbdown && !_this->_dragging)
   //{
   //   _this->_dragging = true;
   //   BeginDnd("dragging dragging");
   //   _this->_dragging = false;
   //   _this->_lbdown = false;
   //}

   if (_this->_lbdown)
   {
      _this->_ee_object->MoveTo(
         ev->cur.output.x + _this->_drag_dx, 
         ev->cur.output.y + _this->_drag_dy);
   }

}

void EEWindow::drop_callback(void *data, const char *str)
{
   EEWindow *_this = (EEWindow *)data;
   _this->_ee_object->Show(true);
   _this->_lbdown = false;
   printf("I HAVE GOT: %s!!!\n", str);
}

