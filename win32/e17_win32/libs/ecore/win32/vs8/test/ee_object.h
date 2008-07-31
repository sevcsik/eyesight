
extern "C"
{

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_Win32.h>

}

#include "ref.h"


class EEObject : public Referenc
{
public:
   EEObject();
   ~EEObject();

   bool Create(Ecore_Evas *ee);
   void MoveTo(int x, int y);
   Evas_Object *Object() { return _obj; }
   int GetX() { return _x; }
   int GetY() { return _y; }
   void Show(bool show) 
   { 
      if (show) 
         evas_object_show(_obj);
      else
         evas_object_hide(_obj);
   }
   bool IsShown()
   {
      return evas_object_visible_get(_obj);
   }
   void MakeWindow(int x, int y);
   void MoveWindow(int x, int y);

private:
   static void mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info);
   static DWORD ThreadFunc(LPVOID ptr);

private:
   Ecore_Evas *_ee;
   Evas_Object *_obj;
   unsigned char *_mask;
   int _width, _height, _x, _y;
   bool _dragging;

   HANDLE _thread;

};
