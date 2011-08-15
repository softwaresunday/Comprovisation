using namespace std;

#define DECLARE 1
#include "theremin.h"
#include <getopt.h>

#include "oscpackd/osc/OscOutboundPacketStream.h"
#include "oscpackd/ip/UdpSocket.h"


#define OUTPUT_BUFFER_SIZE 1024

static osc::OutboundPacketStream *gop;
static UdpTransmitSocket *transmitSocket;
static char buffer[OUTPUT_BUFFER_SIZE];
extern "C" void initSerial(char *dev);
void colorOff(void);

void init_osc(char *address, int port)
{
  if (gOsc)
    {
    transmitSocket = new UdpTransmitSocket(IpEndpointName( address, port ));
    gop = new osc::OutboundPacketStream(buffer, OUTPUT_BUFFER_SIZE);
    }
}

void send_osc(osc::int32 freq, osc::int32 eqfreq,osc::int32 midinote)
{
  if (gOsc)
    {
  char cp[1024];
  (*gop) << osc::BeginMessage("/theremin") << freq << eqfreq << midinote << osc::EndMessage;
  transmitSocket->Send( gop->Data(), gop->Size() );
  gop->Clear();
    }
}

void usage(void)
{
  cout << "usage: ain <cycles> --level <nn>\n"
	    << "\t--scaleAudio <short int scale factor>\n"
	    << "\t--delay <milliseconds between notes>\n"
	    << "\t--midi <midiDevice> --audio <audioDevice>\n"
	    << "\t--key {C,C#,D,D#,E,F,F#,G,Ab,A,Bb,B}\n"
	    << "\t--scale {major,minor,harmonic,melodic,pentatonic,chromatic}\n";
  exit(0);
}

static const struct option options[] =
{
  {"audio",      required_argument, NULL, 'a',},
  {"birds",      required_argument, NULL, 'b',},
  {"cycles",     required_argument, NULL, 'c'},
  {"delay",      required_argument, NULL, 'd'},
  {"help",             no_argument, NULL, 'h'},
  {"instrument", required_argument, NULL, 'i'},
  {"key",        required_argument, NULL, 'k'},
  {"level",      required_argument, NULL, 'l'},
  {"midi",       required_argument, NULL, 'm'},
  {"osc",              no_argument, NULL, 'o'},
  {"colors",           no_argument, NULL, 'p'},
  {"scaleAudio", required_argument, NULL, 'r'},
  {"scale",      required_argument, NULL, 's'},
  {   NULL,            no_argument, NULL,  0}
};

int string_to_enum(const char *str)
{
  if      (!stricmp(str, "major"))      return(major);
  else if (!stricmp(str, "minor"))      return(minor);
  else if (!stricmp(str, "harmonic"))   return(harmonic);
  else if (!stricmp(str, "melodic"))    return(melodic);
  else if (!stricmp(str, "pentatonic")) return(pentatonic);
  else if (!stricmp(str, "chromatic"))  return(chromatic);
  else if (!stricmp(str, "C"))	return(c);
  else if (!stricmp(str, "Db"))	return(db);
  else if (!stricmp(str, "C#"))	return(db);
  else if (!stricmp(str, "D"))	return(d);
  else if (!stricmp(str, "Eb"))	return(eb);
  else if (!stricmp(str, "D#"))	return(eb);
  else if (!stricmp(str, "E"))	return(e);
  else if (!stricmp(str, "F"))	return(f);
  else if (!stricmp(str, "F#"))	return(gb);
  else if (!stricmp(str, "Gb"))	return(gb);
  else if (!stricmp(str, "G"))	return(g);
  else if (!stricmp(str, "G#"))	return(ab);
  else if (!stricmp(str, "Ab"))	return(ab);
  else if (!stricmp(str, "A"))	return(a);
  else if (!stricmp(str, "Bb"))	return(bb);
  else if (!stricmp(str, "A#"))	return(bb);
  else if (!stricmp(str, "B"))	return(b);
}

static int wsaFirsttime = 1;
void getmyhost(void)
{
  WSAData wsaData;
  if (wsaFirsttime)
    {
      wsaFirsttime = 0;
      WSAStartup(MAKEWORD(1, 1), &wsaData);
        
      if (gethostname(gHostname, sizeof(gHostname)) == SOCKET_ERROR)
	{
	  cout << "gethostname() error " << WSAGetLastError() << endl;
	  strcpy(gHostname,"unknown");
	}
    }
}

