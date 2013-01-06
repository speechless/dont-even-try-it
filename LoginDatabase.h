#pragma once
class LoginDatabase
{
public:
	LoginDatabase(void);
	~LoginDatabase(void);

	int init(const unsigned int Timeout);
	int deinit();

	int AddUser(std::string username, const std::string ip_address);
	int PollUser(std::string username, std::string &ip_address);
	int RemoveUser(std::string username);

private:
	void HandleTimeout();

private:
	struct session {
		std::string username;
		std::string ip_address;
		unsigned int timeout;
	};

	std::list <session> database;
	std::mutex m_database;
	unsigned int timeout;
	
	bool KeepAlive;
	std::mutex m_KeepAlive;
};

