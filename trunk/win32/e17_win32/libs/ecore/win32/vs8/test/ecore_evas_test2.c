
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_Win32.h>


int
main (int argc, char *argv[])
{
   Ecore_Evas *ee;
   Evas *evas;
   Evas_Object *o;
   unsigned int *data;

   if (!ecore_init())
     return EXIT_FAILURE;

   if (!ecore_evas_init()) {
     ecore_shutdown();
     return EXIT_FAILURE;
   }

   ee = ecore_evas_direct3d_new(NULL, 10, 10, 256, 256);
   ecore_evas_show(ee);

   evas = ecore_evas_get(ee);

   ecore_main_loop_begin();

   ecore_evas_shutdown();
   ecore_shutdown();

   return EXIT_SUCCESS;
}
