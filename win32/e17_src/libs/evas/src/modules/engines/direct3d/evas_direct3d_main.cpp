
#include "evas_engine.h"
#include <assert.h>

#include "evas_direct3d_device.h"
#include "evas_direct3d_context.h"
#include "evas_direct3d_shader_pack.h"
#include "evas_direct3d_scene.h"
#include "evas_direct3d_object_line.h"
#include "evas_direct3d_object_rect.h"

static Ref<D3DScene> scene;
static Ref<D3DContext> context;

extern "C" {

Direct3DDeviceHandler evas_direct3d_init(HWND window, int depth)
{
   D3DDevice *device = new D3DDevice();
   if (!device->Init(window, depth))
      return NULL;
   scene = new D3DScene();
   context = new D3DContext();

   if (!D3DShaderPack::This()->Initialize(device))
   {
      Log("Failed to build shader pack");
      device->Destroy();
      delete device;
      return NULL;
   }

   return (Direct3DDeviceHandler)device;
}

void
evas_direct3d_free(Direct3DDeviceHandler d3d)
{
   context = NULL;
   scene = NULL;
   D3DShaderPack::This()->Uninitialize();

   D3DDevice *device = (D3DDevice *)d3d;
   delete device;

   Log("uninitialized");
}

void         
evas_direct3d_context_color_set(int r, int g, int b, int a)
{
   context->color = ((a & 0xff) << 24) | ((r & 0xff) << 16) |
      ((g & 0xff) << 8) | (b & 0xff);
}

void         
evas_direct3d_render_all(Direct3DDeviceHandler d3d)
{
   Log("render");
   assert(d3d != NULL);
   D3DDevice *device = (D3DDevice *)d3d;

   if (!device->Begin())
      return;

   D3DObjectLine::BeginCache();
   D3DObjectRect::BeginCache();
   scene->DrawAll(device);
   D3DObjectLine::EndCache(device);
   D3DObjectRect::EndCache(device);

   device->End();

   scene->FreeObjects();
}

void evas_direct3d_line_draw(Direct3DDeviceHandler d3d, int x1, int y1, int x2, int y2)
{
   D3DDevice *device = (D3DDevice *)d3d;

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
      2.f * float(x1) / float(device->GetWidth()) - 1.f, 
      2.f * (1.f - float(y1) / float(device->GetHeight())) - 1.f,
      2.f * float(x2) / float(device->GetWidth()) - 1.f, 
      2.f * (1.f - float(y2) / float(device->GetHeight())) - 1.f,
      context->color);
}

void evas_direct3d_rectangle_draw(Direct3DDeviceHandler d3d, int x, int y, int w, int h)
{
   D3DDevice *device = (D3DDevice *)d3d;

   Ref<D3DObjectRect> rect = scene->GetFreeObject<D3DObjectRect>();
   if (rect == NULL)
   {
      rect = new D3DObjectRect();
      scene->AddObject(rect);
      Log("New rect object (total objects: %d)", scene->GetObjectCount());
   }
   else
   {
      rect->SetFree(false);
      Log("Rect reused (object: %p)", rect.Addr());
   }

   rect->Setup(
      2.f * float(x) / float(device->GetWidth()) - 1.f, 
      2.f * (1.f - float(y) / float(device->GetHeight())) - 1.f,
      2.f * float(w) / float(device->GetWidth()), 
      -2.f * float(h) / float(device->GetHeight()),
      context->color);
}

}  // extern "C"
