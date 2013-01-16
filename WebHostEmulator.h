#pragma once
#include "AccountManager.h"
#include "LoginDatabase.h"

class WebHostEmulator
{
public:
	WebHostEmulator(void);
	~WebHostEmulator(void);

	int start(const std::string port, LoginDatabase *loginDatabase);

private:
	AccountManager accountManager;
	LoginDatabase *loginDatabase;

	SOCKET ListenSocket;
	bool KeepAlive;
	std::mutex m_KeepAlive;

	std::string Root;

private: // WebHostEmulator.cpp
	void HandleListen();
	void HandleClient(SOCKET* socket, const std::string ip_addr);

	int GetPage(const std::string &page, std::string &content);
	std::string GetBody(const std::string &message);

	std::string ParseField(const std::string &data, const std::string &field);
	int parseHTTP (const std::string recvbuf, std::string &message, std::string &overflow);

	std::string BuildHeader (const std::string &content, const int &StatusCode, const std::string &ContentType, const bool &AllowCache);
	std::string BuildResult (const std::string &message, const std::string &page);

	std::string GetReq(const std::string &page, std::string &content);
	std::string PostReq(const std::string &request, const std::string &parameters, const std::string ip_addr);

private: // WebHostEmulator_Post.cpp
	std::string postLogin	(const std::string &parameters, const std::string ip_addr);
	std::string postRegister(const std::string &parameters);
	std::string postForgot	(const std::string &parameters);
	std::string postChangePassword	(const std::string &parameters);
	std::string postDelete	(const std::string &parameters);
};
