#include <windows.h>
#include <stdio.h>
#include <wingdi.h>
#include <math.h>

#include <windowsx.h>
#include "resource.h"

// SIZE OF FFT WINDOW 

static int FFT_Width = 670;
static int FFT_Height = 600;

//#define GAUSSIANTEST
#define BLOCKERASE
#define REALTIME

// AMP.CPP  Show the Theremin waveform 

void initAmp(HINSTANCE hInstance);
void drawAmplitude(double *d);

// BIRD.CPP  Display the current bird count

void makeWindow(int x, int y, int w, int h);
void showBirdCount(void);

extern double dout[];
extern int birds;

extern HINSTANCE global_instance;
extern HWND      global_bwnd;

HWND hWnd;

LPCTSTR ClsName = "FFT";
LPCTSTR WindowCaption = "FFT";

LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

extern int gDetect;

void init(int r, int g, int b);
void data(int x, int y);
void endHistogram(void);
int gauss(int amp, int center, int sd, int x);
void analyze(short *buffer);
short *getAudio(void);
void erase(void);
void showFFT(void);
int cmdline(char* command_line, char ***myArgv);
extern "C" void midiSilence(void);

static HDC hdc;
static HDC hdcMem;
static HPEN hPen;
static HPEN hRed;
static HPEN hErase;
static HBRUSH eraseBrush;
static RECT rct;
static PAINTSTRUCT ps;
    int xPos = 0;
    int yPos = 0;
    int xOrig = 0;
    int yOrig = 0;
    int border = 10;
    int width;
    int height;
    int baseline;
    int yclip;
    int idle = 0;


void gOnDestroy(HWND hwnd)
{
  PostQuitMessage(0);
}

