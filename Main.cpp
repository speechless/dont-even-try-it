#include "stdafx.h"

#include "deti.pb.h"
//#include "AccountManager.h"
#include "WebHostEmulator.h"

int main(int argc, char * argv[])
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	WebHostEmulator emu;

	emu.start(std::string("99"));

	while(1)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return 0;
}