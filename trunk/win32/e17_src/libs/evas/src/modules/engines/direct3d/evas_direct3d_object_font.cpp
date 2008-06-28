
#define ENABLE_LOG_PRINTF

#include <string.h>

#include "evas_direct3d_object_font.h"
#include "evas_direct3d_image_cache.h"
#include "evas_direct3d_device.h"
#include "evas_direct3d_shader_pack.h"
#include "evas_direct3d_vertex_buffer_cache.h"

TArray<D3DObjectFont *> D3DObjectFont::_cache;
bool D3DObjectFont::_cache_enabled = false;

D3DObjectFont::D3DObjectFont()
{
   _x = _y = _w = _h = _ow = _oh = 0;
   _u = _v = _du = _dv = 0;
   _image_id = -1;
   _color = 0xff000000;
   _name[0] = 0;
   _size = 0;
}

D3DObjectFont::~D3DObjectFont()
{
   D3DImageCache::Current()->RemoveImageUser(_image_id);
}

void D3DObjectFont::CopyTo(D3DObjectFont *image)
{
   assert(image != NULL);
   image->_image_id = _image_id;
   strncpy(image->_name, _name, sizeof(image->_name) - 1);
   image->_size = _size;
   image->_u = _u;
   image->_v = _v;
   image->_du = _du;
   image->_dv = _dv;
   _char_size.CopyTo(image->_char_size);
   D3DImageCache::Current()->AddImageUser(image->_image_id);
}

void D3DObjectFont::BeginCache()
{
   _cache.Allocate(0);
   _cache_enabled = true;
}

void D3DObjectFont::EndCache(D3DDevice *d3d)
{
   if (!_cache_enabled || _cache.Length() == 0)
      return;
   D3DShaderPack::Current()->SetVDecl(d3d, D3DShaderPack::VDECL_XYUVC);
   D3DShaderPack::Current()->SetVS(d3d, D3DShaderPack::VS_COPY_UV_COLOR);
   D3DShaderPack::Current()->SetPS(d3d, D3DShaderPack::PS_TEX_COLOR_FILTER);

   struct GroupDesc
   {
      int num;
      int id;
   };

   static TArray<Vertex> sorted;
   static TArray<GroupDesc> groups;
   sorted.Allocate(0);
   groups.Allocate(0);

#if 0
   bool found = true;
   while (found)
   {
      found = false;
      int cur_id = -1;
      int num = 0;
      for (int i = 0; i < _cache.Length(); i++)
      {
         // We have processed this
         if (_cache[i]->_image_id < 0)
            continue;
         found = true;
         if (cur_id < 0)
            cur_id = _cache[i]->_image_id;
         if (_cache[i]->_image_id == cur_id)
         {
            if (!_cache[i]->_with_border)
            {
               Vertex *data = _cache[i]->MakeData();
               sorted.Add(data[0]);
               sorted.Add(data[1]);
               sorted.Add(data[2]);
               sorted.Add(data[3]);
               sorted.Add(data[4]);
               sorted.Add(data[5]);
               _cache[i]->_image_id = -_cache[i]->_image_id - 1;
               num++;
            }
            else
            {
               Vertex *data = _cache[i]->MakeDataBorder();
               int last_len = sorted.Length();
               sorted.Allocate(last_len + 6 * 9);
               CopyMemory(&sorted[last_len], data, sizeof(Vertex) * 6 * 9);
               _cache[i]->_image_id = -_cache[i]->_image_id - 1;
               num += 9;
            }
         }
      }
      if (num > 0)
      {
         GroupDesc gd = {num, cur_id};
         groups.Add(gd);
      }
   }

   // Restore ids
   for (int i = 0; i < _cache.Length(); i++)
      _cache[i]->_image_id = -_cache[i]->_image_id - 1;

   D3DVertexBufferCache::CacheEntryInfo ce_info;
   if (!D3DVertexBufferCache::Current()->InitBuffer(d3d, (BYTE *)sorted.Data(), 
      sorted.Length() * sizeof(Vertex), ce_info))
   {
      return;
   }
   D3DVertexBufferCache::Current()->SelectBufferToDevice(d3d, ce_info.id, sizeof(Vertex));

   for (int i = 0, cur = 0; i < groups.Length(); i++)
   {
      D3DImageCache::Current()->SelectImageToDevice(d3d, groups[i].id);
//      d3d->GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST, groups[i].num * 2, 
//         &sorted[cur], sizeof(Vertex));
      d3d->GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, cur, groups[i].num * 2);
      cur += groups[i].num * 6;
   }
#endif

   Log("Font cache drawn: %d items, %d groups", _cache.Length(), groups.Length());
   _cache_enabled = false;
}

void D3DObjectFont::Draw(D3DDevice *d3d)
{
   if (_cache_enabled)
   {
      _cache.Add(this);
      return;
   }

   D3DShaderPack::Current()->SetVDecl(d3d, D3DShaderPack::VDECL_XYUVC);
   D3DShaderPack::Current()->SetVS(d3d, D3DShaderPack::VS_COPY_UV_COLOR);
   D3DShaderPack::Current()->SetPS(d3d, D3DShaderPack::PS_TEX_COLOR_FILTER);
   D3DImageCache::Current()->SelectImageToDevice(d3d, _image_id);

   static TArray<Vertex> data;
   data.Allocate(0);
   MakeData(d3d, data);

   d3d->GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST, data.Length() / 3, 
      data.Data(), sizeof(Vertex));
}

