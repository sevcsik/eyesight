#ifndef __EVAS_DIRECT3D_SHADER_PACK_H__
#define __EVAS_DIRECT3D_SHADER_PACK_H__

#include "evas_engine.h"

#include "ref.h"
#include "array.h"

class D3DShaderPack : virtual public Referenc
{
public:
   ~D3DShaderPack();

   static D3DShaderPack *This();

   bool Initialize(Direct3D_DeviceContext *d3d);
   void Uninitialize();

   bool SetVDecl(Direct3D_DeviceContext *d3d, int id);
   bool SetVS(Direct3D_DeviceContext *d3d, int id);
   bool SetPS(Direct3D_DeviceContext *d3d, int id);

public:
   enum VDECL
   {
      VDECL_XYC = 0,

      VDECL_NUM
   };

   enum VS
   {
      VS_COPY_COLOR = 0,

      VS_NUM
   };

   enum PS
   {
      PS_COLOR = 0,

      PS_NUM
   };

private:
   D3DShaderPack();

   bool InitVertexDeclarations(Direct3D_DeviceContext *d3d);
   bool InitVertexShaders(Direct3D_DeviceContext *d3d);
   bool InitPixelShaders(Direct3D_DeviceContext *d3d);

   void *CompileShader(Direct3D_DeviceContext *d3d, bool make_vs, const char *name, 
      const char *buf, int size);

private:
   TArray<LPDIRECT3DVERTEXDECLARATION9> _vdecl;
   TArray<LPDIRECT3DVERTEXSHADER9> _vs;
   TArray<LPDIRECT3DPIXELSHADER9> _ps;

   static Ref<D3DShaderPack> _this;
};

#endif  // __EVAS_DIRECT3D_SHADER_PACK_H__
