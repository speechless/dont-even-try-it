#include "stdafx.h"

#include "WebHostEmulator.h"

int main(int argc, char * argv[])
{
	WebHostEmulator emu;

	emu.start(std::string("100"));

	while(1)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return 0;
}