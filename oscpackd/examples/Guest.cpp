
/* 
 * Process received OSC messages using oscpack.
 *   play sound palette.
 */
#include <iostream>
#include <stdlib.h>
#include <sys/time.h>
#include "pthread.h"
#include "q.h"

typedef struct { int a1, a2; } activity_t;
static q_type<activity_t *> aQueue;

extern "C" char *basename(char *);

#include "../osc/OscReceivedElements.h"
#include "../osc/OscPacketListener.h"
#include "../ip/UdpSocket.h"

#include <winsock.h>

#define PORT 57122
#define DEBUG 1

int  playRiff(int square, int level);
void shiftRiffs(void);
int  midiNote(int note, int velocity);
void midiPatch(int patch);
void midiStep(int duration);
int  openMidiOut(int outID);
char *designFile(void);


struct style {int n[20][40];}; // Ten riffs of ten note:durations pairs each
typedef struct style style_t;

int     squares[10];     // Each square (may) have a level
int     legato[10];      // Each square (may) have a random legato magnitude

static style_t sq[10];           // Each square has a style

static time_t lasttime;
static int    comingfast;

static struct timeval lt;
static struct timeval tt;
static bool life;

// OSC message processer puts events onto a queue, which
// automatically flushes when too many events are arriving.


class ExamplePacketListener : public osc::OscPacketListener {
protected:

    virtual void ProcessMessage( const osc::ReceivedMessage& m,
				 const IpEndpointName& remoteEndpoint )
    {
      osc::ReceivedMessageArgumentStream args = m.ArgumentStream();
      osc::int32 a1;
      osc::int32 a2;
      args >> a1 >> a2 >> osc::EndMessage;

      activity_t *ap = (activity_t *)malloc(sizeof(activity_t));
      ap->a1 = a1;
      ap->a2 = a2;
      aQueue.q(ap);
    }
};


// Asynchronous MIDI player pulls events off the Activity queue

void *player(void *arg)
{
  life = true;
  while(life)
    {
      activity_t *xp = aQueue.deq();
      if (xp)
	{
	  playRiff(xp->a1, xp->a2);
	  shiftRiffs();
	  free(xp);
	}
      else { Sleep(100); }
    }
}

#include <stdio.h>
#include <string.h>
#include <time.h>

static char word[128];
static char final[128];
static int  fint;

char *getTag(FILE *fp)
{
  if ( fscanf(fp, " <%s>", word) == 1)
    {
      if (word[strlen(word)-1] =='>') { word[strlen(word)-1] = 0; }
      if (word[0] =='/')
	{
	  sprintf(final, "end %s", &word[1]);
	  return final;
	}
      return word;
    }
  else
    {
      return "eof";
    }
}

int     patches[] = { 30, 40, 50, 60, 70, 80, 90 };
int     nextpatch = 0;
void randomPatch(void)
{
  //  nextpatch++;
  nextpatch = patches[rand()%(sizeof(patches)/sizeof(int))];
  std::cout << "PATCH " << nextpatch << "\n";
  midiPatch(nextpatch);
}

int getInt(FILE *fp)
{
  fint = -1;
  if ( fscanf(fp, " %d", &fint) == 1)
    {
      return fint;
    }
  return 0;
}


  

char *getNext(FILE *fp)
{
  fscanf(fp, "%s", word);

  int i;
  for(i=0;i<128;i++) final[0] = 0;
  i = 1;
  char *cp = word;
  if (*cp == '<')
    {
      cp++;
      if (*cp == '/')
	{
	  strcpy(final, "end ");
	  cp++;
          i = 2;
	}
      while(*cp != 0) 
	{
	  if(*cp == '>' || *cp == ' ')
	    {
	      *cp = 0;
	    }
	  cp++;
	}
      strcat(final, &word[i]);
    }
  else
    {
      strcpy(final, word);
    }
  return final;
}

// Use the guest's ID to retrieve their Studio Design XML file

int readDesign(char *design)
{
    const char *perl = "C:/Perl/bin/perl.exe -w";
    const char *wget = "C:/cygwin/home/peterr/GCC/oscpack/wget.pl";
    const char *host  = "http://info/Museum/Design";
    char filename[1024];
    char buffer[1024];
    strcpy(filename, design);
    sprintf(buffer, "%s %s %s/%s", perl, wget, host, filename);
//  std::cout << "COMMAND: " << buffer << "\n\n";
    std::cout << "wget [" << filename << "]\n";
    if (system(buffer)) { std::cout << "ERROR: " << buffer << "\n"; }
    std::cout << "after WGET\n";

    int web = 0;
    int looping = 0;
    FILE *fp = fopen(filename, "r");
    if (fp) { std::cout << "open " << filename << " file pointer is not zero\n"; }
    int i = 0;

    int atLeastOneRiff = 0;
 tryagain:   // Come back here if web file did not contain a design
    while(!feof(fp))
      {
        char cmd[128];
        sprintf(cmd, getTag(fp));

	if (!strcmp("eof",cmd)) break;

	if ((looping++ > 100) && (web == 0)) break;

        if (!strcmp(cmd, "studio"))
	  {
#ifdef DEBUG
	    std::cout << "reading a studio design\n";
#endif         
	    web = 1;  // It worked!
	    int studioloop = 1;
            while (studioloop)
	      {
		sprintf(cmd, getTag(fp));
		std::cout << "getTag " << cmd << "\n";
		if (!strcmp(cmd, "square"))
		  {
		    int square = getInt(fp);
                    int squareloop = 1;
                    int riff = 0;
		    while (squareloop)
		      {
			sprintf(cmd, getTag(fp));
			if (!strcmp(cmd, "level"))
			  {
			    int lev = getInt(fp);
			    squares[square] = lev;
			  }
			if (!strcmp(cmd, "legato"))
			  {
			    legato[square] = getInt(fp);
			  }
			if (!strcmp(cmd,"riff"))
			  {
			    int j = 0;
                            int n;
			    while ( (n = getInt(fp)) != 0)
			      {
                                atLeastOneRiff++;
				sq[square].n[riff][j++] = n;    // NOTE
				sq[square].n[riff][j++] = 100;  // DURATION
			      }
			    riff++;
			  }

			if (!strcmp(cmd, "end square"))
			  {
			    squareloop = 0;
			  }
		      } // end level
		  } // end if square
		if (!strcmp(cmd, "end studio"))
		  {
		    std::cout << "end studio\n";
		    studioloop = 0;
		  }
                 
		    std::cout << "at bottom of squares loop\n";
	      } // while squares
		    std::cout << "at bottom of if studio\n";
	  }  // end if studio
	    std::cout << "at bottom of EOF file loop\n";
	    std::cout << feof(fp) << "\n";
      } // while data in file
    std::cout << " After feof(fp) \n";

    if (!web || !atLeastOneRiff)
      {
        fclose(fp);
	std::cout << "did not get design from website\n";
	fp = fopen("C:/cygwin/home/peterr/GCC/oscpack/local/des3.xml", "r");
        if (!fp) { std::cout << filename << " not found\n"; }
        web = 1;
        goto tryagain;
      }

#ifdef DEBUG
  int f;
  for (f=0;f<9;f++){std::cout<<"square "<<f<<" level "<<squares[f]<<"\n";}
  for (f=0;f<9;f++){std::cout<<"1st note of riff "<<sq[f].n[0][0]<<"\n";}
  for (f=0;f<9;f++){std::cout<<"legato "<< f << " = " << legato[f] <<"\n";}
#endif
}

