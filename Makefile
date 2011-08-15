OSCLIB = oscpackd/osc/OscTypes.cpp oscpackd/osc/OscOutboundPacketStream.cpp oscpackd/ip/win32/NetworkingUtils.cpp oscpackd/ip/win32/UdpSocket.cpp oscpackd/ip/IpEndpointName.cpp -lws2_32 -lwinmm

#OSCLIB = 
OBJECTS =  analyze.o midi.o fft2note.o
#CPP = g++
CPP = C:/MinGW/bin/g++
#CC = gcc
CC = C:/MinGW/bin/gcc

#CFLAGS = -mno-cygwin -fno-exceptions
#CFLAGS = -DNOTWEET -mno-cygwin -fno-exceptions
CFLAGS = -DNOTWEET -DNEEDSREENT -mno-cygwin -enable-auto-import
#CFLAGS = -DNOTWEET -mno-cygwin

efft	: serial.o tweet.o hist.o amp.o bird.o theremin.o winargs.o $(OBJECTS) resource.o
	$(CPP) -o efft -I./oscpackd hist.o amp.o bird.o resource.o winargs.o theremin.o tweet.o serial.o $(CFLAGS) $(OSCLIB) $(OBJECTS) -LC:/MinGW/lib -L. -lwinmm -lws2_32 libfftw3-3.dll -lgdi32 inpout32.dll
	./efft -o --level 100 --scale chromatic


theremin	: theremin.cpp $(OBJECTS)
	$(CPP) -o theremin theremin.cpp $(OBJECTS) -LC:/MinGW/lib -lwinmm -lws2_32 libfftw3-3.dll

try	: try.cpp
	$(CPP) -DSTANDALONE -o try try.cpp -lgdi32

fft	: hist.o theremin.o winargs.o $(OBJECTS) resource.o
	$(CPP) -o fft hist.o resource.o winargs.o theremin.o $(OBJECTS) -LC:/MinGW/lib -lwinmm -lws2_32 libfftw3-3.dll -lgdi32


fftplus	: hist.o amp.o theremin.o winargs.o $(OBJECTS) resource.o
	$(CPP) -o fft hist.o amp.o resource.o winargs.o theremin.o $(OBJECTS) -LC:/MinGW/lib -lwinmm -lws2_32 libfftw3-3.dll -lgdi32

ain	: ain.cpp midi.o fft2note.o
	$(CPP) -o ain ain.cpp fft2note.o midi.o -LC:/MinGW/lib -lwinmm -lws2_32 libfftw3-3.dll


midi.o	: midi.c theremin.h
	$(CC) -c $(CFLAGS) midi.c

theremin.o	: theremin.cpp theremin.h
	$(CPP) -c $(CFLAGS) theremin.cpp

winargs.o	: winargs.cpp
	$(CPP) -c $(CFLAGS) winargs.cpp

fft2note.o	: fft2note.c theremin.h 
	$(CC) -c fft2note.c

resource.o	: resource.rc
	windres -o resource.o resource.rc

analyze.o	: analyze.cpp theremin.h
	$(CPP) -c $(CFLAGS) analyze.cpp

amp.o	: amp.cpp
	$(CPP) -c $(CFLAGS) amp.cpp

bird.o	: bird.cpp
	$(CPP) -c $(CFLAGS) bird.cpp

hist.o	: hist.cpp
	$(CPP) -c $(CFLAGS) hist.cpp

digit.o	: digit.cpp
	$(CPP) -c $(CFLAGS) digit.cpp

tweet.o	: tweet.cpp
	$(CPP) -c $(CFLAGS) tweet.cpp

dtest	: digit.cpp
	$(CPP) -DSTANDALONE -o dtest $(CFLAGS) digit.cpp

theremin.h	:	fftnotes.h
	touch theremin.h



clean	:
	rm -rf *.o efft.exe *~ \#*

bc	: bc.cpp theremin.o analyze.o fft2note.o midi.o digit.o
	$(CPP) -o bc bc.cpp analyze.o theremin.o fft2note.o midi.o -LC:/MinGW/lib -lwinmm -lws2_32 libfftw3-3.dll


segments	: seg2.cpp
	$(CPP) -o segments seg2.cpp inpout32.dll



