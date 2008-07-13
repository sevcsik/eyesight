
#include "evas_engine.h"
#include "evas_private.h"
#include "Evas_Engine_Direct3D.h"

#undef EAPI
#define EAPI __declspec(dllexport)

/* engine struct data */
typedef struct _Render_Engine Render_Engine;
struct _Render_Engine
{
   Direct3DDeviceHandler d3d;
   int width, height;
   int end : 1;
   int in_redraw : 1;
};


/* function tables - filled in later (func and parent func) */
static Evas_Func func, pfunc;

//////////////////////////////////////////////////////////////////////////////
// Prototypes

static void *eng_info(Evas *e);
static void  eng_info_free(Evas *e, void *info);
static void  eng_setup(Evas *e, void *info);
static void  eng_output_free(void *data);
static void  eng_output_resize(void *data, int width, int height);

//////////////////////////////////////////////////////////////////////////////
// Init / shutdown methods
//

static void *
_output_setup(int width, int height, int rotation, HWND window, int depth)
{
   Render_Engine *re;

   re = (Render_Engine *)calloc(1, sizeof(Render_Engine));
   if (!re) 
     return NULL;

   /* if we haven't initialized - init (automatic abort if already done) */
   evas_common_cpu_init();

   evas_common_blend_init();
   evas_common_image_init();
   evas_common_convert_init();
   evas_common_scale_init();
   evas_common_rectangle_init();
   evas_common_gradient_init();
   evas_common_polygon_init();
   evas_common_line_init();
   evas_common_font_init();
   evas_common_draw_init();
   evas_common_tilebuf_init();

   if ((re->d3d = evas_direct3d_init(window, depth)) == 0)
     {
     free(re);
     return NULL;
     }

   re->width = width;
   re->height = height;

   return re;
}

static void *
eng_info(Evas *e)
{
   Evas_Engine_Info_Direct3D *info;
   info = (Evas_Engine_Info_Direct3D *)calloc(1, sizeof(Evas_Engine_Info_Direct3D));
   if (!info) return NULL;
   info->magic.magic = rand();
   return info;
   e = NULL;
}

static void
eng_info_free(Evas *e, void *info)
{
   Evas_Engine_Info_Direct3D *in;
   in = (Evas_Engine_Info_Direct3D *)info;
   free(in);
}

static void
eng_setup(Evas *e, void *info)
{
   Render_Engine *re;
   Evas_Engine_Info_Direct3D *in;

   in = (Evas_Engine_Info_Direct3D *)info;
   if (!e->engine.data.output)
     {
     e->engine.data.output = _output_setup(e->output.w, e->output.h,
		   in->info.rotation, in->info.window, in->info.depth);
     }

   if (e->engine.data.output == NULL) 
     return;
   if (e->engine.data.context == NULL)
     e->engine.data.context = e->engine.func->context_new(e->engine.data.output);

   re = (Render_Engine *)e->engine.data.output;
}

static void
eng_output_free(void *data)
{
   Render_Engine *re = (Render_Engine *)data;

   evas_direct3d_free(re->d3d);

   free(re);

   evas_common_font_shutdown();
   evas_common_image_shutdown();
}

//////////////////////////////////////////////////////////////////////////////
// Context
//

static void
eng_context_color_set(void *data, void *context, int r, int g, int b, int a)
{
   Render_Engine *re = (Render_Engine *)data;
   evas_direct3d_context_color_set(re->d3d, r, g, b, a);

   evas_common_draw_context_set_color(context, r, g, b, a);
}

static void
eng_context_multiplier_set(void *data, void *context, int r, int g, int b, int a)
{
   Render_Engine *re = (Render_Engine *)data;
   evas_direct3d_context_set_multiplier(re->d3d, 255, 255, 255, a);

   evas_common_draw_context_set_multiplier(context, r, g, b, a);
}

static void
eng_context_multiplier_unset(void *data, void *context)
{
   Render_Engine *re = (Render_Engine *)data;
   evas_direct3d_context_set_multiplier(re->d3d, 255, 255, 255, 255);

   evas_common_draw_context_unset_multiplier(context);
}


//////////////////////////////////////////////////////////////////////////////
// Output manipulating
//

