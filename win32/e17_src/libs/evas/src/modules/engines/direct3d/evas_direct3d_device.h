#ifndef __EVAS_DIRECT3D_DEVICE_H__
#define __EVAS_DIRECT3D_DEVICE_H__

#include "evas_engine.h"

#include <assert.h>

#include "ref.h"
#include "array.h"


class D3DDevice : virtual public Referenc
{
public:
   D3DDevice();

   bool Init(HWND window, int depth);
   void Destroy();
   bool Begin();
   bool End();

   inline LPDIRECT3DDEVICE9 GetDevice();
   inline int GetWidth();
   inline int GetHeight();

private:
   HRESULT RestoreDevice();
   HRESULT ResetDevice();

private:
   HWND _window;
   LPDIRECT3D9 _object;
   LPDIRECT3DDEVICE9 _device;
   int _width;
   int _height;
   int _rot;
   int _depth;
   bool _device_lost;
   bool _scene_rendering;
   D3DPRESENT_PARAMETERS _d3dpp;
   D3DSURFACE_DESC _backbuffer_desc;
};

LPDIRECT3DDEVICE9 D3DDevice::GetDevice()
{
   return _device;
}

int D3DDevice::GetWidth()
{
   return _width;
}

int D3DDevice::GetHeight()
{
   return _height;
}

#endif  // __EVAS_DIRECT3D_DEVICE_H__