int main(int argc, char* argv[])
{

    setbuf(stdout,NULL);

    ExamplePacketListener listener;

    UdpListeningReceiveSocket s(
            IpEndpointName( IpEndpointName::ANY_ADDRESS, PORT ),
            &listener );

    int id = argc > 1 ? atoi(argv[1]) : 3 ;

    std::cout << "calling readDesign with " << designFile() << "\n";
    readDesign(designFile());
    std::cout << "returned from readDesign\n";

    srand(time(0));

    openMidiOut(0);
    std::cout << "Midi Opened\n";

    pthread_t thread;
    pthread_attr_t pthread_custom_attr;
    pthread_attr_init(&pthread_custom_attr);
    pthread_create(&thread, &pthread_custom_attr,player,(void *)NULL);
    std::cout << "Player thread started\n";


    std::cout << "press ctrl-c to end\n";

    s.RunUntilSigInt();
    return 0;
}

int playRiff(int square, int level)
{
  int r = rand();

  int startriff = rand() % 3;
  int endriffs  = startriff + 1 + (rand() % 4);
  int volume    = 40 + level * 40;
      std::cout << "/activity " << square << " " << level
                << ":  Playing riffs " << startriff
                << " through " << endriffs
                << " at volume " << volume
		<< "\n";

      int riff = startriff;
      int note = 0;
      int chord = 0;
       while (sq[square-1].n[riff][note] && riff < endriffs)
	{
	  while (sq[square-1].n[riff][note])
	    {
              if (chord) { chord = chord + 3 + (rand()%2); }
	      else       { chord = sq[square-1].n[riff][note]; }
	      midiNote(chord,(volume>127?127:volume));
	      note++;
               
//      midiStep(sq[square-1].n[riff][note++]+(rand()%(50+legato[square-1])));
	      if (rand()%4)
		{
		  midiStep((legato[square]/2) + (rand()%(50+legato[square-1])));
                  chord = 0;
		}
              note++;
	    }
	  riff++;    // Next riff
	  note = 0; // First note
	}
}

static int patch = 0;

void shiftRiffs(void)
{
  //  if (rand()%10 == 0) { randomPatch(); }
  if (rand()%10 == 0)
    {
      int inc;
      if (rand()%2 == 1)
	{
	  inc = -8;
	  std::cout << "shifting down\n";
	}
      else
	{
	  inc = 7;
	  std::cout << "shifting up\n";
	}
      int square = 0;
      int riff = 0;
      int endriffs = 7;
      int note = 0;
      while (square < 9)
	{
	  while (sq[square].n[riff][note] && riff < endriffs)
	    {
	      while (sq[square].n[riff][note])
		{
		  sq[square].n[riff][note] += inc;
		  note += 2;
		}
	      riff++;    // Next riff
	      note = 0; // First note
	    }
	  square++;
	  riff = 0;
	  note = 0;
	}
      std::cout << "3: " << sq[2].n[0][0] << " " << sq[2].n[0][2] << "\n";
    }
}

static char filename[256];

char * designFile(void)
{
    char ac[80];
    if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) {
        std::cout << "Error " << WSAGetLastError() <<
                " when getting local host name.\n";
        return NULL;
    }
    struct hostent *phe = gethostbyname(ac);
    if (phe == 0) {
        std::cout << "Yow! Bad host lookup.\n";
        return NULL;
    }

    if (phe->h_addr_list[0] != 0)
      {
        struct in_addr addr;
        memcpy(&addr, phe->h_addr_list[0], sizeof(struct in_addr));
        int i = 0;
        char *cp = "des";
        while(*cp) { filename[i++] = *cp++; }
	cp = inet_ntoa(addr);
	while(*cp)
	  {
	    if (*cp != '.') { filename[i++] = *cp; }
            cp++;
	  }
        cp = ".xml";
        while(*cp) { filename[i++] = *cp++; }
        filename[i] = NULL;
    }
    return filename;
}



