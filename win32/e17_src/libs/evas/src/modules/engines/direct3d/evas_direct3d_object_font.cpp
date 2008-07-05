
#define ENABLE_LOG_PRINTF

#include <string.h>

#include "evas_direct3d_object_font.h"
#include "evas_direct3d_image_cache.h"
#include "evas_direct3d_device.h"
#include "evas_direct3d_shader_pack.h"
#include "evas_direct3d_vertex_buffer_cache.h"

D3DObjectFont::Cache D3DObjectFont::_cache;

D3DObjectFont::D3DObjectFont(void *source, int image_id)
{
   _image_id = image_id;
   _color = 0xff000000;
   _source = source;
   D3DImageCache::Current()->AddImageUser(_image_id);
}

D3DObjectFont::~D3DObjectFont()
{
   D3DImageCache::Current()->RemoveImageUser(_image_id);
}

void D3DObjectFont::CopyTo(D3DObjectFont *font)
{
   assert(font != NULL);
   font->_image_id = _image_id;
   font->_source = _source;
   D3DImageCache::Current()->AddImageUser(font->_image_id);
   _glyphs.CopyTo(font->_glyphs);
}

void D3DObjectFont::BeginCache(int image_id)
{
   if (_cache.enabled)
      return;
   int w = D3DImageCache::Current()->GetImageWidth(image_id);
   int h = D3DImageCache::Current()->GetImageHeight(image_id);
   _cache.enabled = true;
   _cache.image_id = image_id;
   _cache.data.Allocate(w * h);
   _cache.data.Set(0x00000000);
   _cache.width = w;
   _cache.height = h;
}

void D3DObjectFont::Draw(D3DDevice *d3d)
{
   assert(_cache.image_id == _image_id);
}

void D3DObjectFont::EndCache(D3DDevice *d3d)
{
   if (!_cache.enabled)
      return;
   _cache.enabled = false;

   if (_cache.data.Length() == 0)
      return;

   D3DImageCache::CacheEntryInfo info;
   ZeroMemory(&info, sizeof(info));
   info.id = _cache.image_id;
   info.width = _cache.width;
   info.height = _cache.height;
   if (!D3DImageCache::Current()->UpdateImageData(info, _cache.data.Data()))
      return;

   D3DShaderPack::Current()->SetVDecl(d3d, D3DShaderPack::VDECL_XYUV);
   D3DShaderPack::Current()->SetVS(d3d, D3DShaderPack::VS_COPY_UV);
   D3DShaderPack::Current()->SetPS(d3d, D3DShaderPack::PS_TEX);
   D3DImageCache::Current()->SelectImageToDevice(d3d, _cache.image_id);

   static const Vertex data[6] = {
      {-1, -1,    0, 1}, 
      {-1,  1,    0, 0},
      { 1, -1,    1, 1}, 
      { 1, -1,    1, 1}, 
      {-1,  1,    0, 0}, 
      { 1,  1,    1, 0}};

   d3d->GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, data, sizeof(Vertex));
}

D3DObjectFont::Glyph *D3DObjectFont::GetGlyph(void *source)
{
   if (_image_id < 0)
   {
      Log("Font is not initialized");
      return NULL;
   }
   for (int i = 0; i < _glyphs.Length(); i++)
   {
      if (_glyphs[i]->Compare(source))
         return _glyphs[i];
   }
   return NULL;
}

D3DObjectFont::Glyph *D3DObjectFont::AddGlyph(D3DDevice *d3d, void *source, 
   BYTE *data8, int width, int height, int pitch)
{
   if (_image_id < 0)
   {
      Log("Font is not initialized");
      return NULL;
   }
   for (int i = 0; i < _glyphs.Length(); i++)
   {
      if (_glyphs[i]->Compare(source))
         return _glyphs[i];
   }
   Ref<Glyph> glyph = new Glyph(source);
   glyph->_data.Allocate(width * height);
   glyph->_width = width;
   glyph->_height = height;

   for (int i = 0; i < height; i++)
      CopyMemory(&glyph->_data[i * width], &data8[i * pitch], width);

   Log("Glyph added (%p) (%dx%d)", source, width, height);
   _glyphs.Add(glyph);
   return _glyphs.Last()->Addr();
}

void D3DObjectFont::PushForDraw(Glyph *glyph, int x, int y)
{
   BeginCache(_image_id);

   // Uff, I'm not sure about multiple windows...

#define LERP(a, b, t1, t2)  (BYTE)(FLOAT(a) * (t1) + FLOAT(b) * (t2))

   Color dc, sc, color;
   FLOAT a;
   color.color = _color;
   const FLOAT color_alpha = color.Alpha();

   DWORD *data = _cache.data.Data();
   BYTE *gdata = glyph->_data.Data();

   for (int i = 0; i < glyph->_height && y + i < _cache.height; i++)
   {
      if (y + i < 0)
         continue;
      DWORD *dst = data + ((y + i) * _cache.width + x);
      BYTE *src = gdata + (i * glyph->_width);
      for (int j = 0; j < glyph->_width && x + j < _cache.width; j++, dst++, src++)
      {
         if (x + j < 0)
            continue;
         if (*src == 0)
            continue;
         if (*src == 0xff && color.a == 0xff)
         {
            *dst = color.color;
            continue;
         }
         sc.a = *src;
         
         a = sc.Alpha() * color_alpha;
         if (*dst == 0)
         {
            *dst = (BYTE(255.f * a) << 24) | (0x00ffffff & color.color);
            continue;
         }

         dc.color = *dst;
         sc.r = color.r;
         sc.g = color.g;
         sc.b = color.b;
         dc.r = LERP(dc.r, sc.r, 1 - a, a);
         dc.g = LERP(dc.g, sc.g, 1 - a, a);
         dc.b = LERP(dc.b, sc.b, 1 - a, a);
         dc.a = max(dc.a, sc.a);
         *dst = dc.color;
      }
   }
}
