#ifndef __EVAS_DIRECT3D_IMAGE_CACHE_H__
#define __EVAS_DIRECT3D_IMAGE_CACHE_H__

#include "evas_engine.h"

#include "ref.h"
#include "array.h"

class D3DDevice;

class D3DImageCache : virtual public Referenc
{
public:
   struct CacheEntryInfo
   {
      int id;
      int width, height;
      FLOAT u, v;
      FLOAT du, dv;
   };

public:
   ~D3DImageCache();

   static D3DImageCache *This();

   inline void SetMaxSize(int w, int h);

   bool InsertImage(D3DDevice *d3d, DWORD *data, int w, int h, CacheEntryInfo &info);
   bool SelectImageToDevice(D3DDevice *d3d, int id);
   void Uninitialize();

private:
   struct CacheEntry
   {
      LPDIRECT3DTEXTURE9 texture;
      int width;
      int height;
      int cur_x;
      int cur_y;
      int cur_h;
   };

private:
   D3DImageCache();

   bool RequestInsert(CacheEntry &entry, int w, int h);
   bool CreateEntry(D3DDevice *d3d, CacheEntry &entry, int w, int h);
   bool InsertData(CacheEntry &entry, DWORD *data, int w, int h);
   void UpdateInsert(CacheEntry &entry, int w, int h);

private:
   TArray<CacheEntry> _cache;
   int _max_width;
   int _max_height;

   static Ref<D3DImageCache> _this;
};

void D3DImageCache::SetMaxSize(int w, int h)
{
   _max_width = w;
   _max_height = h;
}

#endif  // __EVAS_DIRECT3D_IMAGE_CACHE_H__
