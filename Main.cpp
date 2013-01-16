#include "stdafx.h"

#include "Config.h"
#include "deti.pb.h"
#include "Firewall.h"
#include "LoginDatabase.h"
#include "WebHostEmulator.h"


#ifndef __DEBUG
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char * argv[])
#endif
{
	std::string http_port, deti_port, mc_addr, mc_port;
	
	if (cfg::getNetworkCfg(http_port, deti_port, mc_addr, mc_port) != 0) {
#ifdef __DEBUG
		printf("Configuration missing.\n");
#endif
		return 1;
	}

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
	emu.start(std::string(http_port), &login);
	firewall.start(deti_port, mc_addr, mc_port, &login);

	while(1)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	WSACleanup();
	google::protobuf::ShutdownProtobufLibrary();

	return 0;
}