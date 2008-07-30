
#include <windows.h>

bool DndStartup();
void DndShutdown();

bool SetClipboard(const char *str);
bool GetClipboard(char *str, int size);

HRESULT CreateDataObject(FORMATETC *fmtetc, STGMEDIUM *stgmeds, UINT count, 
   IDataObject **object);
HRESULT CreateDropSource(IDropSource **object);
void RegisterDropWindow(HWND hwnd, IDropTarget **ppDropTarget, 
   void (*callback)(void *ptr, const char *str), void *ptr);
void UnregisterDropWindow(HWND hwnd, IDropTarget *pDropTarget);

bool BeginDnd(const char *str);
