#include <windows.h>
#include <windowsx.h>
#include "resource.h"

HBITMAP hbm[7];

int bmid[7] = { IDB_BITMAP1,IDB_BITMAP2,IDB_BITMAP3,
                IDB_BITMAP4,IDB_BITMAP5,IDB_BITMAP6 };

BOOL gOnCreate(HWND hwnd, LPCREATESTRUCT lpcs)
{
  HINSTANCE hInstance = GetWindowInstance(hwnd);
  int i;
  for(i=0;i<7;i++)
    {
      hbm[i] = LoadBitmap(hInstance,MAKEINTRESOURCE(bmid[i]));
    }
  return TRUE;
}

void gOnDestroy(HWND hwnd)
{
  for(i=0;i<7;i++) { DeleteObject(hbm[i]); }
  PostQuitMessage(0);
}

void gOnPaint(HWND hwnd)
{
  PAINTSTRUCT ps;
  HDC hdc = BeginPaint(hwnd,&ps);
  HDC hdcMem = CreateCompatibleDC(NULL);
  HBITMAP hbmT = SelectBitmap(hdcMem,hbm[birds]);
  BITMAP bm;
  GetObject(hbm[birds],sizeof(bm),&bm);
  BitBlt(hdc,0,0,bm.bmWidth,bm.bmHeight,hdcMem,0,0,SRCCOPY);
  SelectBitmap(hdcMem,hbmT);
  DeleteDC(hdcMem);
  EndPaint(hwnd,&ps);
}  


LRESULT CALLBACK gWindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
  switch(uMsg)
  {
  HANDLE_MSG(hwnd,WM_CREATE,gOnCreate);
  HANDLE_MSG(hwnd,WM_DESTROY,gOnDestroy);
  HANDLE_MSG(hwnd,WM_PAINT,gOnPaint);
  }
  return DefWindowProc(hwnd,uMsg,wParam,lParam);
}  

void gRegisterClass(HINSTANCE hInstance)
{
  WNDCLASS wc = { 0, gWindowProc,0,0,
    hInstance, LoadIcon(NULL,IDI_APPLICATION),
    LoadCursor(NULL,IDC_ARROW),
    (HBRUSH)(COLOR_WINDOW+1), NULL, "GDI01" };
  RegisterClass(&wc);
}