void draw(int x, int y)
{
#ifndef BLOCKERASE
  MoveToEx(hdc, x+border, baseline, NULL);
  SelectObject(hdc,hErase);
  LineTo(hdc, x+border, 0);
#endif
  if ( y > 0 )
    {
      MoveToEx(hdc, x+border, baseline, NULL);
      SelectObject(hdc,hPen);
      LineTo(hdc, x+border, (height-border)-min(y,yclip));
    }
}
extern "C" void initAmanuensis();

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    MSG         Msg;
    WNDCLASSEX  WndClsEx;

    WndClsEx.cbSize        = sizeof(WNDCLASSEX);
    WndClsEx.style         = CS_HREDRAW | CS_VREDRAW;
    WndClsEx.lpfnWndProc   = WndProc;
    WndClsEx.cbClsExtra    = 0;
    WndClsEx.cbWndExtra    = 0;
    WndClsEx.hInstance     = hInstance;
    WndClsEx.hIcon         = LoadIcon(NULL, MAKEINTRESOURCE(IDI_FFTICON));
    WndClsEx.hCursor       = LoadCursor(NULL, IDC_ARROW);
    WndClsEx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    WndClsEx.lpszMenuName  = NULL;
    WndClsEx.lpszClassName = ClsName;
    WndClsEx.hIconSm       = LoadIcon(NULL, MAKEINTRESOURCE(IDI_FFTICON));

    RegisterClassEx(&WndClsEx);
    //    setvbuf(stdout,NULL,_IONBF,0);

    char **myArgv;
    int argc = cmdline(lpCmdLine, &myArgv);

    hWnd = CreateWindowEx( WS_EX_OVERLAPPEDWINDOW
			   | WS_EX_STATICEDGE
			 /*| WS_EX_TOPMOST     */
			   | WS_EX_NOACTIVATE,
			   ClsName,
			   WindowCaption,
			   WS_OVERLAPPEDWINDOW,
			   870,240,
			   FFT_Width + 60, FFT_Height + 60,
			   NULL,
			   NULL,
			   hInstance,
			   NULL);

    ShowWindow(hWnd, SW_RESTORE);
    UpdateWindow(hWnd);

    initAmp(hInstance);   // Show the actual waveform (FFT input)
    initAmanuensis();

    global_instance = hInstance;
    makeWindow(600,0,300,250); // Show the current bird count

    int quit = 0;
    hdc = GetDC(hWnd);
    hPen = CreatePen(PS_SOLID,2,RGB(80,200,90));
    hRed = CreatePen(PS_SOLID,2,RGB(200,80,90));
    hErase = CreatePen(PS_SOLID,2,RGB(0,0,0));
    eraseBrush = CreateSolidBrush(RGB(0,0,0));
    GetClientRect(hWnd, &rct);
    int x1;
    int y1;
    int center = 20;
    int c = 10;
    int d = 0;
    int deviation = 90;
    width = (rct.right - rct.left);
    height = (rct.bottom - rct.top);
    baseline = height-border;
    yclip = height-2*border;
    int startTheremin(int argc, char *argv[]);
    char *args[10];
    startTheremin(argc, myArgv);

    while(!quit)
      {
	if(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
	{
		if(Msg.message == WM_QUIT)
			quit = true;
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

#ifdef REALTIME  // Continuous display of FFT output
	analyze(getAudio());
	showFFT();
	showBirdCount();
#else           // Show FFT data when a high-amplitude component is heard
	if ( analyze(getAudio()) )
	  {
	    gWhere = 121;
	    idle = 0;
	    showFFT();
	  }
	else if (idle++ > 12)  // Check idle time and clear screen
	  {
	    erase();
	    midiSilence();
	    idle = 0;
	  }
#endif

#ifdef GAUSSIANTEST
        if (c++ > width) {
	  c = 10;
	  if (d++ > 40) {
#ifdef BLOCKERASE
	    FillRect( hdc, &rct, eraseBrush);
#endif
	    d = 0;
	    deviation += 20;
	    if (center++ > 200)
	      {
		deviation = 90;
		center = 20;
	      }
	  }
	}
	draw(c, gauss(100, center, deviation, c));
	c++;
#endif
      } // end while(!quit)
    ReleaseDC(hWnd, hdc);
    return 0;
}

static int gLevel;
static int gLevelSet = 0;

LRESULT CALLBACK WndProc(HWND lhWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch(Msg)
    {
    case WM_DESTROY:
      gOnDestroy(lhWnd);
      PostQuitMessage(WM_QUIT);
      break;
    case WM_LBUTTONDOWN:
      if (lhWnd == hWnd)
	{
	  gLevel = GET_Y_LPARAM(lParam);
	  int diff = abs((FFT_Height - gLevel)-gDetect);
	  if (diff < 10)
	    {
	      gLevelSet = 1;
	    }
	}
      break;
    case WM_LBUTTONUP:
      if (lhWnd == hWnd && gLevelSet) // Finished Adjusting Level
	{
	  gDetect = FFT_Height - GET_Y_LPARAM(lParam);
	  gLevelSet = 0;
	}
      break;
    case WM_MOUSEMOVE:
      if (lhWnd == hWnd && gLevelSet) // Moving Level in FFT Window
	{
	  gDetect = FFT_Height - GET_Y_LPARAM(lParam);
	}
      break;
    default:
      return DefWindowProc(lhWnd, Msg, wParam, lParam);
    }
    return 0;
}


int gauss(int amp, int center, int sd, int x)
{
  double ee =  pow( (center-x)*(center-x), 2 ) / (2* pow(sd,2));
  return (int) ( (amp*25/sqrt(sd)) * exp(-ee) ); 
}

void erase(void)
{
#ifdef BLOCKERASE
  FillRect( hdc, &rct, eraseBrush);
#endif
  // Draw the detection Level in red
  MoveToEx(hdc, border, baseline-gDetect, NULL);
  SelectObject(hdc,hRed);
  LineTo(hdc, width-border, baseline-gDetect);
}

void showFFT(void)
{
  int c;
  erase();
  for (c=border;c<(width-2*border);c++)
    {
      if ( (int) dout[c] != 0 )
	{
	  draw(c, abs((int)dout[c]));
	}
    }
}
