
#include <windows.h>

extern "C"
{
#include "Ecore_Win32.h"
#include "ecore_win32_private.h"
}


static bool DndInitialized = false;
static HANDLE StringToHandle(const char *szText, int nTextLen)
{
   char *ptr;

   // if text length is -1 then treat as a nul-terminated string
   if(nTextLen == -1)
      nTextLen = strlen(szText);

   // allocate and lock a global memory buffer. Make it fixed
   // data so we don't have to use GlobalLock
   ptr = (char *)GlobalAlloc(GMEM_FIXED, nTextLen + 1);

   // copy the string into the buffer
   memcpy(ptr, szText, nTextLen);
   ptr[nTextLen] = '\0';

   return ptr;
}
static HANDLE DataToHandle(const char *data, int size)
{
   char *ptr;
   ptr = (char *)GlobalAlloc(GMEM_FIXED, size);
   memcpy(ptr, data, size);
   return ptr;
}


int ecore_win32_dnd_initialize()
{
   if (DndInitialized)
      return 1;
   if (OleInitialize(0) == S_OK)
   {
      DndInitialized = true;
      return 1;
   }
   return 0;
}

void ecore_win32_dnd_uninitialize()
{
   if (!DndInitialized)
      return;
   OleUninitialize();
   DndInitialized = false;
}

int ecore_win32_dnd_begin(const char *data, int size)
{
   if (!DndInitialized)
      return 0;
   if (data == NULL)
      return 0;
   if (size < 0)
      size = strlen(data) + 1;

   IDataObject *pDataObject = NULL;
   IDropSource *pDropSource = NULL;

   FORMATETC fmtetc = { CF_TEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
   STGMEDIUM stgmed = { TYMED_HGLOBAL, { 0 }, 0 };

   stgmed.hGlobal = DataToHandle(data, size);

   bool res = false;

   // create the data object
   if ((pDataObject = (IDataObject *)
      _ecore_win32_dnd_create_data_object((void *)&fmtetc, (void *)&stgmed, 1)) != NULL &&
      (pDropSource = (IDropSource *)_ecore_win32_dnd_create_drop_source()) != NULL)
   {
      DWORD dwResult;
      DWORD dwEffect = DROPEFFECT_COPY;

      // do the drag-drop!
      dwResult = DoDragDrop(pDataObject, pDropSource, DROPEFFECT_COPY, &dwEffect);

      // finished. Check the return values to see if we need to do anything else
      if (dwResult == DRAGDROP_S_DROP)
      {
         //printf(">>> \"%s\" Dropped <<<\n", str);
         if(dwEffect == DROPEFFECT_MOVE)
         {
            // remove the data we just dropped from active document
         }
      }
      //else if (dwResult == DRAGDROP_S_CANCEL)
      //   printf("DND cancelled\n");
      //else
      //   printf("DND error\n");

      res = true;
   }

   if (pDataObject != NULL)
      pDataObject->Release();
   if (pDropSource != NULL)
      pDropSource->Release();

   // cleanup
   ReleaseStgMedium(&stgmed);
   return (int)res;
}

int ecore_win32_dnd_register_drop_target(Ecore_Win32_Window *window, 
   Ecore_Win32_Dnd_DropTarget_Callback callback)
{
   if (window == NULL)
      return 0;
   _Ecore_Win32_Window *wnd = (_Ecore_Win32_Window *)window;
   wnd->dnd_drop_target = _ecore_win32_dnd_register_drop_window(wnd->window,
      callback, (void *)wnd);
   return int(wnd->dnd_drop_target != NULL);
}

void ecore_win32_dnd_unregister_drop_target(Ecore_Win32_Window *window)
{
   if (window == NULL)
      return;
   _Ecore_Win32_Window *wnd = (_Ecore_Win32_Window *)window;
   if (wnd->dnd_drop_target != NULL)
      _ecore_win32_dnd_unregister_drop_window(wnd->window, wnd->dnd_drop_target);
}
