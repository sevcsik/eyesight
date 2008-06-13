
#include "evas_engine.h"
#include <assert.h>

#include "evas_direct3d_shader_pack.h"
#include "evas_direct3d_scene.h"
#include "evas_direct3d_object_line.h"

static Ref<D3DScene> scene;

extern "C" {

int
evas_direct3d_init(HWND window, int depth, Direct3D_DeviceContext *d3d)
{
   D3DPRESENT_PARAMETERS pp;
   D3DDISPLAYMODE dm;
   D3DCAPS9 caps;
   RECT rect;
   DWORD flag;
   HRESULT hr;
   
   if (d3d == NULL)
      return 0;
   if (window == NULL)
      return 0;

   evas_direct3d_free(d3d);
   
   d3d->object = Direct3DCreate9(D3D_SDK_VERSION);
   if (d3d->object == NULL)
     return 0;

   if (FAILED(hr = d3d->object->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dm)))
     {
     Log("GetAdapterDisplayMode failed: %x", hr);
     evas_direct3d_free(d3d);
     return 0;
     }

   if (FAILED(hr = d3d->object->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
     {
     Log("GetDeviceCaps failed: %x", hr);
     evas_direct3d_free(d3d);
     return 0;
     }

   if (!GetClientRect(window, &rect))
     {
     Log("GetClientRect failed: %x", GetLastError());
     evas_direct3d_free(d3d);
     return 0;
     }

   if (SUCCEEDED(d3d->object->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
     dm.Format, 0, D3DRTYPE_TEXTURE, (depth == 16) ? D3DFMT_R5G6B5 : D3DFMT_A8R8G8B8)))
     {
     dm.Format = (depth == 16) ? D3DFMT_R5G6B5 : D3DFMT_A8R8G8B8;
     }

   flag = (caps.VertexProcessingCaps != 0) ? 
     (D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE) :
     D3DCREATE_SOFTWARE_VERTEXPROCESSING;

   ZeroMemory(&pp, sizeof(pp));
   pp.BackBufferWidth = rect.right - rect.left;
   pp.BackBufferHeight = rect.bottom - rect.top;
   pp.BackBufferFormat = dm.Format;
   pp.BackBufferCount = 1;
   pp.MultiSampleType = D3DMULTISAMPLE_NONE;
   pp.MultiSampleQuality = 0;
   pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
   pp.hDeviceWindow = window;
   pp.Windowed  = TRUE;
   pp.EnableAutoDepthStencil = FALSE;
   pp.FullScreen_RefreshRateInHz = 0;
   pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

   if (FAILED(hr = d3d->object->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
     window, flag, &pp, &d3d->device)))
     {
     Log("CreateDevice failed: %x", hr);
     evas_direct3d_free(d3d);
     return 0;
     }

   LPDIRECT3DSURFACE9 backbuffer = NULL;
   d3d->device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
   backbuffer->GetDesc(&d3d->backbuffer_desc);
   backbuffer->Release();

   switch (dm.Format) {
   case D3DFMT_A8R8G8B8:
   case D3DFMT_X8R8G8B8:
     d3d->depth = 32;
     break;
   case D3DFMT_R5G6B5:
     d3d->depth = 16;
     break;
   default:
     Log("No supported format found");
     evas_direct3d_free(d3d);
     return 0;
   }

   d3d->d3dpp = pp;
   d3d->device_lost = FALSE;
   d3d->scene_rendering = FALSE;
   d3d->width = rect.right - rect.left;
   d3d->height = rect.bottom - rect.top;

   if (!D3DShaderPack::This()->Initialize(d3d))
   {
      Log("Failed to build shader pack");
      evas_direct3d_free(d3d);
      return 0;
   }

   scene = new D3DScene();

   Log("initialized");
   return 1;
}

void
evas_direct3d_free(Direct3D_DeviceContext *d3d)
{
   scene = NULL;
   D3DShaderPack::This()->Uninitialize();

   if (d3d == NULL)
      return;
   if (d3d->device != NULL)
      d3d->device->Release();
   if (d3d->object != NULL)
      d3d->object->Release();
   ZeroMemory(d3d, sizeof(*d3d));

   Log("uninitialized");
}

void         
evas_direct3d_render_all(Direct3D_DeviceContext *d3d)
{
   Log("render");
   assert(d3d != NULL);

   evas_direct3d_restore(d3d);
   
   if (FAILED(d3d->device->BeginScene()))
      return;

   d3d->device->Clear(0, NULL, D3DCLEAR_TARGET, 0xffffffff, 1.f, 0);

   D3DObjectLine::BegineCache();
   scene->DrawAll(d3d);
   D3DObjectLine::EndCache(d3d);

   d3d->device->EndScene();
   d3d->device->Present(NULL, NULL, NULL, NULL);

   scene->FreeObjects();
}


HRESULT evas_direct3d_restore(Direct3D_DeviceContext *d3d)
{
   Log("restore");
   assert(d3d != NULL);

   HRESULT hr = S_OK;

   // Test the cooperative level to see if it's okay to render
   if (SUCCEEDED(hr = d3d->device->TestCooperativeLevel()))
   {
      d3d->device_lost = FALSE;
      Log("render test ok");
      return S_OK;
   }

   // If the device was lost, do not render until we get it back
   if (hr == D3DERR_DEVICELOST)
      return S_OK;

   // Check if the device needs to be reset.
   if (hr == D3DERR_DEVICENOTRESET)
   {
      if (FAILED(hr = evas_direct3d_reset(d3d)))
         return hr;
   }
   return hr;
}

HRESULT evas_direct3d_reset(Direct3D_DeviceContext *d3d)
{
   Log("reset");
   HRESULT hr = S_OK;

   d3d->scene_rendering = FALSE;

   // Release all video memory objects

   // Reset the device
   if (FAILED(hr = d3d->device->Reset(&d3d->d3dpp)))
   {
      //TrError(Strf::Printf("DXDevice: Reset of the device failed! Error (%X) \"%s\"", 
      //   (DWORD)hr, DXGetErrorStringA(hr)));
      return hr;
   }

   // Store render target surface desc
   LPDIRECT3DSURFACE9 backbuffer;
   d3d->device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
   backbuffer->GetDesc(&d3d->backbuffer_desc);
   backbuffer->Release();

   // Initialize the app's device-dependent objects

   if (FAILED(hr))
   {
      Log("Restoration of device objects failed");
      // Invalidate objects
      
      return E_FAIL;
   }

   Log("Device objects were successfuly restored");
   
   //_device_objects_restored = true;
   return S_OK;
}

void evas_direct3d_line_draw(Direct3D_DeviceContext *d3d, int x1, int y1, int x2, int y2)
{
   Ref<D3DObjectLine> line = scene->GetFreeObject<D3DObjectLine>();
   if (line == NULL)
   {
      line = new D3DObjectLine();
      scene->AddObject(line);
      Log("New line object (total objects: %d)", scene->GetObjectCount());
   }
   else
   {
      line->SetFree(false);
      Log("Line reused (object: %p)", line.Addr());
   }

   line->Setup(
      2.f * float(x1) / float(d3d->width) - 1.f, 
      2.f * (1.f - float(y1) / float(d3d->height)) - 1.f,
      2.f * float(x2) / float(d3d->width) - 1.f, 
      2.f * (1.f - float(y2) / float(d3d->height)) - 1.f,
      0xffff0000);
}

}  // extern "C"
