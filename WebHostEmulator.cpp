#include "stdafx.h"
#include "WebHostEmulator.h"

// #define __DEBUG // Comment this out to disable debug text in console

WebHostEmulator::WebHostEmulator(void) : Root("http/")
{
	accountManager.SetDatabase("database/accounts.gdb");
}


WebHostEmulator::~WebHostEmulator(void)
{
}

// @brief: starts webserver
// @param[in]: port - port that the server listens on
int WebHostEmulator::start(std::string port, LoginDatabase *database)
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

	// Resolve the server address and por
	iResult = getaddrinfo(NULL, port.c_str(), &hints, &result);
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

	std::thread thread(&WebHostEmulator::HandleListen, this);
	thread.detach();

	return 0;
}


// @brief: handles listening of webserver
void WebHostEmulator::HandleListen()
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

		if (*ClientSocket != INVALID_SOCKET) {
			std::thread thread(&WebHostEmulator::HandleClient, this, ClientSocket, std::string(inet_ntoa(client_info.sin_addr)));
			thread.detach();
		}
		else {
			delete ClientSocket;
		}

		m_KeepAlive.lock();
	}
	m_KeepAlive.unlock();

	return;
}


// @brief: handles webclient
// @param[in]: socket - socket that client is connected on
// @param[in]: ip_addr - ip address of the client
void WebHostEmulator::HandleClient(SOCKET* socket, const std::string ip_addr)
{
#ifdef __DEBUG
	static int ThreadCounter = 0;
	const int ThreadID = ThreadCounter++;
	printf("Thread %i started\n", ThreadID);
#endif

	std::queue <std::string> RecvQueue;
	std::string Received, overflow;

	while (1) {
		// Recieve
		std::string RecvBuffer;
		RecvBuffer.resize(8192);
	
		int iResult = recv(*socket, &RecvBuffer[0], RecvBuffer.length(), 0);

		if (iResult <= 0) {
#ifdef __DEBUG
			printf("Thread %i ended\n", ThreadID);
#endif
			closesocket(*socket);
			delete socket;
			return;
		}

		RecvBuffer.resize(iResult);

		if (parseHTTP (RecvBuffer, Received, overflow) != 0) {
			continue;
		}
		
#ifdef __DEBUG
		printf("Received Message:-----\n%s\nEnd Received-----\n\n", Received.c_str());
#endif

		// Send
		size_t found;
		std::string Body, ContentLength, ContentType, Header, Response;

		found = Received.find("GET");
		if (found == 0) {
			size_t start = Received.find("/");
			size_t end = Received.find(" ", start);

			ContentType = GetReq(std::string(Received, start, end - start), Body);
			Header = BuildHeader(Body, 200, ContentType, true);

			Response = Header + Body;

			iResult = send(*socket, Response.c_str(), Response.size(), 0);
		}

		found = Received.find("POST");
		if (found == 0) {

			size_t start = Received.find("/");
			size_t end = Received.find(" ", start);

			std::string Result = PostReq(
				std::string(Received, start, end - start),
				GetBody(Received),
				ip_addr
				);

			ContentType = "text/html"; // This is a temp fix.

			Body = BuildResult(Result, std::string("result.html"));
			Header = BuildHeader(Body, 200, ContentType, false);

			Response = Header + Body;

			iResult = send(*socket, Response.c_str(), Response.size(), 0);
		}

		Received.clear();
	}

#ifdef __DEBUG
	printf("Thread %i ended\n", ThreadID);
#endif

	closesocket(*socket);

	delete socket;
	return;
}


// @brief: handles get request
// @param[in]: page - name of file requested
// @param[out]: content - contents of requested file
// @return: content type of file
std::string WebHostEmulator::GetReq(const std::string &page, std::string &content)
{
	std::string root = "http/";
	std::fstream file;

	GetPage(page, content);

	if (page == std::string("/")) {
		return std::string("text/html");
	}

	size_t found = page.find_last_of(".");
	
	if (found == std::string::npos) {
		return std::string();
	}

	std::string extension(page, found, page.length() - found);


	if (extension == std::string(".html")) {
		return std::string("text/html");
	}

	else if (extension == std::string(".css")) {
		return std::string("text/css");
	}

	else if (extension == std::string(".js")) {
		return std::string("text/javascript");
	}

	else if (extension == std::string(".ico")) {
		return std::string("image/x-icon");
	}

	else if (extension == std::string(".virtual_json")) {
		std::string buffer = jsonGet(page, content);
		return buffer;
	}

	return std::string();
}


std::string WebHostEmulator::ParseField(const std::string &data, const std::string &field)
{
	size_t start = data.find(field + "=");
	size_t end = data.find("&", start + 1);
	
	if (start == std::string::npos) {
		return std::string();
	}

	if (end == std::string::npos) {
		end = data.length();
	}

	return std::string(data, start + field.length() + std::string("=").length(), end - start - field.length() - std::string("=").length());
}


// @brief: handles post request
// @param[in]: request - type of request: /login,/register,/forgot,/change_password,/delete
// @param[in]: parameters - parameters that comes with the request
std::string WebHostEmulator::PostReq(const std::string &request, const std::string &parameters, const std::string ip_addr)
{
	if (request == std::string("/login"))
		return postLogin(parameters, ip_addr);

	else if (request == std::string("/register"))
		return postRegister(parameters);

	else if (request == std::string("/forgot"))
		return postForgot(parameters);

	else if (request == std::string("/change_password")) 
		return postChangePassword(parameters);

	else if (request == std::string("/delete")) 
		return postDelete(parameters);

	else 
		return std::string("<p>Error: Unknown request</p>");

	return std::string();
}


