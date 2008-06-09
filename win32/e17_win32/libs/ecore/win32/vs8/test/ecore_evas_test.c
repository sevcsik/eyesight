/* gcc -g -Wall -o test1 test1.c `pkg-config --cflags --libs ecore ecore-evas` -lm */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_Win32.h>


void torus (unsigned int *data);
void triangle (unsigned int *data, int x, int y);

static void
cb_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev;
   double r2;

   ev = event_info;

   r2 = (ev->output.x - 75) * (ev->output.x - 75) + (ev->output.y - 75) *
(ev->output.y - 75);
   if (r2 > (60 * 60))
     {
       data = evas_object_image_data_get(obj, 1);
       memset (data, 0, 150 * 150 * 4);
       torus (data);
       triangle (data, ev->output.x, ev->output.y);
       evas_object_image_data_update_add(obj, 0, 0, 150, 150);
     }
}

static void
ewl_spectrum_hsv_to_rgb(double h, double s, double v,
    unsigned int *r, unsigned int *g, unsigned int *b)
{
  unsigned int r_tmp = 0, g_tmp = 0, b_tmp = 0;

  if (s == 0)
  {
   unsigned int i;

   i = v * 255.0;
   r_tmp = i;
   g_tmp = i;
   b_tmp = i;
  }
  else
  {
   double h_tmp, v_tmp, p, q, t, vs, vsf;
   int i;

   if (h == 360.0) h = 0.0;

   h_tmp = h / 60.0;
   i = h_tmp;

   vs = v * s;
   vsf = vs * (h_tmp - i);

   p = 255.0 * (v - vs);
   q = 255.0 * (v - vsf);
   t = 255.0 * (v - vs + vsf);

   v_tmp = v * 255.0;

   switch(i)
   {
    case 0:
     r_tmp = v_tmp;
     g_tmp = t;
     b_tmp = p;
     break;

    case 1:
     r_tmp = q;
     g_tmp = v_tmp;
     b_tmp = p;
     break;

    case 2:
     r_tmp = p;
     g_tmp = v_tmp;
     b_tmp = t;
     break;

    case 3:
     r_tmp = p;
     g_tmp = q;
     b_tmp = v_tmp;
     break;

    case 4:
     r_tmp = t;
     g_tmp = p;
     b_tmp = v_tmp;
     break;

    case 5:
     r_tmp = v_tmp;
     g_tmp = p;
     b_tmp = q;
     break;
   }
  }

  if (r) *r = r_tmp;
  if (g) *g = g_tmp;
  if (b) *b = b_tmp;
}

/* torus if 15 pixels of width */
void torus (unsigned int *data)
{
   int max_i;
   int i;
   int j;

   max_i = 1000;
   for (i = 0; i < max_i; i++)
     {
       double theta;
       double c;
       double s;
       unsigned int aa, rr, gg, bb;
       unsigned int color;

       theta = (double)i * 2.0 * M_PI / max_i;
       c = cos (theta);
       s = sin (theta);

       ewl_spectrum_hsv_to_rgb(360.0 * i / max_i, 1.0, 1.0, &rr, &gg, &bb);

       aa = 255;
       color = (aa << 24) | (rr << 16) | (gg << 8) | bb;

       for (j = 0; j < 15; j++)
         {
           int x, y;

           x = 75 + (60 + j) * c;
           y = 75 + (60 + j) * s;
           data[y*150 + x] = color;
         }
     }
}

