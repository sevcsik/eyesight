
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

static Ref<D3DScene> scene;
static Ref<D3DContext> context;

// Internal structure that joins two types of objects
struct ImagePtr
{
   Ref<D3DObjectImage> ref;
   RGBA_Image *img;
};

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
   D3DImageCache::This()->Uninitialize();

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

Direct3DImageHandler evas_direct3d_image_load(Direct3DDeviceHandler d3d, 
   const char *file, const char *key, int *error, Evas_Image_Load_Opts *lo)
{
   D3DDevice *device = (D3DDevice *)d3d;
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
   
   // Just in case if loading function will load data indeed one day
   if (evas_image->image.data != NULL)
   {
      if (!D3DImageCache::This()->InsertImage(device, (DWORD *)evas_image->image.data,
         image_width, image_height, info))
      {
         Log("Couldnt add image to the cache");
         return NULL;
      }
   }
   else
      Log("Image data was not loaded yet");

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
   ImagePtr *ptr = (ImagePtr *)image;

   Ref<D3DObjectImage> image_ref = ptr->ref;
   assert(!image_ref.IsNull());
   scene->DeleteObject(image_ref);

   delete ptr;

   // Warning: image data still in the cache, what will we do with it?
}

void evas_direct3d_image_draw(Direct3DDeviceHandler d3d, Direct3DImageHandler image,
   int src_x, int src_y, int src_w, int src_h, 
   int dst_x, int dst_y, int dst_w, int dst_h, int smooth)
{
   ImagePtr *ptr = (ImagePtr *)image;
   Ref<D3DObjectImage> image_ref = ptr->ref;
   RGBA_Image *evas_image = ptr->img;
   D3DDevice *device = (D3DDevice *)d3d;
   assert(!image_ref.IsNull());

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
      if (!D3DImageCache::This()->InsertImage(device, (DWORD *)evas_image->image.data,
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
      0, 0, -1, -1);
//      src_x, src_y, src_w, src_h);
   image_ref->SetFree(false);
}


}  // extern "C"
