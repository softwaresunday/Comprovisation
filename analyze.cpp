
using namespace std;
#include "theremin.h"   
#include "fftw3.h"      // FFT library interface

#include "oscpackd/osc/OscOutboundPacketStream.h"
#include "oscpackd/ip/UdpSocket.h"
void send_osc(osc::int32 freq, osc::int32 eqfreq,osc::int32 midinote);
extern "C" void sendSerial(char x);
extern "C" void initSerial(char *dev);

#define PPORT_BASE 0x378
extern "C" short Inp32 (short portaddr);
extern "C" void  Out32 (short portaddr, short datum);

static int often = 0;
#define GREEN  0x02
#define YELLOW 0x04
#define RED    0x08
#define BLUE   0x01

void colorOn(int n)
{
  //  printf("%x\n",n);
  Out32(PPORT_BASE,n);
}

void colorOff(void)
{
  //  printf("zeroing parallel port\n");
  Out32(PPORT_BASE,0);
}

double din[BUFFER_SIZE];
double dfilter[BUFFER_SIZE];
double dout[BUFFER_SIZE];

/* fft2note.c fills these in when we call fft2note() */
osc::int32 gActualFrequency;
osc::int32 gNearestMusicalFrequency;
osc::int32 gNearestMIDINote;

int gAct;
int gNear;
int gNMIDI;


/*
 * Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
 * mkfilter -Bu -Bp -o 2 -a 4.5351473923e-03 2.2675736961e-01 -l
 *
 */

void checkBirdCount(double p); // Keep bird count up to date
void drawAmplitude(double *d); // Display the actual waveform

#define NZEROS 4
#define NPOLES 4
#define GAIN   4.049145754e+00

static float xv[NZEROS+1], yv[NPOLES+1];

void bandpass(double *in, double *out, size_t size)
{
  int i;
  for ( i=0 ; i < size ; i++ )
    {
      xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4];
      xv[4] = in[i] / GAIN;
      yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; 
      yv[4] =   (xv[0] + xv[4]) - 2 * xv[2]
	      + ( -0.1789920776 * yv[0]) + (  0.5490327091 * yv[1])
              + ( -1.5219847834 * yv[2]) + (  2.1511262027 * yv[3]);
      out[i] = yv[4];
    }
}

/*
 * Reduce the size of the output array by folding the
 * data into the first quarter of the array.
 *
 */

void fold(double *in, size_t size)
{ int i;
  for (i=1; i<(size/2); i++) { in[i] += in[(size-1)-i]; }
  //  for (i=0; i<(size/4); i++) { in[i] = in[2*i] + in[2*i+1]; }
}

/*
 * dofft() Initialize and call the fftw algorithm
 * for a one-dimensional Real-to-Complex FFT.
 *
 */

bool firsttime = true;
fftw_plan plan;

static char address[128];
static int  port;

void dofft(double *din, double *dout, size_t dsize)
{
  fftw_r2r_kind kind = FFTW_R2HC;
  if (firsttime)
    {
      if (gOsc)
	{
	  FILE *fp = fopen(".oscrc", "r");
	  if (fp)
	    {
	      fscanf(fp, "%s %d", &address[0], &port);
	      init_osc(address, port);
	      printf("Open Sound Control (OSC) [%s]:[%d]\n", address, port);
	    }
	  else
	    {
	      printf("No .oscrc file: Need destination for OSC messages\n");
	      gOsc = 0;
	    }
	}

      plan  = fftw_plan_r2r_1d(dsize, din, dout, kind,
			       FFTW_DESTROY_INPUT|FFTW_MEASURE );

      if (!plan) { cout << "No plan created\n"; return;	}
      firsttime = false;
    }
  fftw_execute(plan);
}

/*
 * analyze() Finds the largest frequency component of
 * the audio sample and plays the nearest MIDI note.
 *
 */

static int lastnote = -1;
static int noActivity = 0;

double *scaleAudio(short *buffer)
{
  int i;
  for (i = 0; i < FFT_SIZE; i++)
    {
      din[i] = ((double)(buffer[i]))/gScaleAudio;
    }
  return din;
}

