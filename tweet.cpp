#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <process.h>

static char runCmdLine[2048];
static char program[1024];
static char msg[2048];


#include <time.h>
static time_t lasttime = 0;
#define TOOSOON 10;

static int maxmsgcnt = 0;

void tweet(const char *tweet, int value)
{
#ifdef NOTWEET
  if (maxmsgcnt++ < 1)
    printf("I'm being good. No auto-tweets.\n");
  return;
#endif
  time_t seconds = time(NULL);
  if (lasttime)
    {
      if ( (seconds - lasttime) < 10 )
	{
	  printf("Too soon. TWEET [%s] was supressed.\n",tweet);
	  return;
	}
    }
  else
    {
      lasttime = seconds;
    }

  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si,sizeof(si));
  si.cb=sizeof(si);
  ZeroMemory(&pi,sizeof(pi));
  sprintf(msg, tweet, value);
  sprintf(runCmdLine, "perl twitter.pl \"%s\"",  msg );
  strcpy(program,"C:/Perl/bin/perl.exe");
  if(!CreateProcess(program, runCmdLine, 0,0,0,0,0,0,&si,&pi))
    {
      printf("Failed to run the Tweet properly[%s]\n", runCmdLine);
    }
}



