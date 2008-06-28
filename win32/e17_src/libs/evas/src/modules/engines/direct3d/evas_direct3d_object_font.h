#ifndef __EVAS_DIRECT3D_OBJECT_FONT_H__
#define __EVAS_DIRECT3D_OBJECT_FONT_H__

#include "evas_engine.h"

#include "ref.h"
#include "array.h"

#include "evas_direct3d_object.h"

class D3DObjectFont : public D3DObject
{
public:
   D3DObjectFont();
   ~D3DObjectFont();

   inline bool Compare(const char *name, int size);
   void CopyTo(D3DObjectFont *image);

   static void BeginCache();
   virtual void Draw(D3DDevice *d3d);
   static void EndCache(D3DDevice *d3d);

   bool Init(D3DDevice *d3d, const char *name, int size);
   void Setup(FLOAT x, FLOAT y, FLOAT w, FLOAT h, FLOAT ow, FLOAT oh, const char *text);

private:
   struct Vertex
   {
      FLOAT x, y;
      FLOAT u, v;
      DWORD col;
   };

   struct TextChar
   {
      D3DXVECTOR2 uv;
      char ch;
   };

private:
   void MakeData(D3DDevice *d3d, TArray<Vertex> &data);

private:
   FLOAT _x, _y, _w, _h, _ow, _oh;
   DWORD _color;

   char _name[128];
   int _size;

   int _image_id;
   FLOAT _u, _v, _du, _dv;
   TArray<D3DXVECTOR2> _char_size;

   TArray<TextChar> _text_uvs;

   static TArray<D3DObjectFont *> _cache;
   static bool _cache_enabled;
};

bool D3DObjectFont::Compare(const char *name, int size)
{
   return (strcmp(name, _name) == 0) && (size == _size);
}

#endif  // __EVAS_DIRECT3D_OBJECT_FONT_H__
