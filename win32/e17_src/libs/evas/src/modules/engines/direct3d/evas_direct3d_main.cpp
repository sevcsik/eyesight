
#include "evas_engine.h"
#include <assert.h>

#include "evas_direct3d_device.h"
#include "evas_direct3d_context.h"
#include "evas_direct3d_shader_pack.h"
#include "evas_direct3d_scene.h"
#include "evas_direct3d_image_cache.h"
#include "evas_direct3d_object_line.h"
#include "evas_direct3d_object_rect.h"
#include "evas_direct3d_object_image.h"
#include "evas_direct3d_vertex_buffer_cache.h"

// Internal structure that joins two types of objects
struct ImagePtr
{
   Ref<D3DObjectImage> ref;
   RGBA_Image *img;
};

struct DevicePtr
{
   Ref<D3DDevice> device;
   Ref<D3DScene> scene;
   Ref<D3DContext> context;
   Ref<D3DImageCache> image_cache;
   Ref<D3DShaderPack> shader_pack;
   Ref<D3DVertexBufferCache> vb_cache;
};

DevicePtr *SelectDevice(Direct3DDeviceHandler d3d)
{
   DevicePtr *dev_ptr = (DevicePtr *)d3d;
   D3DImageCache::SetCurrent(dev_ptr->image_cache);
   D3DShaderPack::SetCurrent(dev_ptr->shader_pack);
   D3DVertexBufferCache::SetCurrent(dev_ptr->vb_cache);
   return dev_ptr;
}


