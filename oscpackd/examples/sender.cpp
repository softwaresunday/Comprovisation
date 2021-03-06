#include <iostream>
#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


#define ADDRESS "127.0.0.1"
#define PORT 57122

#define OUTPUT_BUFFER_SIZE 1024

int main(int argc, char* argv[])
{
    setbuf(stdout,NULL);
    UdpTransmitSocket transmitSocket( IpEndpointName( ADDRESS, PORT ) );
    
    char buffer[OUTPUT_BUFFER_SIZE];

    int    ids[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int levels[] = { 0, 1, 2, 3 };

    srand(time(NULL));
    while(1)
    {
        Sleep(2000);
        int i = rand()%(sizeof(ids)/sizeof(int));
        int l = rand()%(sizeof(levels)/sizeof(int));
        osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );
        p  << osc::BeginMessage( "/activity" ) << ids[i] << levels[l]
           << osc::EndMessage;
	std::cout << "("<<ids[i]<<", "<<levels[l]<<") ";
        transmitSocket.Send( p.Data(), p.Size() );
    }
}



