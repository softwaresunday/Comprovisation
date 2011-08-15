/*
  A       +Anode      --A--
  F                  |     |
  -       B          F     B
  E       G          |     |
  -       C           --G---
  -       -          |     |
          D          E     C
                     |     |
                      --D--
*/


#define a  (~0x01)
#define b  (~0x02)
#define c  (~0x04)
#define d  (~0x08)
#define e  (~0x10)
#define f  (~0x20)
#define g  (~0x40)

unsigned char digits[10] =
   { (a&b&c&d&e&f),  // 0
     (b&c),          // 1
     (a&b&d&e&g),    // 2
     (a&b&c&d&g),    // 3
     (b&c&f&g),      // 4
     (a&c&d&f&g),    // 5
     (a&c&d&e&f&g),  // 6
     (a&b&c),        // 7
     (a&b&c&d&e&f&g),// 8
     (a&b&c&d&f&g)   // 9
  };

#define _WINIO 1 
#include <windows.h>
#include <stdio.h>
#include "winio.h" 

//unsigned char digits[10] =
//    {0x7B,0x60,0x2F,0x6E,0x74,0x5E,0x5F,0x68,0x7F,0x7C};
//    #0   #1   #2   #3   #4   #5   #6   #7   #8  #9

WCHAR *driver;
char *sysname = "WinIo32.sys";

void ctow(WCHAR* Dest, const CHAR* Source)
{
  int i = 0;
  while(Source[i] != '\0')
    {
      Dest[i] = (WCHAR)Source[i];
      ++i;
    }
}

// bool _stdcall GetPortVal(WORD wPortAddr, PDWORD pdwPortVal, BYTE bSize);
// bool _stdcall SetPortVal(WORD wPortAddr, DWORD dwPortVal, BYTE bSize);

void initializeSegments(void)
{
    driver = (WCHAR *)malloc(128);
    ctow(driver,sysname);
    bool ret = InitializeWinIo();
    printf("InitializeWinIo returned %d\n", ret);
    fflush(stdout);
    ret = InstallWinIoDriver(driver, false);
    printf("InstallWinIoDriver returned %d\n", ret);
    fflush(stdout);
}

void displayNumber(int num)
{
  printf("display %d\n", num);
  fflush(stdout);
  SetPortVal(0x378, digits[num], 1);
  printf("displayed %d  (%x)\n", num, digits[num]);
  fflush(stdout);
}

#ifdef STANDALONE
int main(int argc, char *argv[])
{   
  initializeSegments();
  SetPortVal(0x378, 0x0, 1);
  Sleep(20000);
  return 0;
  int i;
  int j;
  for (j=0; j<4; j++)
    {
      for (i=0;i<10;i++)
	{
	  displayNumber(i);
	  printf("displayed %d\n", i);
	  fflush(stdout);
	  Sleep(500);
	}
    }
    RemoveWinIoDriver();
    return 0;
}
#endif






