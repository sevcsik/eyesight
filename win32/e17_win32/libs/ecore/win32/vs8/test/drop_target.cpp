
#include <windows.h>
#include <assert.h>



class DropTarget : public IDropTarget
{
public:
   // IUnknown members
   HRESULT __stdcall QueryInterface(REFIID iid, void ** ppvObject);
   ULONG __stdcall AddRef();
   ULONG __stdcall Release();

   // IDropTarget implementation
   HRESULT __stdcall DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
   HRESULT __stdcall DragOver(DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
   HRESULT __stdcall DragLeave();
   HRESULT __stdcall Drop(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);

   // Constructor / Destructor
   DropTarget(HWND hwnd, void (*callback)(void *ptr, const char *str), void *ptr);
   ~DropTarget();

private:
   // internal helper function
   DWORD DropEffect(DWORD grfKeyState, POINTL pt, DWORD dwAllowed);
   bool  QueryDataObject(IDataObject *pDataObject);

private:
   typedef void (*DropCallbackType)(void *ptr, const char *str);

private:
   LONG _ref_count;
   HWND _hwnd;
   bool _allow_drop;
   DropCallbackType _drop_callback;
   void *_drop_callback_ptr;

};


DropTarget::DropTarget(HWND hwnd, void (*callback)(void *ptr, const char *str), void *ptr)
{
   // reference count must ALWAYS start at 1
   _ref_count = 1;
   _hwnd = hwnd;
   _allow_drop = false;
   _drop_callback = callback;
   _drop_callback_ptr = ptr;
}

DropTarget::~DropTarget()
{
}

HRESULT DropTarget::QueryInterface(REFIID iid, void **ppvObject)
{
   // check to see what interface has been requested
   if (iid == IID_IDropTarget || iid == IID_IUnknown)
   {
      AddRef();
      *ppvObject = this;
      return S_OK;
   }
   *ppvObject = 0;
   return E_NOINTERFACE;
}

ULONG DropTarget::AddRef()
{
   return InterlockedIncrement(&_ref_count);
}

ULONG DropTarget::Release()
{
   LONG count = InterlockedDecrement(&_ref_count);
   if (count == 0)
   {
      delete this;
      return 0;
   }
   return count;
}

HRESULT DropTarget::DragEnter(IDataObject *pDataObject, DWORD grfKeyState, 
   POINTL pt, DWORD *pdwEffect)
{
   // does the dataobject contain data we want?
   _allow_drop = QueryDataObject(pDataObject);
   if (_allow_drop)
   {
      // get the dropeffect based on keyboard state
      *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
      SetFocus(_hwnd);
      //PositionCursor(_hwnd, pt);
   }
   else
      *pdwEffect = DROPEFFECT_NONE;
   return S_OK;
}

HRESULT DropTarget::DragOver(DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{
   if (_allow_drop)
   {
      *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
      //PositionCursor(m_hWnd, pt);
   }
   else
   {
      *pdwEffect = DROPEFFECT_NONE;
   }

   return S_OK;
}

HRESULT DropTarget::DragLeave()
{
   return S_OK;
}

HRESULT DropTarget::Drop(IDataObject *pDataObject, DWORD grfKeyState, 
   POINTL pt, DWORD *pdwEffect)
{
   //PositionCursor(_hwnd, pt);

   if (_allow_drop)
   {
      // construct a FORMATETC object
      FORMATETC fmtetc = { CF_TEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
      STGMEDIUM stgmed;

      // See if the dataobject contains any TEXT stored as a HGLOBAL
      if (pDataObject->QueryGetData(&fmtetc) == S_OK)
      {
         // Yippie! the data is there, so go get it!
         if (pDataObject->GetData(&fmtetc, &stgmed) == S_OK)
         {
            // we asked for the data as a HGLOBAL, so access it appropriately
            PVOID data = GlobalLock(stgmed.hGlobal);

            if (_drop_callback != NULL)
               _drop_callback(_drop_callback_ptr, (const char *)data);

            GlobalUnlock(stgmed.hGlobal);

            // release the data using the COM API
            ReleaseStgMedium(&stgmed);
         }
      }
      *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
   }
   else
   {
      *pdwEffect = DROPEFFECT_NONE;
   }

   return S_OK;
}


DWORD DropTarget::DropEffect(DWORD grfKeyState, POINTL pt, DWORD dwAllowed)
{
   DWORD dwEffect = 0;

   // 1. check "pt" -> do we allow a drop at the specified coordinates?

   // 2. work out that the drop-effect should be based on grfKeyState
   if (grfKeyState & MK_CONTROL)
   {
	   dwEffect = dwAllowed & DROPEFFECT_COPY;
   }
   else if (grfKeyState & MK_SHIFT)
   {
	   dwEffect = dwAllowed & DROPEFFECT_MOVE;
   }

   // 3. no key-modifiers were specified (or drop effect not allowed), so
   //    base the effect on those allowed by the dropsource
   if (dwEffect == 0)
   {
	   if (dwAllowed & DROPEFFECT_COPY) dwEffect = DROPEFFECT_COPY;
	   if (dwAllowed & DROPEFFECT_MOVE) dwEffect = DROPEFFECT_MOVE;
   }

   return dwEffect;
}

bool DropTarget::QueryDataObject(IDataObject *pDataObject)
{
    FORMATETC fmtetc = { CF_TEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    // does the data object support CF_TEXT using a HGLOBAL?
    return pDataObject->QueryGetData(&fmtetc) == S_OK;
}



void RegisterDropWindow(HWND hwnd, IDropTarget **ppDropTarget, 
   void (*callback)(void *ptr, const char *str), void *ptr)
{
	DropTarget *pDropTarget = new DropTarget(hwnd, callback, ptr);
	// acquire a strong lock
	CoLockObjectExternal(pDropTarget, TRUE, FALSE);
	// tell OLE that the window is a drop target
	RegisterDragDrop(hwnd, pDropTarget);
	*ppDropTarget = pDropTarget;
}

void UnregisterDropWindow(HWND hwnd, IDropTarget *pDropTarget)
{
	// remove drag+drop
	RevokeDragDrop(hwnd);
	// remove the strong lock
	CoLockObjectExternal(pDropTarget, FALSE, TRUE);
	// release our own reference
	pDropTarget->Release();
}
