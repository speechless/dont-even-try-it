#include "stdafx.h"

#include "deti.pb.h"
#include "Firewall.h"
#include "WebHostEmulator.h"

#define __FIREWALL

int main(int argc, char * argv[])
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	// Initialize Winsock
	WSADATA wsaData;

	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
#ifdef __DEBUG
		printf("WSAStartup failed with error: %d\n", iResult);
#endif
		return 1;
	}

	WebHostEmulator emu;
	Firewall firewall;
	emu.start(std::string("100"));	
	firewall.start("99", "127.0.0.1", "25566");

	while(1)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	WSACleanup();
	google::protobuf::ShutdownProtobufLibrary();

	return 0;
}