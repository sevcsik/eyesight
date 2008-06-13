#ifndef __EVAS_ENGINE_H__
#define __EVAS_ENGINE_H__


#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "evas_common.h"

#define Log(str, ...) printf("D3D "str"\n", __VA_ARGS__)

typedef struct _Direct3D_DeviceContext Direct3D_DeviceContext;
struct _Direct3D_DeviceContext
{
   HWND                window;
   LPDIRECT3D9         object;
   LPDIRECT3DDEVICE9   device;
   int                 width;
   int                 height;
   int                 rot;
   int                 depth;
   BOOL                device_lost;
   BOOL                scene_rendering;
   D3DPRESENT_PARAMETERS d3dpp;
   D3DSURFACE_DESC     backbuffer_desc;
};


#ifdef __cplusplus
extern "C" {
#endif


int          evas_direct3d_init(HWND window, int depth, Direct3D_DeviceContext *d3d);
void         evas_direct3d_free(Direct3D_DeviceContext *d3d);

void         evas_direct3d_render_all(Direct3D_DeviceContext *d3d);

HRESULT      evas_direct3d_restore(Direct3D_DeviceContext *d3d);
HRESULT      evas_direct3d_reset(Direct3D_DeviceContext *d3d);

void         evas_direct3d_line_draw(Direct3D_DeviceContext *d3d, int x1, int y1, int x2, int y2);

#ifdef __cplusplus
}
#endif


#endif /* __EVAS_ENGINE_H__ */
