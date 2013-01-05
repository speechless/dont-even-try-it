#include "stdafx.h"
#include "WebHostEmulator.h"


WebHostEmulator::WebHostEmulator(void) : Root("http/")
{
	accountManager.SetDatabase("database/accounts.gdb");
}


WebHostEmulator::~WebHostEmulator(void)
{
}

// @brief: starts webserver
// @param[in]: port - port that the server listens on
int WebHostEmulator::start(const std::string &port)
{
	WSADATA wsaData;
	int iResult;

	ListenSocket = INVALID_SOCKET;
	
	m_KeepAlive.lock();
	KeepAlive = true;
	m_KeepAlive.unlock();

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and por
	iResult = getaddrinfo(NULL, port.c_str(), &hints, &result);
	if ( iResult != 0 ) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
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
	// Recieve
	std::string Received;
	Received.resize(8192);
	
	int iResult = recv(*socket, &Received[0], Received.length(), 0);

	Received.resize(iResult);

	std::cout << Received << std::endl;

	// Send
	size_t found;
	std::string Body, ContentLength, ContentType, Header, Response;

	found = Received.find("GET");
	if (found == 0) {
		size_t start = Received.find("/");
		size_t end = Received.find(" ", start);

		ContentType = GetReq(std::string(Received, start, end - start), Body);
		Header = BuildHeader(Body, 200, ContentType, false);

		Response = Header + Body;

		iResult = send(*socket, Response.c_str(), Response.size(), 0);
	}

	found = Received.find("POST");
	if (found == 0) {
		size_t start = Received.find("/");
		size_t end = Received.find(" ", start);

		std::string Result = PostReq(std::string(Received, start, end - start), GetBody(Received));

		Body = BuildResult(Result, std::string("result.html"));
		Header = BuildHeader(Body, 200, ContentType, false);

		Response = Header + Body;

		iResult = send(*socket, Response.c_str(), Response.size(), 0);
	}

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
		std::cout << "text/html" << std::endl;
		return std::string("text/html");
	}

	size_t found = page.find_last_of(".");
	
	if (found == std::string::npos) {
		return std::string();
	}

	std::string extension(page, found, page.length() - found);

	std::cout << "detected ext:" << extension << std::endl;

	if (extension == std::string(".html")) {
		std::cout << "text/html" << std::endl;
		return std::string("text/html");
	}

	else if (extension == std::string(".css")) {
		std::cout << "text/css" << std::endl;
		return std::string("text/css");
	}

	else if (extension == std::string(".js")) {
		std::cout << "text/javascript" << std::endl;
		return std::string("text/javascript");
	}

	return std::string();
}


std::string ParseField(const std::string data, const std::string field)
{
	size_t start = data.find(field + "=");
	size_t end = data.find("&", start);
	
	if (start == std::string::npos) {
		return std::string();
	}

	if (end == std::string::npos) {
		end = data.length();
	}

	return std::string(data, start + field.length() + std::string("=").length(), end - start - field.length() - std::string("=").length());
}


// @brief: handles post request
// @param[in]: request - type of request: /login,/register,/forgot
// @param[in]: parameters - parameters that comes with the request
std::string WebHostEmulator::PostReq(const std::string &request, const std::string &parameters)
{
	std::cout << "Request:" << request << ";" << std::endl;

	// Handle login request.
	if (request == std::string("/login")) {
		std::string password, username;

		username = ParseField(parameters, "username");
		password = ParseField(parameters, "password");

		std::cout << "Username:[" << username << "]"
			<< " Password:[" << password << "]"
			<< std::endl;

		if (username.empty() && password.empty()) {
			return std::string("<p>Login Failed<br />Username and password missing</p>");
		}

		if (username.empty()) {
			return std::string("<p>Login Failed<br />Username missing</p>");
		}

		if (password.empty()) {
			return std::string("<p>Login Failed<br />Password missing</p>");
		}

		// Handle login.

		int Result = accountManager.Login(username, password);
		
		if (Result == 0) {
			return std::string("<p>Login successful</p>");
		}

		if (Result == 1) {
			return std::string("<p>Login failed<br />Incorrect password</p>");
		}

		if (Result == 2) {
			return std::string("<p>Login failed<br />Account does not exist</p>");
		}

		if (Result == -1) {
			return std::string("<p>Server error: 1</p>");
		}

		if (Result == -2) {
			return std::string("<p>Server error: 2</p>");
		}

		return std::string("<p>Login failed<br />Unspecified error</p>");
	}

	// Handle register request.
	else if (request == std::string("/register")) {
		std::string email, password1, password2, username;

		username = ParseField(parameters, "username");
		email = ParseField(parameters, "email");
		password1 = ParseField(parameters, "password1");
		password2 = ParseField(parameters, "password2");

		std::cout << "Username:[" << username << "]"
			<< " Email:[" << email << "]"
			<< " Password1:[" << password1 << "]"
			<< " Password2:[" << password2 << "]"
			<< std::endl;

		if (username.empty() || email.empty() || password1.empty() || password2.empty()) {
			std::string buffer = "<p>Resgister Failed<br />";
			
			if (username.empty()) buffer.append("username ");
			if (username.empty()) buffer.append("email ");
			if (username.empty()) buffer.append("pwd1 ");
			if (username.empty()) buffer.append("pw2 ");

			buffer.append("is missing");

			return buffer;
		}


		// Handle register

		if (password1 != password2) {
			return std::string("<p>Error<br />Passwords do not match</p>");
		}

		int Result = accountManager.Register(username, password1, email);

		if (Result == 0) {
			return std::string("<p>Register successful<br />You can now login to the server</p>");
		}

		if (Result == 1) {
			return std::string("<p>Register failed<br />Username has already been registered</p>");
		}

		if (Result == 2) {
			return std::string("<p>Register failed<br />Email address has already been registered</p>");
		}

		if (Result == -1) {
			return std::string("<p>Server error: 1</p>");
		}

		if (Result == -2) {
			return std::string("<p>Server error: 2</p>");
		}

		return std::string("<p>Register failed<br />Unspecified error</p>");
		
	}

	// Handle forgot request.
	else if (request == std::string("/forgot")) {
		return std::string("<p>Forgot request not supported by server</p>");
	}


	else {
		return std::string("<p>Error: Unknown request</p>");
	}

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
std::string WebHostEmulator::BuildHeader (const std::string &content, const int StatusCode, const std::string ContentType, const bool AllowCache)
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

	return std::string (message, pos + std::string("\r\n\r\n").length(), message.length() - pos);
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

	size_t found = content.find("id=\"result\"");
	if (found == std::string::npos) {
		return std::string("<!DOCTYPE html><html><head><title></title></head><body>") + message + std::string("</body></html>");
	}

	found = content.find(">", found);

	std::string pre(content, 0, found + 1);
	std::string post(content, found + 1, content.length() - found+1);

	return std::string(pre + message + post);
}