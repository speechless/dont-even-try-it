#pragma once

namespace cfg {
	int getNetworkCfg (std::string &http_port, std::string &deti_port, std::string &mc_addr, std::string &mc_port);

	std::string GetAdminPassword();
	std::string GetHTTPdns();
	std::string GetLoginMessage();
}