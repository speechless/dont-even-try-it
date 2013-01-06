#pragma once
class Firewall
{
public:
	Firewall(void);
	~Firewall(void);

	int start(const std::string ListenPort, const std::string ForwardAddress, const std::string ForwardPort);
private:
	void HandleListen(const std::string ForwardAddress, const std::string ForwardPort);

	void HandleClient(SOCKET *ClientSocket, SOCKET *ServerSocket, const std::string IP_Addr);
	void HandleServer(SOCKET *ServerSocket, SOCKET *ClientSocket);

private:
	SOCKET ListenSocket;
	bool KeepAlive;
	std::mutex m_KeepAlive;
};