
#include "evas_direct3d_object_line.h"

#include "evas_direct3d_shader_pack.h"

TArray<D3DObjectLine::Vertex> D3DObjectLine::_cache;
bool D3DObjectLine::_cache_enabled = false;

D3DObjectLine::D3DObjectLine()
{
   _x1 = _y1 = 0;
   _x2 = _y2 = 0;
}

void D3DObjectLine::BegineCache()
{
   _cache.Allocate(0);
   _cache_enabled = true;
}

void D3DObjectLine::EndCache(Direct3D_DeviceContext *d3d)
{
   if (!_cache_enabled)
      return;
   D3DShaderPack::This()->SetVDecl(d3d, D3DShaderPack::VDECL_XYC);
   D3DShaderPack::This()->SetVS(d3d, D3DShaderPack::VS_COPY_COLOR);
   D3DShaderPack::This()->SetPS(d3d, D3DShaderPack::PS_COLOR);
   d3d->device->DrawPrimitiveUP(D3DPT_LINELIST, _cache.Length() / 2, 
      _cache.Data(), sizeof(Vertex));

   Log("Line cache drawn: %d items", _cache.Length() / 2);
}

void D3DObjectLine::Draw(Direct3D_DeviceContext *d3d)
{
   Vertex data[2] = {{_x1, _y1, _color}, {_x2, _y2, _color}};

   if (!_cache_enabled)
   {
      D3DShaderPack::This()->SetVDecl(d3d, D3DShaderPack::VDECL_XYC);
      D3DShaderPack::This()->SetVS(d3d, D3DShaderPack::VS_COPY_COLOR);
      D3DShaderPack::This()->SetPS(d3d, D3DShaderPack::PS_COLOR);
      d3d->device->DrawPrimitiveUP(D3DPT_LINELIST, 1, data, sizeof(Vertex));
   }
   else
   {
      _cache.Add(data[0]);
      _cache.Add(data[1]);
   }
}

void D3DObjectLine::Setup(FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, DWORD color)
{
   _x1 = x1;
   _y1 = y1;
   _x2 = x2;
   _y2 = y2;
   _color = color;
}