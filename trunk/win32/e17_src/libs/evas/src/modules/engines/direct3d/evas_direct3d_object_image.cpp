
#include "evas_direct3d_object_image.h"
#include "evas_direct3d_image_cache.h"
#include "evas_direct3d_device.h"
#include "evas_direct3d_shader_pack.h"

TArray<D3DObjectImage *> D3DObjectImage::_cache;
bool D3DObjectImage::_cache_enabled = false;

D3DObjectImage::D3DObjectImage()
{
   _x = _y = _w = _h = 0;
   _sx = _sy = _sw = _sh = 0;
   _u = _v = _du = _dv = 0;
   _image_id = -1;
   _width = _height = 0;
   _source[0] = 0;
}

void D3DObjectImage::CopyTo(D3DObjectImage *image)
{
   assert(image != NULL);
   image->_x = _x;
   image->_y = _y;
   image->_w = _w;
   image->_h = _h;
   image->_sx = _sx;
   image->_sy = _sy;
   image->_sw = _sw;
   image->_sh = _sh;
   image->_u = _u;
   image->_v = _v;
   image->_du = _du;
   image->_dv = _dv;
   image->_image_id = _image_id;
   image->_width = _width;
   image->_height = _height;
   CopyMemory(image->_source, _source, sizeof(_source));
}

void D3DObjectImage::BeginCache()
{
   _cache.Allocate(0);
   _cache_enabled = true;
}

void D3DObjectImage::EndCache(D3DDevice *d3d)
{
   if (!_cache_enabled)
      return;
   D3DShaderPack::Current()->SetVDecl(d3d, D3DShaderPack::VDECL_XYUV);
   D3DShaderPack::Current()->SetVS(d3d, D3DShaderPack::VS_COPY_UV);
   D3DShaderPack::Current()->SetPS(d3d, D3DShaderPack::PS_TEX);

   struct GroupDesc
   {
      int num;
      int id;
   };

   static TArray<Vertex> sorted;
   static TArray<GroupDesc> groups;
   sorted.Allocate(0);
   groups.Allocate(0);

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
            Vertex *data = _cache[i]->MakeData();
            sorted.Add(data[0]);
            sorted.Add(data[1]);
            sorted.Add(data[2]);
            sorted.Add(data[3]);
            sorted.Add(data[4]);
            sorted.Add(data[5]);
         }
         _cache[i]->_image_id = -_cache[i]->_image_id - 1;
         num++;
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

   for (int i = 0, cur = 0; i < groups.Length(); i++)
   {
      D3DImageCache::Current()->SelectImageToDevice(d3d, groups[i].id);
      d3d->GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST, groups[i].num * 2, 
         &sorted[cur], sizeof(Vertex));
      cur += groups[i].num * 6;
   }

   Log("Image cache drawn: %d items, %d groups", _cache.Length(), groups.Length());
}

void D3DObjectImage::Draw(D3DDevice *d3d)
{
   if (_cache_enabled)
   {
      _cache.Add(this);
      return;
   }

   D3DShaderPack::Current()->SetVDecl(d3d, D3DShaderPack::VDECL_XYUV);
   D3DShaderPack::Current()->SetVS(d3d, D3DShaderPack::VS_COPY_UV);
   D3DShaderPack::Current()->SetPS(d3d, D3DShaderPack::PS_TEX);

   d3d->GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, MakeData(), sizeof(Vertex));
}

void D3DObjectImage::Init(FLOAT u, FLOAT v, FLOAT du, FLOAT dv, 
   int image_id, int width, int height, const char *source)
{
   _u = u;
   _v = v;
   _du = du;
   _dv = dv;
   _image_id = image_id;
   _width = width;
   _height = height;
   strncpy_s(_source, sizeof(_source), source, sizeof(_source) - 1);
}

void D3DObjectImage::Setup(FLOAT x, FLOAT y, FLOAT w, FLOAT h, 
   int sx, int sy, int sw, int sh)
{
   _x = x;
   _y = y;
   _w = w;
   _h = h;
   _sx = FLOAT(sx) / FLOAT(_width);
   _sy = FLOAT(sy) / FLOAT(_height);
   _sw = FLOAT(sw) / FLOAT(_width);
   _sh = FLOAT(sh) / FLOAT(_height);
   if (_sw < 0.f)
      _sw = 1.f;
   if (_sh < 0.f)
      _sh = 1.f;
}

D3DObjectImage::Vertex *D3DObjectImage::MakeData()
{
   Vertex data[6] = {
      {_x, _y,             _u + _sx * _du, _v + _sy * _dv}, 
      {_x + _w, _y,        _u + (_sx + _sw) * _du, _v + _sy * _dv}, 
      {_x, _y + _h,        _u + _sx * _du, _v + (_sy + _sh) * _dv},
      {_x, _y + _h,        _u + _sx * _du, _v + (_sy + _sh) * _dv}, 
      {_x + _w, _y,        _u + (_sx + _sw) * _du, _v + _sy * _dv}, 
      {_x + _w, _y + _h,   _u + (_sx + _sw) * _du, _v + (_sy + _sh) * _dv}};
   CopyMemory(_data, data, sizeof(data));
   return _data;
}
