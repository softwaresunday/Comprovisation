#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>

#include <windows.h>    /* Basic Data Types */
#include <mmsystem.h>   /* MIDI Types */

#include <signal.h>     /* POSIX Signals */

// #define DEBUG
/* RANDOM GAUSSIAN INTEGER GENERATOR */

struct pnote {
  struct pnote *next;
  unsigned long time;
  unsigned char note;
};

typedef struct pnote pnote_t;
pnote_t *lastnote = (pnote_t *)NULL;

char sysExGM[6]={ 0xF0, 0x7E, 0x7F, 0x09, 0x01, 0xF7 };
char sysExGS[11]={0xF0,0x41,0x10,0x42,0x12,0x40,0x00,0x7F,0x00,0x41,0xF7};

static void addpnote(unsigned char);
static void snuffpnote(void);

void initRandom()  /* Initialized by init_midi_predicates() */
{
  srand(time(0));
}

static int gauss(int sigma)
{
  return(
	(rand()&((long) pow(2, (log(2*sigma)+1))-1))-sigma
	 );
}

static HMIDIOUT midiOut;

static int cOut;  /* Current Midi Output device */
static int cIn;   /* Current Midi Input device  */

int         inDevs;
int         outDevs;

int openMidiOut(int outID)
{
  MMRESULT result = midiOutOpen(&midiOut,
				outID,
				(DWORD) NULL,
				(DWORD) NULL,
				CALLBACK_NULL );
  if (result)
    {
      printf("Could not open MIDI Output device(%d)%d\n",outID,result);
      return FALSE;
    }
#ifdef DEBUG
  printf("MIDI out(%d) opened okay\n", outID);
#endif
  return TRUE;
}

#define NOTE_DATA        2
#define VELOCITY_DATA    3
#define CHANNEL_DATA     4
#define TIME_DATA        5
#define CALL_PROLOG      0x10

static int midiStyle = TIME_DATA | CALL_PROLOG;

int closeMidiOut()
{
  midiOutClose(midiOut);
  return TRUE;
}


void longMIDI(char *cp, UINT size)
{
  MIDIHDR hdr;
  LPMIDIHDR lpMidiOutHdr;

  hdr.lpData = (char *)malloc(size);
  strncpy(hdr.lpData, cp, size );
  hdr.dwBufferLength = size;
  midiOutPrepareHeader(midiOut, &hdr, sizeof(MIDIHDR));
  midiOutLongMsg(midiOut, &hdr, sizeof(MIDIHDR));
}

int midiSend(int dev, int command, int note, int velocity)
{
  midiOutShortMsg(midiOut,(DWORD)(command|(note<<8)|(velocity<<16)));
  if (velocity) addpnote((unsigned char)note);
  snuffpnote();
  return TRUE;
}

int midiNote(int note, int velocity)
{
  midiOutShortMsg(midiOut,(DWORD)(0x90|(note<<8)|(velocity<<16)));
  if (velocity) addpnote((unsigned char)note);
  snuffpnote();
  return TRUE;
}

void midiSilence(void)
{
  midiOutShortMsg(midiOut,(DWORD)(0xB0|(120<<8)));
}

int midiDrum(int note, int velocity)
{
  midiOutShortMsg(midiOut,(DWORD)(0x99|(note<<8)|(velocity<<16)));
  if (velocity) addpnote((unsigned char)note);
  snuffpnote();
  return TRUE;
}

void midiPatch(int patch)
{
  midiOutShortMsg(midiOut,(DWORD)(0xC0|(patch<<8)));
}

static int midiOut3(int command, int note, int velocity)
{
  midiOutShortMsg(midiOut,(DWORD)(command|(note<<8)|(velocity<<16)));
  if (velocity) addpnote((unsigned char)note);
  snuffpnote();
  return TRUE;
}

static int silence(int note)
{
  if (!note)
    {
      midiOutShortMsg(midiOut,(DWORD)(0xB0|(123<<8)));
      return TRUE;
    }
  midiOutShortMsg(midiOut,(DWORD)(0x90|cOut)|(note<<8));
  return TRUE;
}

static int drumOut(int command, int note, int velocity)
{
  command += 9;
  midiOutShortMsg(midiOut,(DWORD)(command|(note<<8)|(velocity<<16)));
  return TRUE;
}

void midiStep(int t)
{
  Sleep(t);
}


const char *names[12] = {"C","C#","D","Eb","E","F","F#","G","Ab","A","Bb","B"};

int running;

  struct midiEvent_t {
    DWORD param1;
    DWORD param2;
    UINT command;
    UINT channel;
    UINT note;
    UINT velocity;
    UINT interval;
  };

pnote_t *oldnotes = (pnote_t *)NULL;

/*
 * Create a linked list of notes played so that
 * we can silence them 5 seconds later and they
 * won't be ringing in our ears
 */

static void
snuffpnote()
{
  if (oldnotes != (pnote_t *)NULL)
      {
	if ( oldnotes->time + 2 < time(NULL) )
	  {
	    pnote_t *tmp = oldnotes->next;
	    midiOutShortMsg(midiOut,
			    (DWORD)(0x80|((oldnotes->note)<<8)));

            if (lastnote == oldnotes)
		lastnote = oldnotes->next; /* which is NULL */

	    free(oldnotes);
	    oldnotes = tmp;
	    snuffpnote(); /* Recursion */
	  }
      }
}

static void
addpnote(unsigned char note)
{
  if (oldnotes == (pnote_t *)NULL)
    {
      lastnote = oldnotes = (pnote_t *)malloc(sizeof(pnote_t));
      oldnotes->note = note;
      oldnotes->time = time(NULL);
      oldnotes->next = (pnote_t *)NULL;
    }
  else
    {
      lastnote->next = (pnote_t *)malloc(sizeof(struct pnote));
      lastnote = lastnote->next;

      lastnote->note = note;
      lastnote->time = time(NULL);
      lastnote->next = (pnote_t *)NULL;
    }
}
void resetGS(void) { longMIDI(sysExGS, 11); }
void resetGM(void) { longMIDI(sysExGM, 6); }
