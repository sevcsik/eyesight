
extern "C"
{

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_Win32.h>

}

#include <windows.h>

#include "ref.h"
#include "ee_object.h"


class EEWindow : public Referenc
{
public:
   EEWindow();
   ~EEWindow();

   bool Create(int width, int height, bool enable_events, bool secondary);
   void Destroy();
   void MoveTo(int x, int y);
   EEObject *Object() { return _ee_object.Addr(); }

private:
   static void mouse_left_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
   static void mouse_left_up(void *data, Evas *e, Evas_Object *obj, void *event_info);
   static void mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info);
   static void drop_callback(void *data, const char *str);

private:
   Ecore_Evas *_ee;
   Evas_Object *_rect;
   unsigned char *_mask;
   int _width, _height;
   bool _lbdown;
   bool _dragging;
   bool _secondary;
   IDropTarget *_drop_target;

   Ref<EEObject> _ee_object;
   int _drag_dx, _drag_dy;

};
