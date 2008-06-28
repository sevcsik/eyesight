
#include "evas_direct3d_image_cache.h"

#include "evas_direct3d_device.h"

#include <assert.h>

Ref<D3DImageCache> D3DImageCache::_this;

D3DImageCache::D3DImageCache()
{
   _max_width = 512;
   _max_height = 512;
}

D3DImageCache::~D3DImageCache()
{
   Uninitialize();
}

D3DImageCache *D3DImageCache::Current()
{
   if (_this.IsNull())
      _this = new D3DImageCache();
   return _this;
}

void D3DImageCache::SetCurrent(D3DImageCache *obj)
{
   _this = obj;
}

void D3DImageCache::Uninitialize()
{
   for (int i = 0; i < _cache.Length(); i++)
   {
      // In normal case they all will be NULL
      if (_cache[i].texture != NULL)
         _cache[i].texture->Release();
   }
   _cache.Resize();
}

bool D3DImageCache::SelectImageToDevice(D3DDevice *d3d, int id)
{
   if (id < 0 || id >= _cache.Length())
      return false;
   assert(_cache[id].texture != NULL);
   return SUCCEEDED(d3d->GetDevice()->SetTexture(0, _cache[id].texture));
}

void D3DImageCache::RemoveImageUser(int id)
{
   if (id < 0 || id >= _cache.Length())
      return;
   assert(_cache[id].texture != NULL);
   _cache[id].users--;
   if (_cache[id].users == 0)
   {
      _cache[id].texture->Release();
      ZeroMemory(&_cache[id], sizeof(_cache[id]));
   }
}

void D3DImageCache::AddImageUser(int id)
{
   if (id < 0 || id >= _cache.Length())
      return;
   assert(_cache[id].texture != NULL);
   _cache[id].users++;
}

bool D3DImageCache::InsertImage(D3DDevice *d3d, DWORD *data, int w, int h, CacheEntryInfo &info)
{
   CacheEntry *ce = NULL;
   int id = -1;
   for (int i = 0; i < _cache.Length(); i++)
   {
      if (RequestInsert(_cache[i], w, h))
      {
         ce = &_cache[i];
         id = i;
         break;
      }
   }
   if (ce == NULL)
   {
      CacheEntry new_entry;
      if (!CreateEntry(d3d, new_entry, w, h))
         return false;
      for (id = 0; id < _cache.Length(); id++)
      {
         if (_cache[id].texture == NULL)
            break;
      }

      if (id < _cache.Length())
      {
         _cache[id] = new_entry;
         ce = &_cache[id];
      }
      else
      {
         _cache.Add(new_entry);
         ce = _cache.Last();
         id = _cache.Length() - 1;
      }
   }

   assert(ce != NULL && ce->texture != NULL);

   if (!InsertData(*ce, data, w, h))
      return false;

   info.id = id;
   info.u = FLOAT(ce->cur_x) / FLOAT(ce->width);
   info.v = FLOAT(ce->cur_y) / FLOAT(ce->height);
   info.du = FLOAT(w) / FLOAT(ce->width);
   info.dv = FLOAT(h) / FLOAT(ce->height);
   info.width = w;
   info.height = h;

   UpdateInsert(*ce, w, h);
   return true;
}

bool D3DImageCache::RequestInsert(CacheEntry &entry, int w, int h)
{
   // If we already have large image entry
   if (entry.width > _max_width || entry.height > _max_height)
      return false;
   // If requested size does not fit into this entry at all
   if (entry.height - entry.cur_h < h || entry.width < w)
      return false;

   // If requested size does not fit into the current line of the entry
   if (entry.width - entry.cur_x < w)
   {
      entry.cur_y = entry.cur_h;
      entry.cur_x = 0;
      return true;
   }
   
   return true;
}

bool D3DImageCache::CreateEntry(D3DDevice *d3d, CacheEntry &entry, int w, int h)
{
   int width = max(_max_width, w);
   int height = max(_max_height, h);
   HRESULT hr;
   if (FAILED(hr = d3d->GetDevice()->CreateTexture(width, height, 0, 0, D3DFMT_A8R8G8B8, 
      D3DPOOL_MANAGED, &entry.texture, NULL)))
   {
      Log("Failed to create texture: %X", hr);
      return false;
   }

   entry.cur_x = entry.cur_y = entry.cur_h = 0;
   entry.width = width;
   entry.height = height;
   entry.users = 0;
   return true;
}

bool D3DImageCache::InsertData(CacheEntry &entry, DWORD *data, int w, int h)
{
   if (entry.texture == NULL)
      return false;
   assert(data != NULL);

   RECT rc = {entry.cur_x, entry.cur_y, entry.cur_x + w, entry.cur_y + h};
   D3DLOCKED_RECT lr;
   if (FAILED(entry.texture->LockRect(0, &lr, &rc, 0)))
   {
      Log("Failed to lock texture");
      return false;
   }

   for (int i = 0; i < h; i++)
      CopyMemory(((BYTE *)lr.pBits) + i * lr.Pitch, data + i * w, sizeof(DWORD) * w);
   if (FAILED(entry.texture->UnlockRect(0)))
   {
      Log("Failed to unlock texture");
      return false;
   }
   return true;
}

void D3DImageCache::UpdateInsert(CacheEntry &entry, int w, int h)
{
   entry.cur_h = max(entry.cur_h, entry.cur_y + h);
   entry.cur_x += w;
   entry.users++;
}