extern "C" {

Direct3DDeviceHandler evas_direct3d_init(HWND window, int depth)
{
   Ref<D3DDevice> device = new D3DDevice();
   if (!device->Init(window, depth))
      return NULL;

   D3DImageCache::SetCurrent(NULL);
   D3DShaderPack::SetCurrent(NULL);
   D3DVertexBufferCache::SetCurrent(NULL);

   if (!D3DShaderPack::Current()->Initialize(device))
   {
      Log("Failed to build shader pack");
      device->Destroy();
      delete device;
      return NULL;
   }

   DevicePtr *dev_ptr = new DevicePtr;
   dev_ptr->device = device;
   dev_ptr->scene = new D3DScene();
   dev_ptr->context = new D3DContext();
   dev_ptr->image_cache = D3DImageCache::Current();
   dev_ptr->shader_pack = D3DShaderPack::Current();
   dev_ptr->vb_cache = D3DVertexBufferCache::Current();

   return (Direct3DDeviceHandler)dev_ptr;
}

void
evas_direct3d_free(Direct3DDeviceHandler d3d)
{
   DevicePtr *dev_ptr = SelectDevice(d3d);
   dev_ptr->context = NULL;
   dev_ptr->scene = NULL;
   dev_ptr->image_cache = NULL;
   dev_ptr->shader_pack = NULL;
   D3DShaderPack::Current()->Uninitialize();
   D3DImageCache::Current()->Uninitialize();
   D3DVertexBufferCache::Current()->Uninitialize();
   D3DShaderPack::SetCurrent(NULL);
   D3DImageCache::SetCurrent(NULL);
   D3DVertexBufferCache::SetCurrent(NULL);

   dev_ptr->device = NULL;
   delete dev_ptr;

   Log("uninitialized");
}

void         
evas_direct3d_context_color_set(Direct3DDeviceHandler d3d, int r, int g, int b, int a)
{
   DevicePtr *dev_ptr = SelectDevice(d3d);
   dev_ptr->context->color = ((a & 0xff) << 24) | ((r & 0xff) << 16) |
      ((g & 0xff) << 8) | (b & 0xff);
}

void
evas_direct3d_context_set_multiplier(Direct3DDeviceHandler d3d, int r, int g, int b, int a)
{
   DevicePtr *dev_ptr = SelectDevice(d3d);
   dev_ptr->context->color_mul = ((a & 0xff) << 24) | ((r & 0xff) << 16) |
      ((g & 0xff) << 8) | (b & 0xff);
}

void         
evas_direct3d_render_all(Direct3DDeviceHandler d3d)
{
   Log("render");
   assert(d3d != NULL);
   DevicePtr *dev_ptr = SelectDevice(d3d);
   D3DDevice *device = dev_ptr->device;
   D3DScene *scene = dev_ptr->scene;

   if (!device->Begin())
      return;

   device->GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
   device->GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
   device->GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

   D3DObjectLine::BeginCache();
   D3DObjectRect::BeginCache();
   D3DObjectImage::BeginCache();
   scene->DrawAll(device);
   D3DObjectLine::EndCache(device);
   D3DObjectRect::EndCache(device);
   D3DObjectImage::EndCache(device);

   device->End();

   scene->FreeObjects();
}

void evas_direct3d_line_draw(Direct3DDeviceHandler d3d, int x1, int y1, int x2, int y2)
{
   DevicePtr *dev_ptr = SelectDevice(d3d);
   D3DDevice *device = dev_ptr->device;
   D3DScene *scene = dev_ptr->scene;
   D3DContext *context = dev_ptr->context;

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
   DevicePtr *dev_ptr = SelectDevice(d3d);
   D3DDevice *device = dev_ptr->device;
   D3DScene *scene = dev_ptr->scene;
   D3DContext *context = dev_ptr->context;

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

Direct3DImageHandler evas_direct3d_image_load(Direct3DDeviceHandler d3d, 
   const char *file, const char *key, int *error, Evas_Image_Load_Opts *lo)
{
   DevicePtr *dev_ptr = SelectDevice(d3d);
   D3DDevice *device = dev_ptr->device;
   D3DScene *scene = dev_ptr->scene;

   RGBA_Image *evas_image = evas_common_load_image_from_file(file, key, lo);
   if (evas_image == NULL)
   {
      Log("Failed to load image from %s", file);
      return NULL;
   }
   int image_width = evas_image->cache_entry.w;
   int image_height = evas_image->cache_entry.h;
   assert(image_width > 0 && image_height > 0);

   D3DImageCache::CacheEntryInfo info;
   ZeroMemory(&info, sizeof(info));
   info.id = -1;

   Ref<D3DObjectImage> image = new D3DObjectImage();
   image->Init(info.u, info.v, info.du, info.dv, info.id, 
      info.width, info.height, evas_image->cache_entry.file);
   image->SetFree(true);
   scene->AddObject(image);

   Log("New image object (total objects: %d)", scene->GetObjectCount());

   ImagePtr *ptr = new ImagePtr;
   ptr->ref = image;
   ptr->img = evas_image;

   return (Direct3DImageHandler)ptr;
}

void evas_direct3d_image_free(Direct3DDeviceHandler d3d, Direct3DImageHandler image)
{
   DevicePtr *dev_ptr = SelectDevice(d3d);
   D3DDevice *device = dev_ptr->device;
   D3DScene *scene = dev_ptr->scene;

   ImagePtr *ptr = (ImagePtr *)image;

   Ref<D3DObjectImage> image_ref = ptr->ref;
   assert(!image_ref.IsNull());
   scene->DeleteObject(image_ref);

   delete ptr;
}

void evas_direct3d_image_draw(Direct3DDeviceHandler d3d, Direct3DImageHandler image,
   int src_x, int src_y, int src_w, int src_h, 
   int dst_x, int dst_y, int dst_w, int dst_h, int smooth)
{
   ImagePtr *ptr = (ImagePtr *)image;
   Ref<D3DObjectImage> image_ref = ptr->ref;
   RGBA_Image *evas_image = ptr->img;
   DevicePtr *dev_ptr = SelectDevice(d3d);
   D3DDevice *device = dev_ptr->device;
   D3DScene *scene = dev_ptr->scene;
   assert(!image_ref.IsNull());
   if (image_ref.IsNull())
      return;

   if (!image_ref->IsValid())
   {
      TArray<D3DObjectImage *> images;
      scene->GetObjectsOfType<D3DObjectImage>(images);
      bool found = false;
      for (int i = 0; i < images.Length(); i++)
      {
         if (images[i]->IsValid() && 
            _stricmp(images[i]->GetSource(), image_ref->GetSource()) == 0)
         {
            images[i]->CopyTo(image_ref);
            found = true;
            Log("Image object info reused, source: \"%s\"", image_ref->GetSource());
            break;
         }
      }
      if (!found)
         evas_cache_image_load_data(&evas_image->cache_entry);
   }

   // If the image object wasnt initialized yet
   if (evas_image->image.data != NULL && !image_ref->IsValid())
   {
      D3DImageCache::CacheEntryInfo info;
      ZeroMemory(&info, sizeof(info));
      if (!D3DImageCache::Current()->InsertImage(device, (DWORD *)evas_image->image.data,
         evas_image->cache_entry.w, evas_image->cache_entry.h, info))
      {
         Log("Couldnt add image to the cache");
         return;
      }
      image_ref->Init(info.u, info.v, info.du, info.dv, info.id, 
         info.width, info.height, evas_image->cache_entry.file);
   }

   // Do not draw invalid objects
   if (!image_ref->IsValid())
   {
      image_ref->SetFree(true);
      return;
   }

   image_ref->Setup(
      2.f * float(dst_x) / float(device->GetWidth()) - 1.f, 
      2.f * (1.f - float(dst_y) / float(device->GetHeight())) - 1.f,
      2.f * float(dst_w) / float(device->GetWidth()), 
      -2.f * float(dst_h) / float(device->GetHeight()),
      src_x, src_y, src_w, src_h);

   image_ref->SetupColorFilter(dev_ptr->context->color_mul);

   image_ref->SetFree(false);
}

void evas_direct3d_image_size_get(Direct3DImageHandler image, int *w, int *h)
{
   ImagePtr *ptr = (ImagePtr *)image;
   if (ptr == NULL)
      return;
   if (w != NULL) 
      *w = ptr->img->cache_entry.w;
   if (h != NULL)
      *h = ptr->img->cache_entry.h;
}

void evas_direct3d_image_border_set(Direct3DDeviceHandler d3d, Direct3DImageHandler image, 
   int l, int r, int t, int b)
{
   ImagePtr *ptr = (ImagePtr *)image;
   Ref<D3DObjectImage> image_ref = ptr->ref;
   DevicePtr *dev_ptr = SelectDevice(d3d);
   D3DDevice *device = dev_ptr->device;
   if (image_ref.IsNull())
      return;

   image_ref->SetupBorder(
      D3DXVECTOR4(
         2.f * float(l) / float(device->GetWidth()), 
         -2.f * float(t) / float(device->GetHeight()),
         2.f * float(r) / float(device->GetWidth()), 
         -2.f * float(b) / float(device->GetHeight())),
      D3DXVECTOR4(float(l) / float(ptr->img->cache_entry.w), 
         float(t) / float(ptr->img->cache_entry.h),
         float(r) / float(ptr->img->cache_entry.w), 
         float(b) / float(ptr->img->cache_entry.h)));
}

void evas_direct3d_image_border_get(Direct3DDeviceHandler d3d, Direct3DImageHandler image, 
   int *l, int *r, int *t, int *b)
{
   ImagePtr *ptr = (ImagePtr *)image;
   Ref<D3DObjectImage> image_ref = ptr->ref;
   DevicePtr *dev_ptr = SelectDevice(d3d);
   D3DDevice *device = dev_ptr->device;
   if (image_ref.IsNull())
      return;
   assert(l != NULL && r != NULL && b != NULL && t != NULL);
   *l = 0.5f * image_ref->GetBorderLeft() * device->GetWidth();
   *r = 0.5f * image_ref->GetBorderRight() * device->GetWidth();
   *t = -0.5f * image_ref->GetBorderTop() * device->GetHeight();
   *b = -0.5f * image_ref->GetBorderBottom() * device->GetHeight();
}



}  // extern "C"
