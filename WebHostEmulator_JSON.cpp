#include "stdafx.h"
#include "WebHostEmulator.h"


std::string WebHostEmulator::jsonGet(const std::string &request, std::string &data)
{
	if (request == "/whoisonline.virtual_json")
	{
		data = jsonWhoIsOnline();
		return std::string("application/json");
	}

	return std::string("text/html");
}


std::string WebHostEmulator::jsonWhoIsOnline ()
{
	std::vector <std::string> username;
	std::vector <unsigned int> time;
	std::string serial = "[";

	loginDatabase->PollUserTimer(username, time);

	for (unsigned int i = 0; i < username.size(); i++) {
		std::string buffer;
		std::stringstream ss;

		int c_time = time.at(i) / 3600;
		if (c_time != 0) {
			std::stringstream ss;
			ss << c_time;
			ss >> buffer;
			buffer.append(" Hour(s)");
		}
		else {
			c_time = time.at(i) / 60;
			if (c_time != 0) {
				std::stringstream ss;
				ss << c_time;
				ss >> buffer;
				buffer.append(" Minutes(s)");
			}
			else {
				std::stringstream ss;
				ss << time.at(i);;
				ss >> buffer;
				buffer.append(" Seconds(s)");
			}
		}

		serial.append("{\"player\":\"" + username.at(i) + "\",\"uptime\":\"" + buffer + "\"}");
		if (i < username.size()-1) serial.append(",");
	}

	serial.append("]");
	return serial;
}