#include <windows.h>
#include <stdio.h>
#include "fftnotes.h"
#include "notes.h"

void writeNote(int n); /* defined in hist.cpp */

extern int gAct;
extern int gNear;
extern int gNMIDI;

extern int gWhere;

/* #define DEBUG 1 */

/* Convert a position in the FFT output to the corresponding frequency */
#define FREQUENCY(n)	 ( (sample_rate * n ) /(2*fft_size) )

FILE *notes;
void initAmanuensis(){  notes = fopen("notes.txt","w");}
void writeNote(int n){  fprintf(notes, "%d\n", n);      }

#ifdef NEEDSREENT
#include <sys/reent.h>
struct _reent *getreent(void) { return (struct _reent *)NULL; }
#endif

/*  The defaults, if you forget to call init_fft2note() */

static double     fft_size    = (double) FFT_SIZE;
static double     sample_rate = (double) SAMPLE_RATE;
static enum scale gscale       = major;
static enum key   gkey         = c;
static int noActivity;
static int lastnote;

void init_fft2note(int size, int rate, enum key k, enum scale s)
{
  fft_size     = (double)size;
  sample_rate  = (double)rate;
  gscale        = s;
  gkey          = k;
  noActivity    = 0;
  lastnote      = 0;
}

extern int gDelay;

int fft2note(double n)
{
/* Takes the index of the component in the FFT output array
 * and finds the closest ( appropriate ) MIDI note.
 *
 * The "closest note" is restricted by setting the Key and
 * Scale. The chromatic C major scale being the obvious
 * default (e.g. any note of the equal-tempered scale).
 */
  if (n < 1.0)
    {
      if (noActivity++ > 70) { midiSilence(); }
      return;
    }
  noActivity = 0;

  int result;
  double frequency = FREQUENCY(n);
  int k = (sizeof(mdata)/sizeof(struct midiData))/2;
  int interval = (sizeof(mdata)/sizeof(struct midiData))/4;
  gWhere = 5;


  while(interval)
    {
#ifdef DEBUG
      printf("Considering %s (%d)\n", mdata[k].name, mdata[k].octave);
#endif

      if (mdata[k].frequency > frequency )
	{
	  k -= interval;
	}
      else
	{
	  k += interval;
	}
      interval /= 2;
    }

  result = k;
  if (   abs(mdata[k].frequency-frequency)
       > abs(mdata[k+1].frequency-frequency) )
    {
      result = k+1;
    }
  else if (   abs(mdata[result].frequency-frequency)
            > abs(mdata[k-1].frequency-frequency) )
    {  
      result = k-1;
    }
    
#ifdef DEBUG
      printf("I like %s (%d)\n", mdata[k].name, mdata[k].octave);
#endif
      gWhere = 7;
      int mnote = nearestValidMIDINote( mdata[result].note );

      if (!(lastnote == mnote) || (noActivity > 20) )
	{
/*	  midiDrum(mnote, 100); */
	  midiNote(mnote-12, 127);
	  writeNote(mnote);
	  Sleep(gDelay);
	}
      lastnote = mnote;

      gAct = (int) frequency;
      gNear = (int) mdata[result].frequency;
      gNMIDI = mdata[result].note;

      return ( mdata[result].note );
}

static int vnotes[7][12] = {
 { 0,2,4,5,7,9,11,0,0,0,0,0 },  /* major */
 { 0,2,3,5,7,8,10,0,0,0,0,0 },  /* minor */
 { 0,2,3,5,7,8,11,0,0,0,0,0 },  /* harmonic */
 { 0,2,3,5,7,8,11,0,0,0,0,0 },  /* melodic */
 { 0,2,5,7,9, 0,0,0,0,0,0,0 },  /* pentatonic */
 { 0,1,2,3,4,5,6,7,8,9,10,11},  /* chromatic */
};


#define ABS(x)  ((x) < 0 ? -(x) : (x))

int nearestValidMIDINote(int n)
{
  int residual =  n - (int)((n)/12)*12;
      residual =  (residual + (12-gkey) ) % 12;
  int delta    = 12;
  int best;
  int i;
  for (i=0; i<12; i++)
    {
      int diff = residual - vnotes[gscale][i];
      if (diff)
	{
	  if (ABS(diff) < delta)
	    {
	      delta = ABS(diff);
	      best = vnotes[gscale][i];
	    }
	}
      else
	{
	  return(n);
	}
    }
  return( (best + gkey) + (int)(n/12)*12);
}

