#ifndef INCLUDED_GUEST_H
#define INCLUDED_GUEST_H
#include <iostream>
#include "../ip/PacketListener.h"

namespace mls {

class MLSStreamListener : public StreamListener{ 
protected:

    virtual void ProcessMessage( const mls::char *m,
				 const IpEndpointName& remoteEndpoint ) = 0;
    
public:
    virtual void ProcessPacket( const char *data, int size, 
			const IpEndpointName& remoteEndpoint )
    {
        mls::ReceivedData p( data, size );
	ProcessMessage( ReceivedMessage(p), remoteEndpoint );
    }
};

} // namespace mls

int midiSend(int dev, int command, int note, int velocity);

int openMidiOut(int outID);

#endif /* INCLUDED_GUEST_H */
