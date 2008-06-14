#ifndef __EVAS_ENGINE_H__
#define __EVAS_ENGINE_H__


#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "evas_common.h"

#define Log(str, ...) printf("D3D "str"\n", __VA_ARGS__)

typedef void * Direct3DDeviceHandler;

#ifdef __cplusplus
extern "C" {
#endif


Direct3DDeviceHandler evas_direct3d_init(HWND window, int depth);
void         evas_direct3d_free(Direct3DDeviceHandler d3d);

void         evas_direct3d_context_color_set(int r, int g, int b, int a);

void         evas_direct3d_render_all(Direct3DDeviceHandler d3d);

void         evas_direct3d_line_draw(Direct3DDeviceHandler d3d, int x1, int y1, int x2, int y2);
void         evas_direct3d_rectangle_draw(Direct3DDeviceHandler d3d, int x, int y, int w, int h);

#ifdef __cplusplus
}
#endif


#endif /* __EVAS_ENGINE_H__ */