// @brief: copies contents of a file on harddrive into string
// @param[in]: page - name of file requested
// @param[out]: content - contents of requested file
int WebHostEmulator::GetPage(const std::string &page, std::string &content)
{
	std::fstream file;

	if (page == "/") {
		file.open(Root + "index.html", std::ios::in | std::ios::binary);
	}
	else {
		file.open(Root + page, std::ios::in | std::ios::binary);
	}

	if (!file.good()) {
		file.close();
		file.open(Root + "404.html", std::ios::in | std::ios::binary);
	}

	if(!file.good()) {
		file.close();
		content = "<!DOCTYPE html><html><head><title>404</title></head><body><h1>Error: 404</h1><br /><p>The page that was requested was not found.</p></body></html>";
		return 1;
	}

	file.seekg(0, std::ios::end);
	int length = (int)file.tellg();
	file.seekg(0, std::ios::beg);

	content.resize(length);

	file.read (&content[0], length);
	file.close();

	return 0;
}


// @brief: builds custom header for given page
// @param[in]: content - page that the header is being built for
// @param[in]: StatusCode - status to be placed into header
// @param[in]: ContentType - ContentType to be placed into header
// @param[in]: AllowCache - adds cache to header
// @return: custom built header.
std::string WebHostEmulator::BuildHeader (const std::string &content, const int &StatusCode, const std::string &ContentType, const bool &AllowCache)
{
	std::stringstream ss;
	std::string Header, status_code, content_length;

	ss << StatusCode;
	ss >> status_code;
	ss.clear();

	Header =
		"HTTP/1.1 " + status_code + " OK\r\n"
		"Content-Type:" + ContentType + "\r\n";

	if (AllowCache == false)
		Header.append("Cache-Control: no-cache\r\n");

	ss << content.length();
	ss >> content_length;
	
	Header.append(
		"Content-Length:" + content_length + "\r\n"
		"\r\n");
	
	return Header;
}


// @brief: extracts and returns body from message
// @param[in]: content - message to extract the body from
// @return: extracted body
std::string WebHostEmulator::GetBody(const std::string &message)
{
	size_t pos = message.find("\r\n\r\n");

	if (pos == std::string::npos) {
		return std::string();
	}

	std::string body = std::string (message, pos + std::string("\r\n\r\n").length(), message.length() - pos);

	return body;
}


// @brief: inserts message into a file on harddrive
// @param[in]: message - message that will be placed inside the page
// @param[in]: page - file address of the page on harddrive
// @return: custom result page
std::string WebHostEmulator::BuildResult (const std::string &message, const std::string &page)
{
	std::fstream file;
	file.open(Root + page, std::ios::in | std::ios::binary);
	
	if (!file.good()) {
		file.close();
		return std::string("<!DOCTYPE html><html><head><title></title></head><body>") + message + std::string("</body></html>");
	}

	std::string content;

	file.seekg(0, std::ios::end);
	int length = (int)file.tellg();
	file.seekg(0, std::ios::beg);

	content.resize(length);

	file.read (&content[0], length);
	file.close();

	size_t found = content.find("id=\"resultinfo\"");
	if (found == std::string::npos) {
		return std::string("<!DOCTYPE html><html><head><title></title></head><body>") + message + std::string("</body></html>");
	}

	found = content.find(">", found);

	std::string pre(content, 0, found + 1);
	std::string post(content, found + 1, content.length() - found+1);

	return std::string(pre + message + post);
}


// @brief: parses defragments fragmented HTTP messages
// @param[in]: recvbuf- fragmented http message
// @param[out]: message - defragmented http message
// @param[none]: overflow - function personal storage
// @return: 0: message ready to be read
// @note: when function returns 0, move the message somewhere else, then clear it.
//			NEVER TOUCH overflow or this will break...
int WebHostEmulator::parseHTTP (const std::string recvbuf, std::string &message, std::string &overflow)
{
	message.append(overflow + recvbuf);
	overflow.clear();

	size_t delimiter = message.find("\r\n\r\n");
	
	if (delimiter != std::string::npos) {

		size_t cont_beg = message.find("Content-Length: ");

		if (cont_beg != std::string::npos)
		{
			size_t cont_end = message.find("\r\n", cont_beg);
			
			std::stringstream ss;
			ss << std::string (message,	cont_beg + std::string("Content-Length: ").length(),
				cont_end - cont_beg - std::string("Content-Length: ").length());

			int rawData;
			ss >> rawData;
			rawData;

			if (message.length() < delimiter + rawData) {
				return 2; // Reading data in raw mode.
			}
			else {
				if (message.length() == delimiter + std::string("\r\n\r\n").length() + rawData - 1) {
					overflow.clear();
				}
				else {
					overflow = message.substr(delimiter + std::string("\r\n\r\n").length() + rawData);
				}
				message.resize(delimiter + std::string("\r\n\r\n").length() + rawData);
				
				return 0; // Message complete.
			}
		}
		else {
			overflow = message.substr(delimiter + std::string("\r\n\r\n").length());
			message.resize(delimiter + std::string("\r\n\r\n").length());
			return 0; // Message complete.
		}
	}

	return 1; // Reading data in delimiter mode.
}
