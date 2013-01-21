#include "stdafx.h"
#include "Config.h"
#include "Firewall.h"

//#define __DEBUG  // Comment this out to disable debug text in console

Firewall::Firewall(void)
{
}


Firewall::~Firewall(void)
{
}


int Firewall::start(const std::string ListenPort, const std::string ForwardAddress, const std::string ForwardPort, LoginDatabase *database)
{
	loginDatabase = database;
	int iResult;

	ListenSocket = INVALID_SOCKET;
	
	m_KeepAlive.lock();
	KeepAlive = true;
	m_KeepAlive.unlock();

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, ListenPort.c_str(), &hints, &result);
	if ( iResult != 0 ) {
#ifdef __DEBUG
		printf("getaddrinfo failed with error: %d\n", iResult);
#endif
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
#ifdef __DEBUG
		printf("socket failed with error: %ld\n", WSAGetLastError());
#endif
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
#ifdef __DEBUG
		printf("bind failed with error: %d\n", WSAGetLastError());
#endif
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
#ifdef __DEBUG
		printf("listen failed with error: %d\n", WSAGetLastError());
#endif
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	std::thread thread(&Firewall::HandleListen, this, ForwardAddress, ForwardPort);
	thread.detach();

	return 0;
}


void Firewall::HandleListen(const std::string ForwardAddress, const std::string ForwardPort)
{
#ifdef __DEBUG
	printf("Server listening started\n");
#endif
	m_KeepAlive.lock();
	while (KeepAlive)
	{
		m_KeepAlive.unlock();

		SOCKET* ClientSocket = new SOCKET;
		struct sockaddr_in client_info;
		int size = sizeof(client_info);

		*ClientSocket = accept(ListenSocket, (sockaddr*)&client_info, &size);

		if (*ClientSocket != INVALID_SOCKET) {//	
			SOCKET* ServerSocket = new SOCKET;
			*ServerSocket = INVALID_SOCKET;

			int iResult;
			struct addrinfo *result = NULL, *ptr = NULL, hints;

			ZeroMemory(&hints, sizeof(hints));
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;

			iResult = getaddrinfo(ForwardAddress.c_str(), ForwardPort.c_str(), &hints, &result);
			if (iResult != 0) {
#ifdef __DEBUG
				printf("getaddrinfo failed with error: %d\n", iResult);
#endif
				delete ServerSocket;
				m_KeepAlive.lock();
				continue;
			}

			for(ptr=result; ptr != NULL; ptr=ptr->ai_next) {

				*ServerSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
				if (*ServerSocket == INVALID_SOCKET) {
#ifdef __DEBUG
					printf("socket failed with error: %ld\n", WSAGetLastError());
#endif
					break;
				}

				iResult = connect(*ServerSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
				if (iResult == SOCKET_ERROR) {
					closesocket(*ServerSocket);
					*ServerSocket = INVALID_SOCKET;
					continue;
				}
				break;
			}

			freeaddrinfo(result);

			if (*ServerSocket == INVALID_SOCKET) {
#ifdef __DEBUG
				printf("Unable to connect to server!\n");
#endif
				delete ServerSocket;
				m_KeepAlive.lock();
				continue;
			}
			
			std::thread ClientThread(&Firewall::HandleClient, this, ClientSocket, ServerSocket, std::string(inet_ntoa(client_info.sin_addr)));
			std::thread ServerThread(&Firewall::HandleServer, this, ServerSocket, ClientSocket);
			ClientThread.detach();
			ServerThread.detach();
		}
		else {
			delete ClientSocket;
		}

		m_KeepAlive.lock();
	}
	m_KeepAlive.unlock();

	return;
}


void Firewall::HandleClient(SOCKET *ClientSocket, SOCKET *ServerSocket, const std::string IP_Addr)
{
#ifdef __DEBUG
	static int ThreadCounter = 0;
	const int ThreadID = ThreadCounter++;
	printf("Client thread %i started\n", ThreadID);
#endif

	std::string Handshake;
	Handshake.reserve(512);
	bool Authorised = false;

	m_KeepAlive.lock();
	while (KeepAlive) {
		m_KeepAlive.unlock();

		std::string RecvBuffer;
		RecvBuffer.resize(8192);

		int iResult = recv(*ClientSocket, &RecvBuffer[0], RecvBuffer.length(), 0);

		if (iResult <= 0) {
#ifdef __DEBUG
			printf("Client thread %i ended\n", ThreadID);
#endif
			closesocket(*ClientSocket);
			closesocket(*ServerSocket);
			delete ClientSocket;
			return;
		}

		RecvBuffer.resize(iResult);

		if (Authorised == false) {
			Handshake.append(RecvBuffer);
			std::string username;
			int uResult = GetUsername(Handshake, username);
			if (uResult == 0) {
#ifdef __DEBUG
				printf("User:[%s] connected on thread:[%i] using IP:[%s]\n", username.c_str(), ThreadID, IP_Addr.c_str());
#endif
				std::string pollAddress;
				if (loginDatabase->PollUser(username, pollAddress) == 0) { // Check if user connected using correct ip
					if (pollAddress == IP_Addr) {
						Authorised = true;
						loginDatabase->RemoveUser(username);
					}
					else { // Player trying to connect with ip different to the one used for authentication
						std::string kickPacket = GenKickPacket("You must connect the IP you used to log in");
						iResult = send(*ClientSocket, &kickPacket[0], kickPacket.length(), 0);
						if (iResult == SOCKET_ERROR) {
#ifdef __DEBUG
							printf("send failed with error: %d\nClient thread %i ended\n", WSAGetLastError(),ThreadID);
#endif		
						}

						closesocket(*ClientSocket);
						closesocket(*ServerSocket);
						delete ClientSocket;
#ifdef __DEBUG
						printf("Client thread %i ended # Reason: ip address does not match\n", ThreadID);
#endif
						return;
					}
				}
				else { // Player did not authenticate using website
					std::string kickPacket = GenKickPacket(
						"You must login to\n" + cfg::GetHTTPdns() +
						"\nwith the username \"" + username +
						"\"\nbefore you are allowed to\nconnect to this server.");
					iResult = send(*ClientSocket, &kickPacket[0], kickPacket.length(), 0);
					if (iResult == SOCKET_ERROR) {
#ifdef __DEBUG
						printf("send failed with error: %d\nClient thread %i ended\n", WSAGetLastError(), ThreadID);
#endif		
					}

					closesocket(*ClientSocket);
					closesocket(*ServerSocket);
					delete ClientSocket;
#ifdef __DEBUG
					printf("Client thread %i ended # Reason: user not authenticated\n", ThreadID);
#endif
					return;
				}
			}
			else if (uResult == -1) { // If client is not trying to connect to the server for game play purposes
				Authorised = true;
			}
		}


		iResult = send(*ServerSocket, &RecvBuffer[0], RecvBuffer.length(), 0);
		if (iResult == SOCKET_ERROR) {
#ifdef __DEBUG
			printf("send failed with error: %d\nClient thread %i ended\n", WSAGetLastError(), ThreadID);
#endif			
			closesocket(*ClientSocket);
			closesocket(*ServerSocket);
			delete ClientSocket;
			return;
		}

		m_KeepAlive.lock();
	}
	m_KeepAlive.unlock();

#ifdef __DEBUG
	printf("Client thread %i ended\n", ThreadID);
#endif

	closesocket(*ClientSocket);
	closesocket(*ServerSocket);
	delete ClientSocket;

	return;
}


void Firewall::HandleServer(SOCKET *ServerSocket, SOCKET *ClientSocket)
{
#ifdef __DEBUG
	static int ThreadCounter = 0;
	const int ThreadID = ThreadCounter++;
	printf("Server thread %i started\n", ThreadID);
#endif

	m_KeepAlive.lock();
	while (KeepAlive) {
		m_KeepAlive.unlock();

		std::string RecvBuffer;
		RecvBuffer.resize(8192);

		int iResult = recv(*ServerSocket, &RecvBuffer[0], RecvBuffer.length(), 0);

		if (iResult <= 0) {
#ifdef __DEBUG
			printf("Server thread %i ended\n", ThreadID);
#endif
			closesocket(*ServerSocket);
			closesocket(*ClientSocket);
			delete ServerSocket;
			return;
		}

		RecvBuffer.resize(iResult);

		iResult = send(*ClientSocket, &RecvBuffer[0], RecvBuffer.length(), 0);
		if (iResult == SOCKET_ERROR) {
#ifdef __DEBUG
			printf("send failed with error: %d\nServer thread %i ended\n", WSAGetLastError(),ThreadID);
#endif			 
			closesocket(*ServerSocket);
			closesocket(*ClientSocket);
			delete ServerSocket;
			return;
		}

		m_KeepAlive.lock();
	}
	m_KeepAlive.unlock();

#ifdef __DEBUG
	printf("Server thread %i ended\n", ThreadID);
#endif

	closesocket(*ServerSocket);
	closesocket(*ClientSocket);
	delete ServerSocket;
	return;
}


int Firewall::GetUsername(const std::string data, std::string & username)
{
	if (data.at(0) != 0x02) {
		return -1; // not correct packet.
	}

	if (data.length() < 10) {
		return 1; // Not enough data.
	}

	// Get username
	unsigned int StringLen = ((unsigned char)data.at(2) << 8) + ((unsigned char)data.at(3));

	if (StringLen*2 + 4 > data.length()) {
		return 1; // Not enought data.
	}

	username.clear();
	username.reserve(StringLen);

	for (unsigned int i = 4; i < StringLen*2 + 4; i+=2)
	{
		username.push_back(((unsigned char)data.at(i) << 8) + (unsigned char)data.at(i+1));
	}

	return 0; // Username found.
}


std::string Firewall::GenKickPacket(const std::string message)
{
	std::string kickPacket;

	// Type
	kickPacket.push_back((char)0xFF);

	// Length
	kickPacket.push_back(message.length() >> 8);
	kickPacket.push_back(message.length() & 0xFF);

	// Payload
	for (unsigned int i = 0; i < message.length(); i++)
	{
		kickPacket.push_back(message.at(i) >> 8);
		kickPacket.push_back(message.at(i) & 0xFF);
	}

	return kickPacket;
}