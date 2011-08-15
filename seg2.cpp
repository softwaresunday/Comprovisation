#include "windows.h"
#include "stdio.h"
#include <signal.h>
#include "digits.h"

void handler(int signum)
{
  printf("Handling signal %d\n", signum);
  exit(0);
}

void dispNum(int n)
{
  unsigned char val = digits[n];
  printf("value - %x\n", val);
  Out32(PPORT_BASE,val);
  printf("after Out32(addr,data)\n");
}

int main(int argc, char *argv[])
{   
  int i;
  int j;

  setvbuf(stdout,NULL,_IONBF,0);
 signal(SIGINT  ,handler);
 signal(SIGILL	,handler);
 signal(SIGFPE	,handler);
 signal(SIGSEGV	,handler);
 signal(SIGTERM	,handler);
 signal(SIGBREAK,handler);
 signal(SIGABRT,handler);
  printf("Starting...\n");
  for (j=0; j<4; j++)
    {
      for (i=0;i<10;i++)
	{
	  dispNum(i);
	  Sleep(400);
	}
      for (i=9;i>-1;i--)
	{
	  dispNum(i); Sleep(300);
	}
    }
  printf("Finished.\n");
    return 0;
}