void analyze(short *buffer)
{
  gWhere = 1;
  scaleAudio(buffer);
  gWhere = 2;
  drawAmplitude(din);
  gWhere = 3;
  bandpass(din, dfilter, FFT_SIZE);
  gWhere = 4;
  dofft(dfilter, dout, FFT_SIZE);
  gWhere = 5;
  fold(dout, FFT_SIZE);
  gWhere = 6;

  double last = 0.0;
  for (int i=3; i<FFT_SIZE/4; i++)
    {
       if ( dout[i] > gDetect && dout[i+2] > gDetect )
	{
	  last = i + 1;
	}
       else if ( dout[i] > gDetect )
	 { 
	   last = i;
	 }
       else if ( dout[i] > (gDetect-20) && dout[i+1] > (gDetect-20) )
	 {
	  last = i + 0.5;
	 }
       else
	 {
	   continue;
	 }
       // We only get here if a high-amplitude component was seen 
       checkBirdCount(last); // Display the bird count
    }
  gWhere = 3;
  fft2note(last);  // This will also play the note (see fft2note.c)

  if (gAct != gActualFrequency)
    {
      gActualFrequency = gAct;
      gNearestMusicalFrequency = gNear;
      gNearestMIDINote = gNMIDI;
      if (gOsc) {
	send_osc(gActualFrequency,gNearestMusicalFrequency,gNearestMIDINote);
      }
      if (gSerial)
	{
	  char x = 'A' + (gActualFrequency/50);
	  printf("[%c(%d)%d]",x,x,gActualFrequency);
	  sendSerial(x);
	}

      if (gColors)
	{
      if (gActualFrequency < 180)     { colorOff(); }
      else if (gActualFrequency < 300) { colorOn(RED); }
      else if (gActualFrequency < 500) { colorOn(YELLOW); }
      else if (gActualFrequency < 900) { colorOn(GREEN); }
      else { colorOn(BLUE); }
	} else { colorOff(); }
    }
  gWhere = 10;
  cout << flush;
}

short *getAudio(void);  // From theremin.cpp

double *fft(void)
{
  scaleAudio(getAudio());
  bandpass(din, dfilter, FFT_SIZE);
  dofft(dfilter, dout, FFT_SIZE);
  fold(dout, FFT_SIZE);
  return dout;
}


bool select_device(int * devId)
{
	int count = waveInGetNumDevs();
	int i;

	WAVEINCAPS caps;
#ifdef DEBUG
	cout << "devID is " << *devId << " out of 0-" << (count-1) << endl;
#endif
	if (count == 1) { *devId = count - 1; return true; }
        if (*devId > count-1)
	  {
	    cout << "Audio device ("<<*devId<<") is out of range 0-"<< count-1<<endl;
	    return false;
	  }
        if (*devId > -1)
	  {
	    if (waveInGetDevCaps(*devId, &caps, sizeof(caps)) == MMSYSERR_NOERROR)
		{
#ifdef DEBUG
		  cout << caps.szPname << " selected" << endl;
#endif
                  return true;
		}

	  }

	UINT index = 1;
	for (i = 0; i < count; i++)
	{
		if (waveInGetDevCaps(i, &caps, sizeof(caps)) == MMSYSERR_NOERROR)
		{
			cout << "\t" << index << ") " << caps.szPname << endl;
			index++;
		}
	}

	cout << "Select Wave Device: ";
	cin >> i;

	while (i < 0 || i >= index)
	{
		cout << "Invalid Device Selection" << endl;
		cout << "Select Wave Device: ";
		cin >> i;
	}

	*devId = i - 1;
	return true;
}

bool open_device(UINT devId, LPHWAVEIN device)
{
	WAVEFORMATEX format;
	format.cbSize = 0;
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = 1;
	format.wBitsPerSample = 16;
	format.nSamplesPerSec = SAMPLE_RATE;
	format.nBlockAlign = format.nChannels * (format.wBitsPerSample / 8);
	format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;

	MMRESULT err = waveInOpen(device, WAVE_MAPPER, &format, 0, 0,
				  CALLBACK_NULL|WAVE_FORMAT_DIRECT );
	if ( err == MMSYSERR_NOERROR )
	  return true;
	cout << "ERROR: waveInOpen()  " << err << endl;
	return false;
}




