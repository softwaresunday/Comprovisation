#include "windows.h"
#include "stdio.h"

#define PPORT_BASE 0x378

extern short Inp32 (short portaddr);
extern void  Out32 (short portaddr, short datum);

int main(int argc, char *argv[])
{
  printf("Starting up...\n");
  Out32(PPORT_BASE,0);
  exit(0);
  int i;
  for(i=1; i<512; i <<= 1)
    {
      printf("Snooze after writing %d\n",i);
      Sleep(1000);
    }
 printf("All done.\n");
 return 0;
}