void triangle (unsigned int *data, int x, int y)
{
   double theta;
   double alpha0,  alpha1,  alpha2;
   int x0, y0, x1, y1, x2, y2;
   unsigned int a0, r0, g0, b0;
   unsigned int a1, r1, g1, b1;
   unsigned int a2, r2, g2, b2;
   int i, j, k;
   unsigned int color;

   theta = atan2 ((double)(y - 75), (double)(x - 75));
   if (theta < 0.0)
     theta += 2.0 * M_PI;
   printf (" * theta : %lf\n", theta);

   x0 = 75 + 60 * cos(theta);
   y0 = 75 + 60 * sin(theta);

   x1 = 75 + 60 * cos(theta + 2.0 * M_PI / 3.0);
   y1 = 75 + 60 * sin(theta + 2.0 * M_PI / 3.0);

   x2 = 75 + 60 * cos(theta + 4.0 * M_PI / 3.0);
   y2 = 75 + 60 * sin(theta + 4.0 * M_PI / 3.0);

   a0 = 255;
   ewl_spectrum_hsv_to_rgb(360.0 * theta / (2.0 * M_PI), 1.0, 1.0, &r0, &g0, &b0);

   a1 = 255;
   r1 = 255;
   g1 = 255;
   b1 = 255;

   a2 = 0;
   r2 = 0;
   g2 = 0;
   b2 = 0;

   for (i = 0; i < 190; i++)
     for (j = 0; j < 190; j++)
       for (k = 0; k < 190; k++)
       {
         alpha0 = (double)i / 190.0;
         alpha1 = (double)j / 190.0;
         alpha2 = (double)k / 190.0;
         if (((alpha0 + alpha1 + alpha2) <= 1) &&
             (alpha0 > 0) &&
             (alpha1 > 0) &&
             (alpha2 > 0))
           {
             int x, y;
             int aa, rr, gg, bb;

             x = ceil ((alpha0 * x0 + alpha1 * x1 + alpha2 * x2) / (alpha0 + alpha1 + alpha2));
             y = ceil ((alpha0 * y0 + alpha1 * y1 + alpha2 * y2) / (alpha0 + alpha1 + alpha2));

             /*
             aa = ceil (alpha0 * a0 + alpha1 * a1 + alpha2 * a2);
             rr = ceil (alpha0 * r0 + alpha1 * r1 + alpha2 * r2);
             gg = ceil (alpha0 * g0 + alpha1 * g1 + alpha2 * g2);
             bb = ceil (alpha0 * b0 + alpha1 * b1 + alpha2 * b2);
             */

             /* speed up */
             aa = ceil ((alpha0 + alpha1) * 255);
             rr = ceil (alpha0 * r0 + alpha1 * 255);
             gg = ceil (alpha0 * g0 + alpha1 * 255);
             bb = ceil (alpha0 * b0 + alpha1 * 255);

             color = (aa << 24) | (rr << 16) | (gg << 8) | bb;
             data[y*150 + x] = color;
           }
       }
}

int
main (int argc, char *argv[])
{
   Ecore_Evas *ee;
   Evas *evas;
   Evas_Object *o;
   unsigned int *data;
   Ecore_Win32_Window *wnd;

   printf ("test : %ld\n", sizeof ("tetetete"));

   if (!ecore_init ())
     return EXIT_FAILURE;

   if (!ecore_evas_init ()) {
     ecore_shutdown ();
     return EXIT_FAILURE;
   }

   if (!ecore_win32_init())
   {
      ecore_evas_shutdown();
      ecore_shutdown();
      return EXIT_FAILURE;
   }

   wnd = ecore_win32_window_new(NULL, 100, 100, 256, 256);
   ee = ecore_evas_direct3d_new (wnd, 10, 10, 150, 150);
   ecore_evas_show (ee);

   evas = ecore_evas_get (ee);

   o = evas_object_image_add (evas);
   evas_object_move (o, 0, 0);
   evas_object_resize (o, 150, 150);
   evas_object_image_size_set (o, 150, 150);
   evas_object_image_fill_set (o, 0, 0, 150, 150);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, cb_down, ee);
   evas_object_show (o);

   data = evas_object_image_data_get(o, 1);
   memset (data, 0, 150 * 150 * 4);
   torus (data);
   triangle (data, 136, 75);
   evas_object_image_data_update_add(o, 0, 0, 160, 160);

   ecore_main_loop_begin();

   ecore_win32_shutdown();
   ecore_evas_shutdown();
   ecore_shutdown();

   return EXIT_SUCCESS;
}
