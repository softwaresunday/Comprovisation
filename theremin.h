#include <windows.h>
#include <winsock2.h>
#include <mmsystem.h>  /* Access to audio input */
#include <iostream>
#include <fstream>

#include "fftnotes.h"  /* Enumeration types for keys and scales */

// Divide integer audio data by SCALE_AUDIO to get double less than 1.0

#define BUFFER_SIZE  ( FFT_SIZE *  1  * 2)

#ifdef DECLARE
int  gBirds       = 0;      // Don't show the birds unless asked
int  gCycles      = -1;     // Run forever
int  gDetect      =  400;   // Option  -level NNNN
int  gDelay       =   50;   // Option  -delay <msec>
int  gInstrument  =   1;    // Option  -i <midiPatch>  (default: piano)
int  gDevId       =  -1;
int  gMIDIDev     =   0;
int  gWhere       =   0;
int  gOsc         =   0;  // OSC messages off by default
int  gColors      =   0;  // Colored lights off by default
int  gSerial      =   0;  // Serial output off by default
char gSerialPort[20];
enum key   gKey   =   c;
enum scale gScale = major;
double gScaleAudio = SCALE_AUDIO;
char gHostname[256];
bool  gShutdown    =  false;
#else
extern int    gBirds;
extern int    gCycles;
extern int    gDetect;
extern int    gDelay;
extern int    gInstrument;
extern int    gDevId;
extern int    gMIDIDev;
extern int    gWhere;
extern int    gOsc;
extern int    gColors;
extern int    gSerial;
extern char   gSerialPort[20];
extern enum key   gKey;
extern enum scale gScale;
extern double gScaleAudio;
extern char gHostname[256];
extern bool  gShutdown;
#endif

bool select_device(int *devId);
bool open_device(UINT devId, LPHWAVEIN device);
void analyze(short *buffer);
int  string_to_enum(const char *str);
void init_osc(char *address, int port);
void send_osc(int freq, int eqfreq, int midinote);

/*  MIDI INTERFACE (and FFT->note conversion) all written in "C" */

#ifdef __cplusplus
extern "C" int  openMidiOut(int outID);
extern "C" void midiPatch(int patch);
extern "C" int  midiNote(int note, int velocity);
extern "C" int  nearestValidMIDINote(int size);
extern "C" void init_fft2note(int fftsize, int rate, enum key k, enum scale s);
extern "C" int fft2note(double n);
extern "C" void resetGS(void);
extern "C" void resetGM(void);
extern "C" void midiSilence(void);
#else
int  openMidiOut(int outID);
void midiPatch(int patch);
int  midiNote(int note, int velocity);
int  nearestValidMIDINote(int);
void init_fft2note(int fftsize, int rate, enum key k, enum scale s);
int fft2note(double n);
void resetGS(void);
void resetGM(void);
void midiSilence(void);
#endif