int process_options(int argc, char *argv[])
{
  /* For convenience, there are defaults based on hostname */
  /* overridden by command line options and .thereminrc    */
  getmyhost();

  if      (!strcmp(gHostname,"NCMLS7352"))
    {
    gDetect = 100;
    gMIDIDev = 0;
    gDevId = 0;
    gDelay = 10;
    }
  else if (!strcmp(gHostname,"foo") || !strcmp(gHostname,"FOO"))
    {
      gMIDIDev = 8;
      gDevId = 0;
      gDetect = 120; 
      gScaleAudio = 48767.0;
    }
  else if (!strcmp(gHostname,"Francine-PC"))
    {
      gMIDIDev = 0;
      gDevId = 0; 
      gScaleAudio = 65534.0;
    }
  else if (!strcmp(gHostname,"sandstone"))  
    {
      gMIDIDev = 0;
      gDevId = 0;
    }

  /* Now process command line options */

  char c;
  int optindex;
  while((c=getopt_long(argc,argv,"a:c:d:hi:k:m:opr:s:u:",options,&optindex))!=-1)
    {
      switch (c)
	{
	case 'a':
	   gDevId = atoi(optarg);
	   break;
	case 'b':
	   gBirds= 1;
	   break;
	case 'd':
	  gDelay = atoi(optarg);
	  break;
	case 'k':
	  gKey = (enum key) string_to_enum(optarg);
	  break;
	case 'i':
	   gInstrument = atoi(optarg);
	   break;
	case 'l':
	   gDetect = atoi(optarg);
	   break;
       case 'm':
	 gMIDIDev = atoi(optarg);
	 break;
       case 'o':
	 gOsc = 1;
	 break;
       case 'p':
	 gColors = 1;
	 break;
	case 'r':
	  gScaleAudio = (double) atoi(optarg);
	  break;
	case 's':
	  gScale = (enum scale) string_to_enum(optarg);
	  break;
	case 'u':
	  gSerial = 1;
	  strncpy(gSerialPort,optarg,10);
	  break;
	default:
	  usage();
	  break;
	}
    }

  /* The remaining argument is a configuration file */
  /* containing (only) option/value pairs so there  */
  /* is no danger of infinite recursion.            */

  if (optind < argc)
    {
      ifstream cf;
      cf.open( argv[optind++], ios::in );
      if (cf)
	{
	  int  cargc;
	  char *cargv[24];
	  for (cargc=0; cargc<24; cargc++)
	    {
	      cargv[cargc]=(char *)malloc(128);
	    }
	  cargc = 0;
/*
  	  while (cf.fscanf("%s = %s", &cargv[cargc], &cargv[cargc+1]) == 2 )
	    {
	      cargc += 2;
	    }
	  process_options(cargc, cargv);
*/
	  cf.close();
	}
    }
}
  

short *getAudio(void)
{
static int initializeAudio = 1;
static HWAVEIN hDevice;
static WAVEHDR buffer;

  if (initializeAudio)
    {
      initializeAudio = 0;
      if (!select_device(&gDevId)) { exit(0); }
      if (open_device((UINT)gDevId, &hDevice))
	{
	  ZeroMemory(&buffer, sizeof(buffer));
	  buffer.lpData = (LPSTR)new BYTE[BUFFER_SIZE]; 
	  buffer.dwBufferLength = BUFFER_SIZE;
	  ZeroMemory(buffer.lpData, BUFFER_SIZE);
	}
      else
	{
	  printf("Failed to open Audio Device %d\n", gDevId);
	  gShutdown = true;
	}
    }
  if (gShutdown)
    {
      printf("Shutting down\n");
      waveInUnprepareHeader(hDevice, &buffer, sizeof(buffer));
      delete [] buffer.lpData;
      waveInClose(hDevice);
      exit(0);
    }
	  
  if (waveInPrepareHeader(hDevice, &buffer, sizeof(buffer))==MMSYSERR_NOERROR )
    {
      waveInAddBuffer(hDevice, &buffer, sizeof(buffer));
      waveInStart(hDevice);
      Sleep(24);
      while ((buffer.dwFlags & WHDR_DONE) != WHDR_DONE)
	{
	  Sleep(4);
	}
      waveInStop(hDevice);
      if (buffer.dwBytesRecorded == BUFFER_SIZE)
	{
	  return((short *)buffer.lpData);
	}
      else
	{
	  cout << "Audio input did not return a full buffer\n";
	  exit(0);
	}
    }
}

void initializeMIDI(void)
{
  if (! openMidiOut(gMIDIDev) )    /* INITIALIZE MIDI OUTPUT */
    {
      cerr << "Could not open midi Device " << gMIDIDev << endl;
      exit(0);
    }
  if (gMIDIDev == 9 && !stricmp(gHostname,"foo")) { resetGS(); }

  midiSilence();
  if (gInstrument != 1) cout << "Instrument " << gInstrument << endl;
  midiPatch(gInstrument);

#if ( DEBUG == 2 )
  midiNote(60, 100);
#endif
}

#include <signal.h>
void setBirdCount(int n);

void handler(int signum)
{
  cout << "Inside handler : "  << signum << endl;
  setBirdCount(0);
  midiSilence();
  cout << "Goodbye"  << endl;
  exit(0);
}

void memhandler(int signum)
{
  cout << "MemErr: "  << signum << " from "  << gWhere << endl;
  exit(0);
}



int startTheremin(int argc, _TCHAR* argv[])
{
  cout.setf(std::ios::unitbuf); /* No buffering on standard out */
  signal (SIGINT,  handler);
  signal (SIGTERM, handler);
  signal (SIGSEGV, memhandler);

  if (gSerial) { initSerial(gSerialPort); }
  colorOff();

  process_options(argc, argv);

  initializeMIDI();

  // Requiring this init call allows us to keep the code for
  // the fft-to-note translator separated from the application.
  // Fft2note could include "theremin.h", but then it would
  // become entwined. This one call tells fft2note everything
  // it needs to know (It doesn't need access to the FFT data).
  //
  // Any application can simply
  //   1) #include "fftnote.h" ( constants enums,FFT_SIZE,SAMPLE_RATE)
  //   1) #include "notes.h"   ( MIDI note data )
  //   2) Call init_fft2note() and fft2note() and
  //   3) Link with fft2note.o
  //

  init_fft2note(FFT_SIZE, SAMPLE_RATE, gKey, gScale);
//  cout << "Theremin initialized\n";
       
  return 0;
  cout << "Notreached\n";
  /* PROCESS DATA */

  static int showprogress = 0;

  while(gCycles < 0 || gCycles--)  // Count down or infinite (-1)
    {
      analyze(getAudio());
      if (showprogress++ > 20) { showprogress = 0; cout << " ."; }
    }
}


