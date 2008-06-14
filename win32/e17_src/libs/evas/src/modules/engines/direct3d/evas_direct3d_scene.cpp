
#include "evas_direct3d_scene.h"

D3DScene::D3DScene()
{
}

void D3DScene::FreeObjects()
{
   for (int i = 0; i < _objects.Length(); i++)
      _objects[i]->SetFree(true);
}

void D3DScene::DrawAll(D3DDevice *d3d)
{
   for (int i = 0; i < _objects.Length(); i++)
      _objects[i]->Draw(d3d);
}
