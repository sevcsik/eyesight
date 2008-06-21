#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <Evas.h>

typedef struct _Pdf_Show_Hide_Anim_Data
{
   Evas *evas;
   Evas_Object *object;
   int start_x;
   int ew;
} Pdf_Show_Hide_Anim_Data;

int show_anim(void *data);
int hide_anim(void *data);

int anim_acc_x_calc(void *data);

#endif /*ANIMATIONS_H*/
