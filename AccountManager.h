#pragma once
class AccountManager
{
public:
	AccountManager(void);
	~AccountManager(void);

	int SetDatabase(const std::string Database);

	int Change(std::string change, std::string Username, const std::string Password, std::string Email);
	int Delete(std::string Username);
	int GetUserPass(std::string Email, std::string &Username, std::string &Password);
	int Login(std::string Username, const std::string Password);
	int Register(std::string Username, const std::string Password, std::string Email);

private:
	std::string _database;
	std::mutex m_file;
};

