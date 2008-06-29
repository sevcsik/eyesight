
#include "evas_direct3d_device.h"

D3DDevice::D3DDevice()
{
   Reset();
}

bool D3DDevice::Init(HWND window, int depth)
{
   D3DPRESENT_PARAMETERS pp;
   D3DDISPLAYMODE dm;
   D3DCAPS9 caps;
   RECT rect;
   DWORD flag;
   HRESULT hr;
   
   if (window == NULL)
      return false;

   Destroy();
   
   _object = Direct3DCreate9(D3D_SDK_VERSION);
   if (_object == NULL)
     return false;

   if (FAILED(hr = _object->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dm)))
     {
     Log("GetAdapterDisplayMode failed: %x", hr);
     Destroy();
     return false;
     }

   if (FAILED(hr = _object->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
     {
     Log("GetDeviceCaps failed: %x", hr);
     Destroy();
     return false;
     }

   if (!GetClientRect(window, &rect))
     {
     Log("GetClientRect failed: %x", GetLastError());
     Destroy();
     return false;
     }

   if (SUCCEEDED(_object->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
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
   //pp.EnableAutoDepthStencil = TRUE;
   //pp.AutoDepthStencilFormat = D3DFMT_D16;
   pp.FullScreen_RefreshRateInHz = 0;
   pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

   if (FAILED(hr = _object->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
     window, flag, &pp, &_device)))
     {
     Log("CreateDevice failed: %x", hr);
     Destroy();
     return false;
     }

   LPDIRECT3DSURFACE9 backbuffer = NULL;
   _device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
   backbuffer->GetDesc(&_backbuffer_desc);
   backbuffer->Release();

   switch (dm.Format) {
   case D3DFMT_A8R8G8B8:
   case D3DFMT_X8R8G8B8:
     _depth = 32;
     break;
   case D3DFMT_R5G6B5:
     _depth = 16;
     break;
   default:
     Log("No supported format found");
     Destroy();
     return false;
   }

   _d3dpp = pp;
   _device_lost = FALSE;
   _scene_rendering = FALSE;
   _width = rect.right - rect.left;
   _height = rect.bottom - rect.top;

   Log("initialized");
   return true;
}

void D3DDevice::Destroy()
{
   if (_device != NULL)
      _device->Release();
   if (_object != NULL)
      _object->Release();
   Reset();

   Log("uninitialized");
}

void D3DDevice::Reset()
{
   _window = NULL;
   _object = NULL;
   _device = NULL;
   _width = 0;
   _height = 0;
   _rot = 0;
   _depth = 0;
   _device_lost = false;
   _scene_rendering = false;
   ZeroMemory(&_d3dpp, sizeof(_d3dpp));
   ZeroMemory(&_backbuffer_desc, sizeof(_backbuffer_desc));
}

HRESULT D3DDevice::RestoreDevice()
{
   Log("restore");
   assert(_device != NULL);

   HRESULT hr = S_OK;

   // Test the cooperative level to see if it's okay to render
   if (SUCCEEDED(hr = _device->TestCooperativeLevel()))
   {
      _device_lost = FALSE;
      Log("render test ok");
      return S_OK;
   }

   // If the device was lost, do not render until we get it back
   if (hr == D3DERR_DEVICELOST)
      return S_OK;

   // Check if the device needs to be reset.
   if (hr == D3DERR_DEVICENOTRESET)
   {
      if (FAILED(hr = ResetDevice()))
         return hr;
   }
   return hr;
}

HRESULT D3DDevice::ResetDevice()
{
   Log("reset");
   HRESULT hr = S_OK;

   _scene_rendering = FALSE;

   // Release all video memory objects

   // Reset the device
   if (FAILED(hr = _device->Reset(&_d3dpp)))
   {
      //TrError(Strf::Printf("D3DDevice: Reset of the device failed! Error (%X) \"%s\"", 
      //   (DWORD)hr, DXGetErrorStringA(hr)));
      return hr;
   }

   // Store render target surface desc
   LPDIRECT3DSURFACE9 backbuffer;
   _device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
   backbuffer->GetDesc(&_backbuffer_desc);
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

bool D3DDevice::Begin()
{
   RestoreDevice();
   
   if (FAILED(_device->BeginScene()))
      return false;

   //static const D3DVIEWPORT9 vp = {0, 0, _width, _height, 0.f, 1.f};
   //_device->SetViewport(&vp);
   //_device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

   //_device->Clear(0, NULL, D3DCLEAR_TARGET /*| D3DCLEAR_ZBUFFER*/, 0xffdddddd, 1.f, 0);
   return true;
}

bool D3DDevice::End()
{
   _device->EndScene();
   _device->Present(NULL, NULL, NULL, NULL);
   return true;
}
