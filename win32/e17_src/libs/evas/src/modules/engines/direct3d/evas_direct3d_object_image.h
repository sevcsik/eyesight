#ifndef __EVAS_DIRECT3D_OBJECT_IMAGE_H__
#define __EVAS_DIRECT3D_OBJECT_IMAGE_H__

#include "evas_engine.h"

#include "ref.h"
#include "array.h"

#include "evas_direct3d_object.h"

class D3DObjectImage : public D3DObject
{
public:
   D3DObjectImage();

   void CopyTo(D3DObjectImage *image);

   static void BeginCache();
   virtual void Draw(D3DDevice *d3d);
   static void EndCache(D3DDevice *d3d);

   void Init(FLOAT u, FLOAT v, FLOAT du, FLOAT dv, int image_id, 
      int width, int height, const char *source);
   void Setup(FLOAT x, FLOAT y, FLOAT w, FLOAT h, 
      int sx, int sy, int sw, int sh);

   inline bool IsValid();
   inline const char *GetSource();
   inline int GetWidth();
   inline int GetHeight();

private:
   struct Vertex
   {
      FLOAT x, y;
      FLOAT u, v;
   };

private:
   Vertex *MakeData();

private:
   FLOAT _x, _y, _w, _h;
   FLOAT _sx, _sy, _sw, _sh;

   FLOAT _u, _v, _du, _dv;
   int _image_id;
   int _width, _height;

   char _source[256];

   Vertex _data[6];

   static TArray<D3DObjectImage *> _cache;
   static bool _cache_enabled;
};

bool D3DObjectImage::IsValid()
{
   return _image_id >= 0 && _width > 0 && _height > 0;
}

const char *D3DObjectImage::GetSource()
{
   return _source;
}

int D3DObjectImage::GetWidth()
{
   return _width;
}

int D3DObjectImage::GetHeight()
{
   return _height;
}

#endif  // __EVAS_DIRECT3D_OBJECT_IMAGE_H__
