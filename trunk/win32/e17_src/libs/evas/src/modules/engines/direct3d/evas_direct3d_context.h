#ifndef __EVAS_DIRECT3D_CONTEXT_H__
#define __EVAS_DIRECT3D_CONTEXT_H__

#include "evas_engine.h"

#include "ref.h"

class D3DContext : virtual public Referenc
{
public:
   D3DContext();

public:
   DWORD color;

};

#endif  // __EVAS_DIRECT3D_CONTEXT_H__
