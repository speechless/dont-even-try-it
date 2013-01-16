#include "stdafx.h"
#include "Config.h"

// Local definitions
#define CONFIG_FILE_PATH "config/deti.properties"

// Local declarations
::std::string strConfigReadFile (::std::string const &path);
::std::string strConfigGetValue (::std::string const &data, ::std::string const &search, ::std::string const &delimiter);

// Global function definitions ......................................
int cfg::getNetworkCfg (std::string &http_port, std::string &deti_port, std::string &mc_addr, std::string &mc_port)
{
	std::string data = strConfigReadFile(CONFIG_FILE_PATH);

	if (data.empty() == true)
		return -1;

	http_port.clear(); deti_port.clear(); mc_addr.clear(); mc_port.clear();

	http_port = strConfigGetValue(data, "http-port=", "\r\n");
	deti_port = strConfigGetValue(data, "deti-port=", "\r\n");
	mc_addr = strConfigGetValue(data, "mc-address=", "\r\n");
	mc_port = strConfigGetValue(data, "mc-port=", "\r\n");

	if (http_port.empty() || deti_port.empty() || mc_addr.empty() || mc_port.empty())
		return 1;

	return 0;
}


std::string cfg::GetAdminPassword()
{
	std::string data = strConfigReadFile(CONFIG_FILE_PATH);

	if (data.empty() == true)
		return std::string();

	return strConfigGetValue(data, "admin-pass=", "\r\n");
}


std::string cfg::GetLoginMessage()
{
	std::string data = strConfigReadFile(CONFIG_FILE_PATH);

	if (data.empty() == true)
		return std::string();

	return strConfigGetValue(data, "login-message=", "\r\n");
}


std::string cfg::GetHTTPdns()
{
	std::string data = strConfigReadFile(CONFIG_FILE_PATH);

	if (data.empty() == true)
		return std::string();

	return strConfigGetValue(data, "http-dns=", "\r\n");
}


// Local function definitions .................................................
::std::string strConfigReadFile (::std::string const &path)
{
	::std::fstream file;
	file.open(path, ::std::ios::in | ::std::ios::binary);

	if (!file.good()) {
		file.close();
		return ::std::string();
	}

	file.seekg (0, ::std::ios::end);
	int length = (int)file.tellg();
	file.seekg (0, ::std::ios::beg);

	::std::string data;
	data.resize(length);

	file.read(&data[0], length);

	file.close();

	return data;
}


::std::string strConfigGetValue (::std::string const &data, ::std::string const &search, ::std::string const &delimiter)
{
	::size_t found = data.find(search);

	if (found == ::std::string::npos)
		return ::std::string();

	::size_t delim = data.find(delimiter, found);

	if (delim == ::std::string::npos)
		return ::std::string();

	return ::std::string(
		data,
		found + search.length(),
		delim - found - search.length()
		);
}