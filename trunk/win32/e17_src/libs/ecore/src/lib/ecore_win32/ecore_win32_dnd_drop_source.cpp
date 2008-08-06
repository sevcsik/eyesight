
#include <windows.h>
#include <assert.h>

extern "C"
{
#include "Ecore_Win32.h"
#include "ecore_win32_private.h"
}


class DropSource : public IDropSource
{
public:
   // IUnknown members
   HRESULT __stdcall QueryInterface(REFIID iid, void ** ppvObject);
   ULONG __stdcall AddRef();
   ULONG __stdcall Release();

   // IDropSource members
   HRESULT __stdcall QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState);
   HRESULT __stdcall GiveFeedback(DWORD dwEffect);

   // Constructor / Destructor
   DropSource();
   ~DropSource();

private:
   LONG _ref_count;

};


DropSource::DropSource()
{
   // reference count must ALWAYS start at 1
   _ref_count = 1;
}

DropSource::~DropSource()
{
}

HRESULT DropSource::QueryInterface(REFIID iid, void **ppvObject)
{
   // check to see what interface has been requested
   if (iid == IID_IDropSource || iid == IID_IUnknown)
   {
      AddRef();
      *ppvObject = this;
      return S_OK;
   }
   *ppvObject = 0;
   return E_NOINTERFACE;
}

ULONG DropSource::AddRef()
{
   return InterlockedIncrement(&_ref_count);
}

ULONG DropSource::Release()
{
   LONG count = InterlockedDecrement(&_ref_count);
   if(count == 0)
   {
      delete this;
      return 0;
   }
   return count;
}

void *_ecore_win32_dnd_create_drop_source()
{
   IDropSource *object = new DropSource();
   assert(object != NULL);
   return object;
}

HRESULT DropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
{
    // if the Escape key has been pressed since the last call, cancel the drop
    if(fEscapePressed == TRUE)
        return DRAGDROP_S_CANCEL;	

    // if the LeftMouse button has been released, then do the drop!
    if((grfKeyState & MK_LBUTTON) == 0)
        return DRAGDROP_S_DROP;

    // continue with the drag-drop
    return S_OK;
}

HRESULT DropSource::GiveFeedback(DWORD dwEffect)
{
    return DRAGDROP_S_USEDEFAULTCURSORS;
}
