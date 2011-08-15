#include "windows.h"
#include "stdio.h"
#include <signal.h>
#include "digits.h"

void colorOn(int n)
{
  Out32(PPORT_BASE,0x01<<(n-1));
}

void colorOff(int n)
{
  Out32(PPORT_BASE,0);
}




