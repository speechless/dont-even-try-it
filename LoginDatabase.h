#pragma once
class LoginDatabase
{
	/*Config and core system*/
public:
	LoginDatabase(void);
	~LoginDatabase(void);

	int init(const unsigned int Timeout);
	int deinit();

private:
	void HandleTimeout();
	void HandleTimer();

private:
	bool KeepAlive;
	std::mutex m_KeepAlive;

	/* Authentication addon*/
public:
	int AddUser(std::string username, const std::string ip_address);
	int PollUser(std::string username, std::string &ip_address);
	int RemoveUser(std::string username);
private:
	struct session {
		std::string username;
		std::string ip_address;
		unsigned int timeout;
	};

	std::list <session> database;
	std::mutex m_database;
	unsigned int timeout;

	/* Connection timer addon*/
public:
	int AddUserTimer(std::string username);
	int PollUserTimer(std::vector <std::string> &username, std::vector <unsigned int> &timers);
	int RemoveUserTimer(std::string username);
private:
	struct connectionTimer {
		std::string username;
		unsigned int uptime;
	};

	std::list <connectionTimer> userUptime;
	std::mutex m_userUptime;
};

