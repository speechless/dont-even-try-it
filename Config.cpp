#include "stdafx.h"


int GetConfig (std::string &http_port, std::string &deti_port, std::string &mc_addr, std::string &mc_port)
{
	std::fstream file;
	file.open("config/deti.properties", std::ios::in);

	if (!file.good()) {
		file.close();
		return -1;
	}

	http_port.clear();
	deti_port.clear();
	mc_addr.clear();
	mc_port.clear();

	while (!file.eof())
	{
		std::string buffer;
		buffer.resize(512);

		file.getline(&buffer[0],buffer.size());

		size_t found = buffer.find("http-port=");
		if (found == 0) {
			http_port = buffer.substr(std::string("http-port=").length());
		}

		found = buffer.find("deti-port=");
		if (found == 0) {
			deti_port = buffer.substr(std::string("deti-port=").length());
		}

		found = buffer.find("mc-address=");
		if (found == 0) {
			mc_addr = buffer.substr(std::string("mc-address=").length());
		}

		found = buffer.find("mc-port=");
		if (found == 0) {
			mc_port = buffer.substr(std::string("mc-port=").length());
		}
	}

	if (http_port.empty() || deti_port.empty() || mc_addr.empty() || mc_port.empty())
	{
		return 1;
	}
	
	return 0;
}


std::string GetLoginMessage()
{
	std::fstream file;
	file.open("config/deti.properties", std::ios::in);

	if (!file.good()) {
		file.close();
		return std::string();
	}

	while (!file.eof())
	{
		std::string buffer;
		buffer.resize(512);
		file.getline(&buffer[0],buffer.size());
		size_t found = buffer.find("login-message=");
		if (found == 0) {
			file.close();
			return (buffer.substr(std::string("login-message=").length()));
		}
	}

	return std::string();
}