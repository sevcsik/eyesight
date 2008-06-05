#include "plugin.h"
#include "pdf.h"
#include "animations.h"
#include <Evas.h>
#include <stdio.h>

#define ANIM_TIME 0.5

int
show_anim(void *_data)
{
   // TODO: pdf/animations.c: decelerating movement
   
   Pdf_Show_Anim_Data *data = _data;
   Evas_Object *object = data->object;
   Evas *evas = data->evas;
   int start_x = data->start_x;
   int ew = data->ew;
   int x, y, w, delta_x, end_x;
   
   evas_object_geometry_get(object, &x, &y, &w, NULL);
   end_x = ew / 2 - w / 2;
   delta_x = (start_x - end_x) / (ANIM_TIME / FRAMETIME);
   
   
   if (x - delta_x <= end_x) 
   {
      evas_object_move(object, end_x, y);
      return 0;
   }
   
   evas_object_move(object, x - delta_x, y);
   return 1;
}
