rm bin/OscUnitTests.exe
rm bin/OscDump.exe
rm bin/OscSendTests.exe
rm bin/OscReceiveTest.exe

C:/cygwin/bin/g++ tests/OscUnitTests.cpp osc/OscTypes.cpp osc/OscReceivedElements.cpp osc/OscPrintReceivedElements.cpp osc/OscOutboundPacketStream.cpp -Wall -I. -lws2_32 -o bin/OscUnitTests.exe

C:/cygwin/bin/g++ examples/OscDump.cpp osc/OscTypes.cpp osc/OscReceivedElements.cpp osc/OscPrintReceivedElements.cpp ip/win32/NetworkingUtils.cpp ip/win32/UdpSocket.cpp -Wall -I. -lws2_32 -lwinmm -o bin/OscDump.exe

C:/cygwin/bin/g++ examples/SimpleSend.cpp osc/OscTypes.cpp osc/OscOutboundPacketStream.cpp ip/win32/NetworkingUtils.cpp ip/win32/UdpSocket.cpp ip/IpEndpointName.cpp -Wall -I. -lws2_32 -lwinmm -o bin/SimpleSend.exe

C:/cygwin/bin/g++ examples/SimpleReceive.cpp osc/OscTypes.cpp osc/OscReceivedElements.cpp ip/win32/NetworkingUtils.cpp ip/win32/UdpSocket.cpp -Wall -I. -lws2_32 -lwinmm -o bin/SimpleReceive.exe

C:/cygwin/bin/g++ tests/OscSendTests.cpp osc/OscTypes.cpp osc/OscOutboundPacketStream.cpp ip/win32/NetworkingUtils.cpp ip/win32/UdpSocket.cpp ip/IpEndpointName.cpp -Wall -I. -lws2_32 -lwinmm -o bin/OscSendTests.exe

C:/cygwin/bin/g++ tests/OscReceiveTest.cpp osc/OscTypes.cpp osc/OscReceivedElements.cpp ip/win32/NetworkingUtils.cpp ip/win32/UdpSocket.cpp -Wall -I. -lws2_32 -lwinmm -o bin/OscReceiveTest.exe

./bin/OscUnitTests.exe