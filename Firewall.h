#pragma once
#include "LoginDatabase.h"

class Firewall
{
public:
	Firewall(void);
	~Firewall(void);

	int start(const std::string ListenPort, const std::string ForwardAddress, const std::string ForwardPort, LoginDatabase *database);

private:
	void HandleListen(const std::string ForwardAddress, const std::string ForwardPort);

	void HandleClient(SOCKET *ClientSocket, SOCKET *ServerSocket, const std::string IP_Addr);
	void HandleServer(SOCKET *ServerSocket, SOCKET *ClientSocket);

	int GetUsername(const std::string data, std::string & username);
	std::string GenKickPacket(const std::string message);

private:
	SOCKET ListenSocket;
	bool KeepAlive;
	std::mutex m_KeepAlive;

	LoginDatabase *loginDatabase;
};