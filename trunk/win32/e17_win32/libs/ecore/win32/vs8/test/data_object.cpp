
#include <windows.h>
#include <shlobj.h>
#include <assert.h>


static HGLOBAL DupGlobalMem(HGLOBAL hMem)
{
   DWORD len = (DWORD)GlobalSize(hMem);
   PVOID source = GlobalLock(hMem);
   PVOID dest = GlobalAlloc(GMEM_FIXED, len);
   memcpy(dest, source, len);
   GlobalUnlock(hMem);
   return dest;
}



class DataObject : public IDataObject
{
public:
   // IUnknown members
   HRESULT __stdcall QueryInterface(REFIID iid, void **ppvObject);
   ULONG __stdcall AddRef();
   ULONG __stdcall Release();

   // IDataObject members
   HRESULT __stdcall GetData(FORMATETC *pFormatEtc, STGMEDIUM *pmedium);
   HRESULT __stdcall GetDataHere(FORMATETC *pFormatEtc, STGMEDIUM *pmedium);
   HRESULT __stdcall QueryGetData(FORMATETC *pFormatEtc);
   HRESULT __stdcall GetCanonicalFormatEtc(FORMATETC *pFormatEct, FORMATETC *pFormatEtcOut);
   HRESULT __stdcall SetData(FORMATETC *pFormatEtc, STGMEDIUM *pMedium,  BOOL fRelease);
   HRESULT __stdcall EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppEnumFormatEtc);
   HRESULT __stdcall DAdvise(FORMATETC *pFormatEtc,  DWORD advf, IAdviseSink *, DWORD *);
   HRESULT __stdcall DUnadvise(DWORD dwConnection);
   HRESULT __stdcall EnumDAdvise(IEnumSTATDATA **ppEnumAdvise);

   // Constructor / Destructor
   DataObject(FORMATETC *fmtetc, STGMEDIUM *stgmed, int count);
   ~DataObject();

private:
   int LookupFormatEtc(FORMATETC *pFormatEtc);

private:
   LONG _ref_count;
   int _num_formats;
   FORMATETC *_format_etc;
   STGMEDIUM *_stg_medium;

};


DataObject::DataObject(FORMATETC *fmtetc, STGMEDIUM *stgmed, int count)
{
   assert(fmtetc != NULL);
   assert(stgmed != NULL);
   assert(count > 0);

   // reference count must ALWAYS start at 1
   _ref_count = 1;
   _num_formats = count;

   _format_etc = new FORMATETC[count];
   _stg_medium = new STGMEDIUM[count];

   for(int i = 0; i < count; i++)
   {
      _format_etc[i] = fmtetc[i];
      _stg_medium[i] = stgmed[i];
   }
}

DataObject::~DataObject()
{
   delete[] _format_etc;
   delete[] _stg_medium;
}

HRESULT DataObject::QueryInterface(REFIID iid, void **ppvObject)
{
   // check to see what interface has been requested
   if (iid == IID_IDataObject || iid == IID_IUnknown)
   {
      AddRef();
      *ppvObject = this;
      return S_OK;
   }
   *ppvObject = 0;
   return E_NOINTERFACE;
}

ULONG DataObject::AddRef()
{
   return InterlockedIncrement(&_ref_count);
}

ULONG DataObject::Release()
{
   LONG count = InterlockedDecrement(&_ref_count);
   if(count == 0)
   {
      delete this;
      return 0;
   }
   return count;
}

HRESULT CreateDataObject(FORMATETC *fmtetc, STGMEDIUM *stgmeds, UINT count, 
   IDataObject **object)
{
   if(object == NULL)
      return E_INVALIDARG;
   *object = new DataObject(fmtetc, stgmeds, count);
   return (*object != NULL) ? S_OK : E_OUTOFMEMORY;
}

HRESULT DataObject::GetData(FORMATETC *pFormatEtc, STGMEDIUM *pMedium)
{
   assert(pMedium != NULL);
   int idx;
	
   // try to match the specified FORMATETC with one of our supported formats
   if((idx = LookupFormatEtc(pFormatEtc)) == -1)
      return DV_E_FORMATETC;

   // found a match - transfer data into supplied storage medium
   pMedium->tymed = _format_etc[idx].tymed;
   pMedium->pUnkForRelease = 0;

   // copy the data into the caller's storage medium
   switch(_format_etc[idx].tymed)
   {
   case TYMED_HGLOBAL:
      pMedium->hGlobal = DupGlobalMem(_stg_medium[idx].hGlobal);
      break;

   default:
      return DV_E_FORMATETC;
   }

   return S_OK;
}

HRESULT DataObject::GetDataHere(FORMATETC *pFormatEtc, STGMEDIUM *pmedium)
{
   return DATA_E_FORMATETC;
}

HRESULT DataObject::QueryGetData(FORMATETC *pFormatEtc)
{
   return (LookupFormatEtc(pFormatEtc) == -1) ? DV_E_FORMATETC : S_OK;
}

HRESULT DataObject::GetCanonicalFormatEtc(FORMATETC *pFormatEct, FORMATETC *pFormatEtcOut)
{
   // Apparently we have to set this field to NULL even though we don't do anything else
   pFormatEtcOut->ptd = NULL;
   return E_NOTIMPL;
}

HRESULT DataObject::SetData(FORMATETC *pFormatEtc, STGMEDIUM *pMedium,  BOOL fRelease)
{
   return E_NOTIMPL;
}

HRESULT DataObject::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppEnumFormatEtc)
{
   // only the get direction is supported for OLE
   if(dwDirection == DATADIR_GET)
   {
      // for Win2k+ you can use the SHCreateStdEnumFmtEtc API call, however
      // to support all Windows platforms we need to implement IEnumFormatEtc ourselves.
      return SHCreateStdEnumFmtEtc(_num_formats, _format_etc, ppEnumFormatEtc);
   }
   else
   {
      // the direction specified is not supported for drag+drop
      return E_NOTIMPL;
   }
}

HRESULT DataObject::DAdvise(FORMATETC *pFormatEtc,  DWORD advf, IAdviseSink *, DWORD *)
{
   return OLE_E_ADVISENOTSUPPORTED;
}

HRESULT DataObject::DUnadvise(DWORD dwConnection)
{
   return OLE_E_ADVISENOTSUPPORTED;
}

HRESULT DataObject::EnumDAdvise(IEnumSTATDATA **ppEnumAdvise)
{
   return OLE_E_ADVISENOTSUPPORTED;
}

int DataObject::LookupFormatEtc(FORMATETC *pFormatEtc)
{
   // check each of our formats in turn to see if one matches
   for(int i = 0; i < _num_formats; i++)
   {
      if((_format_etc[i].tymed & pFormatEtc->tymed) &&
         _format_etc[i].cfFormat == pFormatEtc->cfFormat &&
         _format_etc[i].dwAspect == pFormatEtc->dwAspect)
      {
         // return index of stored format
         return i;
      }
   }

   // error, format not found
   return -1;
}

