#include "stdafx.h"

#include "deti.pb.h"
#include "Firewall.h"
#include "LoginDatabase.h"
#include "WebHostEmulator.h"


int main(int argc, char * argv[])
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	WSADATA wsaData;

	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
#ifdef __DEBUG
		printf("WSAStartup failed with error: %d\n", iResult);
#endif
		return 1;
	}

	Firewall firewall;
	LoginDatabase login;
	WebHostEmulator emu;
	
	login.init(300);
	emu.start(std::string("99"), &login);
	firewall.start("98", "127.0.0.1", "25566", &login);

	while(1)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	WSACleanup();
	google::protobuf::ShutdownProtobufLibrary();

	return 0;
}