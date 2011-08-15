#include <windows.h>
#include <stdio.h>

//   USAGE:
//  1) save the instance and get a context from the window handle
//   extern global_hdc = GetDC(hWnd);
//   extern global_instance = hInstance;
//
//  2) Create the window
//   makeWindow(posX,posY,width,height);
//
//  3) Render the Bitmap
//
//   drawBitmap(file);
//
//  This part of the program can be tested by compiling it
//  g++ -DSTANDALONE -o bdisplay bisplay.cpp -lgdi32

#ifdef STANDALONE
LRESULT CALLBACK WinProc(HWND,UINT,WPARAM,LPARAM);
ATOM MyRegisterClass(HINSTANCE);
#endif

void newWindow(int x, int y, int w, int h); // to replace InitInstance
void DrawBitmap(char*);       // Give it the  filename of bitmap

HDC       global_hdc;
HWND      global_bwnd;
HINSTANCE global_instance;

void makeWindow(int x, int y, int w, int h)
{
    MyRegisterClass(global_instance);
    global_bwnd = CreateWindow(
       "FFT",                 //window class
       "Bird Count",          //title bar
       WS_OVERLAPPEDWINDOW,   //window style
       x, y, w, h,                     //height of the window
       NULL,                  //parent window
       NULL,                  //menu
       global_instance,       //application instance
       NULL);                 //window parameters

    if (!global_bwnd) return;

    global_hdc = GetDC(global_bwnd);
    ShowWindow(global_bwnd, SW_RESTORE);
    UpdateWindow(global_bwnd);
}

void drawBitmap(char *filename)
{
  HBITMAP image;
  image = (HBITMAP)LoadImage(0,filename,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);

  BITMAP bm;
  GetObject(image, sizeof(BITMAP), &bm);       // get bitmap properties
  HDC hdcMem = CreateCompatibleDC(global_hdc); //create device context
  SelectObject(hdcMem, image);

  BitBlt(global_hdc,              // destination device context
	 0, 0,                    // x,y location on destination
	 bm.bmWidth, bm.bmHeight, // width,height of source bitmap
	 hdcMem,                  // source bitmap device context
	 0, 0,                    // start x,y on source bitmap
	 SRCCOPY );               // blit method

  DeleteDC(hdcMem);
  DeleteObject((HBITMAP)image);
}

#ifdef STANDALONE
LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) 
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wc;
    wc.cbSize        = sizeof(WNDCLASSEX); 
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = (WNDPROC)WinProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "FFT";
    wc.hIconSm       = NULL;

    return RegisterClassEx(&wc);
}

int WINAPI WinMain(HINSTANCE hI, HINSTANCE hP,LPSTR c, int f)
{
  char file[32];

  global_instance = hI;
  makeWindow(600,300,300,240);   // Create the window
  for (int i=0; i<8; i++)
    {
      sprintf(file, "budgie%d.bmp", i);
      drawBitmap(file);            // Render the Bitmap
      Sleep(1000 + i*500);
    }
}
#endif