bool D3DObjectFont::Init(D3DDevice *d3d, const char *name, int size)
{
   LPDIRECT3DDEVICE9 dev = d3d->GetDevice();
   LPDIRECT3DTEXTURE9 tex = NULL;
   int width = size * 16;
   int height = size * 16;
   if (FAILED(dev->CreateTexture(width, height, 0, 0, D3DFMT_X8R8G8B8, 
      D3DPOOL_SYSTEMMEM, &tex, NULL)))
   {
      Log("Failed to create aux font texture");
      return false;
   }

   LPDIRECT3DSURFACE9 surf = NULL;
   if (FAILED(tex->GetSurfaceLevel(0, &surf)))
   {
      Log("Failed to get surface");
      tex->Release();
      return false;
   }

   HDC hdc;
   if (FAILED(surf->GetDC(&hdc)))
   {
      Log("Failed to get hdc");
      surf->Release();
      tex->Release();
      return false;
   }

   SetTextColor(hdc, 0xffff0000);
   SetBkColor(hdc, 0xff00ff00);
   SetBkMode(hdc, OPAQUE);

   int ch_extra = GetTextCharacterExtra(hdc);
   _char_size.Resize(256);

   for (int i = 0; i < 16; i++)
   {
      for (int j = 0; j < 16; j++)
      {
         char ch[2] = {i * 16 + j, 0};
         TextOutA(hdc, j * size, i * size, ch, 1);

         SIZE size;
         GetTextExtentPoint32A(hdc, ch, 1, &size);
         size.cx += ch_extra;
         D3DXVECTOR2 sz(2.f * size.cx / d3d->GetWidth(), 2.f * size.cy / d3d->GetHeight());
         _char_size.Add(sz);
      }
   }

   surf->ReleaseDC(hdc);
   surf->Release();
   hdc = NULL;
   surf = NULL;

   TArray<DWORD> data;
   data.Allocate(width * height);
   D3DLOCKED_RECT rect;
   if (FAILED(tex->LockRect(0, &rect, NULL, D3DLOCK_READONLY)))
   {
      Log("Failed to lock texture");
      tex->Release();
      return false;
   }

   for (int i = 0; i < height; i++)
      CopyMemory(&data[i * width], (BYTE *)rect.pBits + i * rect.Pitch, sizeof(DWORD) * width);

   tex->UnlockRect(0);
   tex->Release();
   tex = NULL;

   D3DImageCache::CacheEntryInfo info;
   if (!D3DImageCache::Current()->InsertImage(d3d, data.Data(), width, height, info))
   {
      Log("Failed to insert font image data into cache");
      return false;
   }
   _image_id = info.id;
   _u = info.u;
   _v = info.v;
   _du = info.du;
   _dv = info.dv;
   return true;
}

void D3DObjectFont::Setup(FLOAT x, FLOAT y, FLOAT w, FLOAT h, FLOAT ow, FLOAT oh, 
   const char *text)
{
   _x = x;
   _y = y;
   _w = w;
   _h = h;
   _ow = ow;
   _oh = oh;

   _text_uvs.Allocate(0);

   int len = strlen(text);
   for (int i = 0; i < len; i++)
   {
      int ch = text[i];
      FLOAT pix = FLOAT(ch % 16) / 16.f;
      FLOAT piy = FLOAT(ch / 16) / 16.f;

      TextChar tc = {D3DXVECTOR2(_u + pix * _du, _v + piy * _dv), ch};
      _text_uvs.Add(tc);
   }
}

void D3DObjectFont::MakeData(D3DDevice *d3d, TArray<Vertex> &data)
{
   Vertex vx;
   FLOAT shift_x = 0;

   const D3DXVECTOR2 tsize(_du / 16.f, _dv / 16.f);
   const D3DXVECTOR2 psize(2.f * FLOAT(_size) / d3d->GetWidth(), 2.f * FLOAT(_size) / d3d->GetHeight());

   for (int i = 0; i < _text_uvs.Length(); i++)
   {
      const D3DXVECTOR2 &size = _char_size[_text_uvs[i].ch];
      const D3DXVECTOR2 &base_uv = _text_uvs[i].uv;
      const D3DXVECTOR2 ssize(size.x / psize.x, size.y / psize.y);
      
      vx.col = 0xffffffff;
      vx.x = _x + shift_x;
      vx.y = _y;
      vx.u = base_uv.x;
      vx.v = base_uv.y;
      data.Add(vx);
      vx.x = _x + shift_x + size.x;
      vx.y = _y;
      vx.u = base_uv.x + tsize.x * ssize.x;
      vx.v = base_uv.y;
      data.Add(vx);
      vx.x = _x + shift_x;
      vx.y = _y - size.y;
      vx.u = base_uv.x;
      vx.v = base_uv.y + tsize.y * ssize.y;
      data.Add(vx);
      vx.x = _x + shift_x;
      vx.y = _y - size.y;
      vx.u = base_uv.x;
      vx.v = base_uv.y + tsize.y * ssize.y;
      data.Add(vx);
      vx.x = _x + shift_x + size.x;
      vx.y = _y;
      vx.u = base_uv.x + tsize.x * ssize.x;
      vx.v = base_uv.y;
      data.Add(vx);
      vx.x = _x + shift_x + size.x;
      vx.y = _y - size.y;
      vx.u = base_uv.x + tsize.x * ssize.x;
      vx.v = base_uv.y + tsize.y * ssize.y;
      data.Add(vx);

      shift_x += size.x;
   }
}
