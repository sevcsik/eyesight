
#include "evas_direct3d_shader_pack.h"

#include <assert.h>

Ref<D3DShaderPack> D3DShaderPack::_this;

D3DShaderPack::D3DShaderPack()
{
}

D3DShaderPack::~D3DShaderPack()
{
   Uninitialize();
}

D3DShaderPack *D3DShaderPack::This()
{
   if (_this.IsNull())
      _this = new D3DShaderPack();
   return _this;
}


bool D3DShaderPack::Initialize(Direct3D_DeviceContext *d3d)
{
   bool res = true;
   if (!(res = InitVertexDeclarations(d3d) && res))
      Log("Failed to create vdecl set");
   if (!(res = InitVertexShaders(d3d) && res))
      Log("Failed to create vs set");
   if (!(res = InitPixelShaders(d3d) && res))
      Log("Failed to create ps set");
   return res;
}

void D3DShaderPack::Uninitialize()
{
   for (int i = 0; i < _vdecl.Length(); i++)
   {
      if (_vdecl[i] != NULL)
      {
         _vdecl[i]->Release();
         _vdecl[i] = NULL;
      }
   }

   for (int i = 0; i < _vs.Length(); i++)
   {
      if (_vs[i] != NULL)
      {
         _vs[i]->Release();
         _vs[i] = NULL;
      }
   }

   for (int i = 0; i < _ps.Length(); i++)
   {
      if (_ps[i] != NULL)
      {
         _ps[i]->Release();
         _ps[i] = NULL;
      }
   }
}

bool D3DShaderPack::InitVertexDeclarations(Direct3D_DeviceContext *d3d)
{
   _vdecl.Allocate(VDECL_NUM);
   _vdecl.Set(NULL);

   LPDIRECT3DVERTEXDECLARATION9 vdecl = NULL;
   {
      D3DVERTEXELEMENT9 elements[] = {
         {0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
         {0, 8, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
         D3DDECL_END()
         };
      if (FAILED(d3d->device->CreateVertexDeclaration(elements, &vdecl)))
         return false;
      if (vdecl == NULL)
         return false;
   }
   _vdecl[VDECL_XYC] = vdecl;

   return true;
}

bool D3DShaderPack::InitVertexShaders(Direct3D_DeviceContext *d3d)
{
   _vs.Allocate(VS_NUM);
   _vs.Set(NULL);

   {
      char buf[] = 
         "struct VsInput {	float2 pos : POSITION; float4 col : COLOR; };\n"
         "struct VsOutput { float4 pos : POSITION; float4 col : COLOR0; };\n"
         "VsOutput main(VsInput vs_in) {\n"
         "VsOutput vs_out;\n"
	      "vs_out.pos = float4(vs_in.pos, 0, 1);\n"
         "vs_out.col = vs_in.col;\n"
	      "return vs_out;}";

      _vs[VS_COPY_COLOR] = (LPDIRECT3DVERTEXSHADER9)
         CompileShader(d3d, true, "CopyColor", buf, sizeof(buf) - 1);
      if (_vs[VS_COPY_COLOR] == NULL)
         return false;
   }

   return true;
}

bool D3DShaderPack::InitPixelShaders(Direct3D_DeviceContext *d3d)
{
   _ps.Allocate(PS_NUM);
   _ps.Set(NULL);

   {
      char buf[] = 
         "struct VsOutput { float4 pos : POSITION; float4 col : COLOR0; };\n"
         "float4 main(VsOutput ps_in) : COLOR0 {\n"
         "return ps_in.col;}";

      _ps[PS_COLOR] = (LPDIRECT3DPIXELSHADER9)
         CompileShader(d3d, false, "Color", buf, sizeof(buf) - 1);
      if (_ps[PS_COLOR] == NULL)
         return false;
   }

   return true;
}

void *D3DShaderPack::CompileShader(Direct3D_DeviceContext *d3d, bool make_vs, 
   const char *name, const char *buf, int size)
{
   LPD3DXBUFFER compiled_res = NULL;
   LPD3DXBUFFER error_msgs = NULL;

   HRESULT res = D3DXCompileShader(buf, size, NULL, NULL, 
      "main", make_vs ? "vs_2_0" : "ps_2_0",  // ?
      0, &compiled_res, &error_msgs, NULL);

   if (FAILED(res))
   {
      Log("Shader %s compilation failed, code = %X", name, res);
      if (error_msgs == NULL)
         return NULL;
      const char *mess = (const char *)error_msgs->GetBufferPointer();
      Log("Error output:\n%s", mess);
      error_msgs->Release();
      return NULL;
   }

   if (error_msgs != NULL)
      error_msgs->Release();

   void *res_ptr = NULL;
   if (make_vs)
   {
      LPDIRECT3DVERTEXSHADER9 vs;
      res = d3d->device->CreateVertexShader((DWORD *)compiled_res->GetBufferPointer(), &vs);
      res_ptr = (void *)vs;
   }
   else
   {
      LPDIRECT3DPIXELSHADER9 ps;
      res = d3d->device->CreatePixelShader((DWORD *)compiled_res->GetBufferPointer(), &ps);
      res_ptr = (void *)ps;
   }

   compiled_res->Release();

   if (FAILED(res))
   {
      Log("Shader %s creation failed, code = %X", name, res);
      return NULL;
   }
   return res_ptr;
}

bool D3DShaderPack::SetVDecl(Direct3D_DeviceContext *d3d, int id)
{
   if (id < 0 || id >= _vdecl.Length() || _vdecl[id] == NULL)
      return false;
   assert(d3d != NULL);
   d3d->device->SetVertexDeclaration(_vdecl[id]);
   return true;
}

bool D3DShaderPack::SetVS(Direct3D_DeviceContext *d3d, int id)
{
   if (id < 0 || id >= _vs.Length() || _vs[id] == NULL)
      return false;
   assert(d3d != NULL);
   d3d->device->SetVertexShader(_vs[id]);
   return true;
}

bool D3DShaderPack::SetPS(Direct3D_DeviceContext *d3d, int id)
{
   if (id < 0 || id >= _ps.Length() || _ps[id] == NULL)
      return false;
   assert(d3d != NULL);
   d3d->device->SetPixelShader(_ps[id]);
   return true;
}
