#define _WINIO 1 
#include <windows.h>
#include <stdio.h>
#include "winio.h" 

unsigned char digits[10] =
    {0x7B,0x60,0x2F,0x6E,0x74,0x5E,0x5F,0x68,0x7F,0x7C};
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





