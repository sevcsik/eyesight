#include "Eyesight.h"

char
init_libs(int argc, char **argv)
{
   // initialization

   if (!ecore_init())
   {
      printf("init_libs: Failed to init ecore!\n");
      return 0;
   }

   if (!evas_init())
   {
      printf("init_libs: Failed to init evas!\n");
      return 0;
   }

   if (!ecore_evas_init())
   {
      printf("init_libs: Failed to init ecore_evas!\n");
      return 0;
   }

   if (!edje_init())
   {
      printf("init_libs: Failed to init edje!\n");
      return 0;
   }
   return 1;
}
