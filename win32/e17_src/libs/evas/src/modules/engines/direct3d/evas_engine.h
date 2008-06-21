#ifndef __EVAS_ENGINE_H__
#define __EVAS_ENGINE_H__


#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "evas_common.h"
#include "evas_private.h"

#ifdef ENABLE_LOG_PRINTF
#define Log(str, ...) printf("D3D "str"\n", __VA_ARGS__)
#else
#define Log(str, ...)
#endif

typedef void * Direct3DDeviceHandler;
typedef void * Direct3DImageHandler;

#ifdef __cplusplus
extern "C" {
#endif


Direct3DDeviceHandler evas_direct3d_init(HWND window, int depth);
void         evas_direct3d_free(Direct3DDeviceHandler d3d);

void         evas_direct3d_context_color_set(Direct3DDeviceHandler d3d, int r, int g, int b, int a);
void         evas_direct3d_context_set_multiplier(Direct3DDeviceHandler d3d, int r, int g, int b, int a);

void         evas_direct3d_render_all(Direct3DDeviceHandler d3d);

void         evas_direct3d_line_draw(Direct3DDeviceHandler d3d, int x1, int y1, int x2, int y2);
void         evas_direct3d_rectangle_draw(Direct3DDeviceHandler d3d, int x, int y, int w, int h);

Direct3DImageHandler evas_direct3d_image_load(Direct3DDeviceHandler d3d, 
   const char *file, const char *key, int *error, Evas_Image_Load_Opts *lo);
void evas_direct3d_image_free(Direct3DDeviceHandler d3d, Direct3DImageHandler image);
void evas_direct3d_image_draw(Direct3DDeviceHandler d3d, Direct3DImageHandler image,
   int src_x, int src_y, int src_w, int src_h, 
   int dst_x, int dst_y, int dst_w, int dst_h, int smooth);
void evas_direct3d_image_size_get(Direct3DImageHandler image, int *w, int *h);


#ifdef __cplusplus
}
#endif


#endif /* __EVAS_ENGINE_H__ */