static void
eng_output_resize(void *data, int width, int height)
{
   Render_Engine *re = (Render_Engine *)data;
   re->width = width;
   re->height = height;
   evas_direct3d_resize(re->d3d, width, height);
}

static void
eng_output_redraws_rect_add(void *data, int x, int y, int width, int height)
{
   Render_Engine *re = (Render_Engine *)data;
}

static void
eng_output_redraws_rect_del(void *data, int x, int y, int width, int height)
{
   Render_Engine *re = (Render_Engine *)data;
}

static void
eng_output_redraws_clear(void *data)
{
   Render_Engine *re = (Render_Engine *)data;
}

static void *
eng_output_redraws_next_update_get(void *data, int *x, int *y, int *w, int *h,
  int *cx, int *cy, int *cw, int *ch)
{
   Render_Engine *re;

   re = (Render_Engine *)data;
   if (re->end)
     {
     re->end = 0;
     re->in_redraw = 0;
     return NULL;
     }

   if (x) *x = 0;
   if (y) *y = 0;
   if (w) *w = 800;  //re->d3d.width;
   if (h) *h = 600;  //re->d3d.height;
   if (cx) *cx = 0;
   if (cy) *cy = 0;
   if (cw) *cw = 800;  //re->d3d.width;
   if (ch) *ch = 600;  //re->d3d.height;

   re->in_redraw = 1;

   return re;  
}

static void
eng_output_redraws_next_update_push(void *data, void *surface, 
  int x, int y, int w, int h)
{
   Render_Engine *re = (Render_Engine *)data;
   re->end = 1;
}

static void
eng_output_flush(void *data)
{
   Render_Engine *re = (Render_Engine *)data;
   evas_direct3d_render_all(re->d3d);
}

static void
eng_output_idle_flush(void *data)
{
   Render_Engine *re = (Render_Engine *)data;
}


//////////////////////////////////////////////////////////////////////////////
// Draw objects
//

static void
eng_line_draw(void *data, void *context, void *surface, int x1, int y1, int x2, int y2)
{
   Render_Engine *re = (Render_Engine *)data;
   if (re->in_redraw == 0)
      return;
   evas_direct3d_line_draw(re->d3d, x1, y1, x2, y2);
}

static void
eng_rectangle_draw(void *data, void *context, void *surface, int x, int y, int w, int h)
{
   Render_Engine *re = (Render_Engine *)data;
   if (re->in_redraw == 0)
      return;
   evas_direct3d_rectangle_draw(re->d3d, x, y, w, h);
}

static void *
eng_image_load(void *data, const char *file, const char *key, int *error, Evas_Image_Load_Opts *lo)
{
   Render_Engine *re = (Render_Engine *)data;
   *error = 0;
   return evas_direct3d_image_load(re->d3d, file, key, NULL, lo);
}

static void *
eng_image_new_from_data(void *data, int w, int h, DATA32 *image_data, int alpha, int cspace)
{
   Render_Engine *re = (Render_Engine *)data;
   return evas_direct3d_image_new_from_data(re->d3d, w, h, image_data, alpha, cspace);
}

static void *
eng_image_new_from_copied_data(void *data, int w, int h, DATA32 *image_data, int alpha, int cspace)
{
   Render_Engine *re = (Render_Engine *)data;
   return evas_direct3d_image_new_from_copied_data(re->d3d, w, h, image_data, alpha, cspace);
}

static void
eng_image_free(void *data, void *image)
{
   Render_Engine *re = (Render_Engine *)data;
   evas_direct3d_image_free(re->d3d, image);
}

static void *
eng_image_data_put(void *data, void *image, DATA32 *image_data)
{
   Render_Engine *re = (Render_Engine *)data;
   evas_direct3d_image_data_put(re->d3d, image, image_data);
   return image;
}

static void *
eng_image_dirty_region(void *data, void *image, int x, int y, int w, int h)
{
   return image;
}

static void *
eng_image_data_get(void *data, void *image, int to_write, DATA32 **image_data)
{
   Render_Engine *re = (Render_Engine *)data;
   evas_direct3d_image_data_get(re->d3d, image, to_write, image_data);
   return image;
}

