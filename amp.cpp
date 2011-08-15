#include <windows.h>
#include <stdio.h>
#include <wingdi.h>
#include <math.h>

// INTERFACE

void initAmp(HINSTANCE hInstance);
void drawAmplitude(double *d);

// DEFINITIONS

#define OFFSETY    205.0
#define SCALEY     600.0

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

static HDC hdcMem;
static HPEN hPen;
static HPEN hRed;
static HPEN hErase;
static HBRUSH eraseBrush;
static HBRUSH eraseBrush2;
static HBRUSH eraseBrush3;
static RECT rct;
static PAINTSTRUCT ps;

    int axPos = 0;
    int ayPos = 0;
    int axOrig = 0;
    int ayOrig = 0;
    int aborder = 10;
    int awidth;
    int aheight;
    int abaseline;

static HDC hdc;

LRESULT CALLBACK AProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch(Msg)
    {
    case WM_DESTROY:
        PostQuitMessage(WM_QUIT);
        break;
    default:
      return DefWindowProc(hWnd, Msg, wParam, lParam);
    }
    return 0;
}


void initAmp(HINSTANCE hInstance)
{
HWND hWnd = CreateWindowEx( WS_EX_OVERLAPPEDWINDOW
			   | WS_EX_STATICEDGE
			/* | WS_EX_TOPMOST    */
			   | WS_EX_NOACTIVATE,
			   "FFT",
			   "Amplitude",
			   WS_OVERLAPPEDWINDOW,
			   10,200,
			   700, 700,
			   NULL,
			   NULL,
			   hInstance,
			   NULL);

    ShowWindow(hWnd, SW_RESTORE);
    UpdateWindow(hWnd);

    hdc = GetDC(hWnd);
    hPen = CreatePen(PS_SOLID,2,RGB(80,200,90));
    eraseBrush = CreateSolidBrush(RGB(0,0,0));
    eraseBrush2 = CreateSolidBrush(RGB(50,50,100));
    eraseBrush3 = CreateSolidBrush(RGB(100,20,30));
    GetClientRect(hWnd, &rct);

    awidth = (rct.right - rct.left);
    aheight = (rct.bottom - rct.top);
    abaseline = aheight/2;
}

static int howoften = 0;
static HBRUSH lastBrush = eraseBrush; // Black

HBRUSH randomBrush(void)
{
  if ( (howoften++%50) == 0)
    {
      lastBrush = (rand()&2?eraseBrush:(rand()&1)?eraseBrush2:eraseBrush3);
    }
  return lastBrush;
}
extern int gWhere;

void drawAmplitude(double *d)
{
  int c = 0;
  FillRect(hdc, &rct, randomBrush());
  SelectObject(hdc,hPen);
  MoveToEx(hdc, aborder, (int)(abaseline + (d[aborder]*SCALEY)), NULL);
  gWhere = 1200;
  for (c=aborder;c<(awidth-2*aborder);c++)
    {
      int y = (int)(abaseline + (d[c]*SCALEY));
      LineTo(hdc, c+1, y+1 );
    }
}







