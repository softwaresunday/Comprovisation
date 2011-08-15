#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "fftnotes.h"

#define MAX_BIRDS  8

const char *spaces = "                            ";
int birds = 0;
extern int gDevId;

#define abs(x)  ( x<0 ? -(x) : (x) )


#define HANDWAVING
// #define TESTING

#ifdef TESTING
static int td = 1;
static int testdata[13] = { 13, 67, 68, 33, 32, 12, 77, 66, 277, 66, 77, 88, 500 };
double fftdata[FFT_SIZE];
double *fft(void)
{
  if (td > 12) { td = 1; fftdata[testdata[12]] = 0.0; }
  fftdata[testdata[td-1]] = 0.0;
  fftdata[testdata[td]] = 300.0;
  td++;
  return fftdata;
}
#else
double *fft(void);                           // In theremin.cpp
int process_options(int argc, char *argv[]);
#endif




bool nearly(double a, double b, double error)
{
  if (abs(a-b) < error ) return true;
  return false;
}

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



double principal(double *fft, double previous)
{
 double max = 0.0;
 int maxIndex = 0;
 int low;
 int high;
 int i;
    if (previous < 1.0)
      {
	low = 1; high = FFT_SIZE/2;
      }
    else
      {
	low = (int)previous - 40;
	high = low + 80;
      }

    for(i=low; i<high ; i++)
    {
      if (fft[i] > max )
	{
	  max = fft[i];
	  maxIndex = i;
	}
    }
  return (double) maxIndex;
}

static int prevbirds = -1;

void report(int birds)
{
  printf(spaces);
  if (birds < prevbirds)
    {
      printf("Bye bye, Birdie. ");
    }
  if (birds == 1)
    {
      printf("There is now 1 bird on the perch\n");
    }
  else
    {
      printf("There are %d  birds on the perch\n", birds);
    }
  prevbirds = birds;
}
      
      


/*
 * New value is lower by an amount approximately equal
 * to several of the steps added together. For each step
 * in the sum, decrement the bird count and remove it
 * from the steps.
 */

int sumperm(double target)
{
  double sum = 0.0;
  int i = 0;
  int k = 0;
  int bin = 0;
  int temp = 0;
  int insum[100];
  int perm = (int) pow(2,nextstep);
  printf("permutation goal is %d\n", perm);

  double min = 1000;
  double goalsum;
  int goalfactors;
  int goal;

  for(bin = 1; bin < perm; bin++)
   {
     for(k=0;k<nextstep;k++) insum[k] = 0;
     temp = bin;
     k = 0;
     i = 0;
     sum = 0.0;
     while(temp)
       {
	 if (temp&1)
	   {
	     sum += steps[i];
	     insum[k++] = i;
	   }
	 temp >>= 1;
	 i++;
       }
     if (abs(target-sum) < min)
       {
	 min = abs(target-sum);
	 goalsum = sum;
	 goalfactors = k;
	 goal = bin;
       }
   }
  showFactors(goal);
  printf("Closest Sum = %g composed of %d factors (%d)%s\n",
	 goalsum, goalfactors, goal, showFactors(goal));
  printf("Withing %3.2f error\n", min);
  return goal;
}

#include <signal.h>
void myhandler(int signum) { printf("Goodbye\n"); exit(0);}

double average(void)
{
static double previous = 0.0;

       int count = 0;
       int bad = 0;
       double first;
       double next;
       first = principal( fft(), previous );
       while(count < 3)
	 {
	   next = principal( fft(), previous );
// printf("first %6.2f  previous %6.2f count %d\n", first, previous, count);
	      if (abs(first-next)< 3.0) count++;
	      else if (bad++ > 3)
		{
		  first = principal( fft(), previous );
		  bad = 0;
		  count = 0;
		}
	 }
       previous = first;
       return first;
}

main(int argc, char *argv[])
{
  setvbuf(stdout,NULL,_IONBF,0);
  signal (SIGINT,  myhandler);
  signal (SIGTERM, myhandler);

  printf("hello\n");
  double delta = 1.5;
  double error = 10.0;
  process_options(argc, argv);

  int    dir;  // higher 1, unchanged 0, lower -1
  double last;
  double empty;
  last =  empty = average();
  printf("%sLAST %6.2f   EMPTY PERCH %6.2f\n\n", spaces, last, empty);


  double current;
  double change;
  int drift = 0;

  while(1)
    {
      Sleep(500);
      current = average();
      change = current - last;
//    printf("%sprincipal %6.2f last(%6.2f)  CHANGE IS %6.2f\n\n",
      //     spaces, current, last, current-last);

      if ( change > 0 && abs(change) > delta )  // HGHER
       {
	 drift = 0; // Significant change
	 if (birds < MAX_BIRDS)
	   {
	     birds += 1;
	     report(birds);
	     last = current;
//	     printf("setting last %6.2f\n",last);
	     steps[nextstep++] = change;
	   }
	 else
	   {
	     printf("Too many birds! Ignoring the new one and knocking another off the perch!\n");
	     birds -= 1;
             nextstep--;
	   }
       }
      else if ( change < 0 && abs(change) > delta) // LOWER
	{
	 drift = 0; // Significant change
//    printf("%sCHANGE WAS %6.2f (delta is %6.2f)\n",spaces,change,delta);
          change = -change;
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
//		  printf("value in step %d is closer\n", step);
		  thisstep = step;
		  mindiff = diff;
		}
	    }
	  if (thisstep < 0)
	    {
	      printf("no stored value is close enough to this change\n");
	      printf("DELTA VALUE IS TOO SMALL\n");
	    }
	  else
	    {
//      printf("value in step %d is closest\n", thisstep);
	      if ( mindiff < error)
		{
		  if (birds > 0)
		    {
		      birds -= 1;
		      report(birds);
		      last = current;
//		      printf("setting last %6.2f\n",last);
		      for(step=thisstep; step<nextstep; step++)
			{
		          // printf("Moving %d to %d\n", step+1, step);
			  steps[step] = steps[step+1];
			}
		      nextstep--;
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
	      last = current;
	    }
	}

#ifdef TESTING
      if (nextstep > 3)
	{
	  int uu = sumperm( steps[0] + steps[3] + steps[2] + 2.0);
	  printf("sumperm returned %d\n", uu);
	}
#endif
    }
}