static void
eng_image_draw(void *data, void *context, void *surface, void *image, 
   int src_x, int src_y, int src_w, int src_h, 
   int dst_x, int dst_y, int dst_w, int dst_h, int smooth)
{
   Render_Engine *re = (Render_Engine *)data;
   evas_direct3d_image_draw(re->d3d, image,
      src_x, src_y, src_w, src_h, 
      dst_x, dst_y, dst_w, dst_h, smooth);
}

static void
eng_image_size_get(void *data, void *image, int *w, int *h)
{
   evas_direct3d_image_size_get(image, w, h);
}

static int
eng_image_alpha_get(void *data, void *image)
{
   // Hm:)
   if (!image) 
      return 1;
   return 0;
}

static int
eng_image_colorspace_get(void *data, void *image)
{
   // Well, change that when you think about other colorspace
   return EVAS_COLORSPACE_ARGB8888;
}

static void *
eng_image_border_set(void *data, void *image, int l, int r, int t, int b)
{
   Render_Engine *re = (Render_Engine *)data;
   evas_direct3d_image_border_set(re->d3d, image, l, t, r, b);
   return image;
}

static void
eng_image_border_get(void *data, void *image, int *l, int *r, int *t, int *b)
{
   Render_Engine *re = (Render_Engine *)data;
   evas_direct3d_image_border_get(re->d3d, image, l, t, r, b);
}

static void
eng_font_draw(void *data, void *context, void *surface, void *font, int x, int y, int w, int h, int ow, int oh, const char *text)
{
   Render_Engine *re = (Render_Engine *)data;
	RGBA_Image im;
   im.image.data = NULL;
   im.cache_entry.w = re->width;
   im.cache_entry.h = re->height;

   evas_direct3d_select_or_create_font(re->d3d, font);

   evas_common_draw_context_font_ext_set(context, re->d3d,
      evas_direct3d_font_texture_new, 
      evas_direct3d_font_texture_free, 
      evas_direct3d_font_texture_draw);
   evas_common_font_draw(&im, context, font, x, y, text);
   evas_common_draw_context_font_ext_set(context, NULL, NULL, NULL, NULL);
}

static void
eng_font_free(void *data, void *font)
{
   Render_Engine *re = (Render_Engine *)data;
   evas_common_font_free(font);
   evas_direct3d_font_free(re->d3d, font);
}


/* module advertising code */
EAPI int
module_open(Evas_Module *em)
{
   if (!em) return 0;
   /* get whatever engine module we inherit from */
   if (!_evas_module_engine_inherit(&pfunc, "software_generic")) return 0;
   /* store it for later use */
   func = pfunc;
   /* now to override methods */
#define ORD(f) EVAS_API_OVERRIDE(f, &func, eng_)
   ORD(info);
   ORD(info_free);
   ORD(setup);
   ORD(context_color_set);
   ORD(context_multiplier_set);
   ORD(context_multiplier_unset);
   ORD(output_free);
   ORD(output_resize);
   ORD(output_redraws_rect_add);
   ORD(output_redraws_rect_del);
   ORD(output_redraws_clear);
   ORD(output_redraws_next_update_get);
   ORD(output_redraws_next_update_push);
   ORD(output_flush);
   ORD(output_idle_flush);
   ORD(line_draw);
   ORD(rectangle_draw);
   ORD(image_load);
   ORD(image_new_from_data);
   ORD(image_new_from_copied_data);
   ORD(image_free);
   ORD(image_data_put);
   ORD(image_dirty_region);
   ORD(image_data_get);
   ORD(image_draw);
   ORD(image_size_get);
   ORD(image_alpha_get);
   ORD(image_colorspace_get);
   ORD(image_border_set);
   ORD(image_border_get);
   ORD(font_draw);
   ORD(font_free);
   /* now advertise out own api */
   em->functions = (void *)(&func);
   return 1;
}

EAPI void
module_close(void)
{
}

EAPI Evas_Module_Api evas_modapi =
{
   EVAS_MODULE_API_VERSION,
   EVAS_MODULE_TYPE_ENGINE,
   "direct3d",
   "none"
};
