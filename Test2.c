#include "windows.h"
#include "stdio.h"

#define PPORT_BASE 0x378 // Prototypes for Test functions void
typedef short (_stdcall *inpfuncPtr)(short portaddr);
typedef void (_stdcall *oupfuncPtr)(short portaddr, short datum);

inpfuncPtr inp32fp;
oupfuncPtr oup32fp;

short Inp32 (short portaddr) { return(inp32fp)(portaddr); }
void  Out32 (short portaddr, short datum) {(oup32fp)(portaddr,datum); } 

int main(void)
{
  printf("Starting up...\n");
  short x;
  HINSTANCE hLib = LoadLibrary("inpout32.dll");
 if (hLib == NULL) 
   {
     fprintf(stderr,"LoadLibrary Failed.\n");
     return -1;
   }
  inp32fp = (inpfuncPtr)GetProcAddress(hLib, "Inp32");
  if (inp32fp == NULL)
    {
      fprintf(stderr,"GetProcAddress for Inp32 Failed.\n");
      return -1;
    }
  oup32fp = (oupfuncPtr) GetProcAddress(hLib, "Out32");
  if (oup32fp == NULL)
    {
      fprintf(stderr,"GetProcAddress for Oup32 Failed.\n");
      return -1;
    }
  /* Do work */

  int i;
  for(i=1; i<512; i <<= 1)
    {
      Out32(PPORT_BASE,i);
      printf("Snooze after writing %d\n",i);
      Sleep(4000);
    }
 /* finished - unload library and exit */
FreeLibrary(hLib);
 printf("All done.\n");
 return 0;
 }
