#include <windows.h>
#include <stdio.h>
#include <wingdi.h>
#include <math.h>
#include <windowsx.h>

//#include "theremin.h"  // global variables
#include "digits.h"   // displayNumber(n) MACRO defined here

void tweet(char const *tweet, int value);

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

void makeWindow(int x, int y, int w, int h);
void showBirdCount(void);

void drawBitmap(char *filename);

HDC       global_hdc;
HWND      global_bwnd;
HINSTANCE global_instance;

// INTERFACE


static HPEN hPen;
static HPEN hRed;
static HPEN hErase;
static HBRUSH eraseBrush;
static RECT rct;
static PAINTSTRUCT ps;

    int bwidth;
    int bheight;

#define MAX_BIRDS  8
int birds = 0;
#define abs(x)  ( x<0 ? -(x) : (x) )

int    step;
int    nextstep = 0;
double steps[10];

char *showFactors(int bits)
{
  char *s = (char *)malloc(128);
  char *ss = (char *)malloc(128);
  strcpy(s, "FACTORS :  [ ");
  strcpy(ss, " VALUES :  [ ");
  int pos = 0;
  while(bits)
    {
      if (bits&1)
	{
	  char tmp[20];
	  char stmp[20];
	  sprintf(tmp,"%d, ",pos);
	  sprintf(stmp, "%8.2f, ", steps[pos]);
	  strcat(s,tmp);
	  strcat(ss,stmp);
	}
      bits >>= 1;
      pos++;
    }
  s[strlen(s)-2] = (char)NULL;
  ss[strlen(ss)-2] = (char)NULL;
  strcat(s, " ]");
  strcat(ss, " ]");
  strcat(s,ss);
  return s;
}

void moreBirds(int birds)
{
  if (birds == 1) tweet("There is now %d bird on the perch", birds);
  else            tweet("There are now %d birds on the perch", birds);
  if (nextstep < 9) nextstep++;
}

void lessBirds(int birds)
{
  if ( nextstep > 0 ) nextstep--;

if (birds==0) { tweet("The perch is now empty.",0); }
else if (birds == 1) { tweet("A bird has flown. Now only one.",0); }
else tweet("This bird has flown. There are now %d birds on the perch", birds);
}


void makeWindow(int x, int y, int w, int h)
{

    global_bwnd = CreateWindow(
       "FFT",                 //window class
       "Bird Count",          //title bar
       WS_OVERLAPPEDWINDOW,   //window style
       x, y, w, h,                     //height of the window
       NULL,                  //parent window
       NULL,                  //menu
       global_instance,       //application instance
       NULL);                 //window parameters

    if (!global_bwnd) 
      {
	printf("Could not create window for Bird Count\n");
	exit(0);
      }
#ifdef DEBUG
    else
      {
	printf("Bird Count window created okay %x\n",global_bwnd);
      }
#endif
    ShowWindow(global_bwnd, SW_RESTORE);
    UpdateWindow(global_bwnd);
#ifdef DEBUG
    printf("Bird Window should be visible\n");
#endif
}

static int prevbirds = -1;
static int refresh   = 0;
void showBirdCount(void)
{
  char bitmapfile[32];
  if (birds == prevbirds && refresh++ < 100 ) return;
  refresh = 0;
  sprintf(bitmapfile, "budgie%d.bmp", birds);
  //  printf("Drawing %s\n",bitmapfile);
  drawBitmap(bitmapfile);
  prevbirds = birds;
  //  displayNumber(birds);
}  

void setBirdCount(int n)
{
  printf("Setting Bird Count to %d\n",n);
  birds = n;
  //  displayNumber(n);
}



void drawBitmap(char *filename)
{
  if (!global_hdc)
    { global_hdc = GetDC(global_bwnd); }
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

static int cbc = 0;
static double pitch;
static double previous;
static int drift = 0;
static double delta = 1.5;
static double error = 10.0;

void checkBirdCount(double principal)
{
  if      (principal < 50.0)       {  birds = 0; }
  else if (principal < 70.0)  {  birds = 1; }
  else if (principal < 100.0)  {  birds = 2; }
  else if (principal < 120.0)  {  birds = 3; }
  else if (principal < 160.0) {  birds = 4; }
  else if (principal < 200.0) {  birds = 5; }
  else if (principal < 260.0) {  birds = 6; }
  else if (principal < 300.0) {  birds = 7; }
  else {  birds = 8; }
  return;

  if (!cbc) { previous = principal; cbc++; }  // FIRSTTIME
  if (cbc<4)
    {
      if (abs(principal-pitch)<1.0)  { cbc++;             }
      else                           { pitch = principal; }
      return;
    }
  else
    {
      cbc = 1;
      double change = pitch - previous;

      if (abs(change)<1.0) return;

      //      printf("Got a change to!%6.2f\n",pitch);

      if ( change > 0 && abs(change) > delta )  // HGHER
       {
	 drift = 0; // Significant change
	 if (birds < MAX_BIRDS)
	   {
	     birds += 1;
	     steps[nextstep] = change;
	     moreBirds(birds);
	     previous = pitch;
	   }
	 else
	   {
	     printf("Too many birds! Ignoring the new one and knocking another off the perch!\n");
	   }
       }
      else if ( change < 0 && abs(change) > delta) // LOWER
	{
	 drift = 0; // Significant change
	 change = -change;
#define HANDWAVING
#ifdef HANDWAVING
	  double mindiff = 10.0;
#else
	  double mindiff = delta;
#endif
	  double diff = 0.0;
          int    thisstep = -1;
	  for(step=0; step<nextstep; step++)
	    {
	      diff = abs(steps[step] - change);
	      if (diff < mindiff)
		{
		  thisstep = step;
		  mindiff = diff;
		}
	    }
	  if (thisstep < 0)
	    {
#ifdef DEBUG
	      if (rand()&127==127) {
		  printf("del2sm\n");
	      }
#endif
	      if (birds > 0) {
		birds -= 1;
		lessBirds(birds);
	      }
	    }
	  else
	    {
	      if ( mindiff < error)
		{
		  if (birds > 0)
		    {
		      birds -= 1;
		      previous = pitch;
		      for(step=thisstep; step<(nextstep-1); step++)
			{
			  steps[step] = steps[step+1];
			}
		      lessBirds(birds);
		    }
		  else
		    {
		      printf("What does it mean when there are negative birds?\n");
		    }
		}
	    }
	}
      else
	{
	  // Compensate for drift by updating the baseline
	  // when we have insignificant change over time.
	  if (drift++ > 10)
	    {
	      drift = 0;
	      previous = pitch;
	    }
	}
    }
}




