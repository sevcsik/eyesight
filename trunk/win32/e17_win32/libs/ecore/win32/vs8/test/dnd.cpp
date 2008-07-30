
#include <windows.h>
#include <stdio.h>

#include "dnd.h"

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




bool DndStartup()
{
   return (OleInitialize(0) == S_OK);
}

void DndShutdown()
{
   OleUninitialize();
}

bool SetClipboard(const char *str)
{
   IDataObject *pDataObject;

   FORMATETC fmtetc = { CF_TEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
   STGMEDIUM stgmed = { TYMED_HGLOBAL, { 0 }, 0 };

   stgmed.hGlobal = StringToHandle(str, -1);

   bool res = false;

   // create the data object
   if (CreateDataObject(&fmtetc, &stgmed, 1, &pDataObject) == S_OK)
   {
      // add data to the clipboard
      OleSetClipboard(pDataObject);
      OleFlushClipboard();

      pDataObject->Release();
      res = true;
   }

   // cleanup
   ReleaseStgMedium(&stgmed);
   return res;
}

bool GetClipboard(char *str, int size)
{
   IDataObject *pDataObject;
   *str = 0;
   bool res = false;

   // Access the data on the clipboard
   if (OleGetClipboard(&pDataObject) == S_OK)
   {
       FORMATETC fmtetc = { CF_TEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
       STGMEDIUM stgmed;

       // ask the IDataObject for some CF_TEXT data, stored as a HGLOBAL
       if (pDataObject->GetData(&fmtetc, &stgmed) == S_OK)
       {
           // We need to lock the HGLOBAL handle because we can't
           // be sure if this is GMEM_FIXED (i.e. normal heap) data or not
           char *data = (char *)GlobalLock(stgmed.hGlobal);
           
           strncpy(str, data, size);

           // cleanup
           GlobalUnlock(stgmed.hGlobal);
           ReleaseStgMedium(&stgmed);
       }

      pDataObject->Release();
      res = true;
   }

   return res;
}

bool BeginDnd(const char *str)
{
   IDataObject *pDataObject = NULL;
   IDropSource *pDropSource = NULL;

   FORMATETC fmtetc = { CF_TEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
   STGMEDIUM stgmed = { TYMED_HGLOBAL, { 0 }, 0 };

   stgmed.hGlobal = StringToHandle(str, -1);

   bool res = false;

   // create the data object
   if (CreateDataObject(&fmtetc, &stgmed, 1, &pDataObject) == S_OK &&
      CreateDropSource(&pDropSource) == S_OK)
   {
      DWORD dwResult;
      DWORD dwEffect = DROPEFFECT_COPY;

      // do the drag-drop!
      dwResult = DoDragDrop(pDataObject, pDropSource, DROPEFFECT_COPY, &dwEffect);

      // finished. Check the return values to see if we need to do anything else
      if (dwResult == DRAGDROP_S_DROP)
      {
         printf(">>> \"%s\" Dropped <<<\n", str);
         if(dwEffect == DROPEFFECT_MOVE)
         {
            // remove the data we just dropped from active document
         }
      }
      else if (dwResult == DRAGDROP_S_CANCEL)
         printf("DND cancelled\n");
      else
         printf("DND error\n");

      res = true;
   }

   if (pDataObject != NULL)
      pDataObject->Release();
   if (pDropSource != NULL)
      pDropSource->Release();

   // cleanup
   ReleaseStgMedium(&stgmed);
   return res;
}
