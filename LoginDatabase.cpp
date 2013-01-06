#include "stdafx.h"
#include "LoginDatabase.h"


LoginDatabase::LoginDatabase(void)
{
}


LoginDatabase::~LoginDatabase(void)
{
}


int LoginDatabase::init(const unsigned int Timeout)
{
	std::lock_guard<std::mutex> d(m_database);
	std::lock_guard<std::mutex> k(m_KeepAlive);
	timeout = Timeout;
	KeepAlive = true;

	std::thread t(&LoginDatabase::HandleTimeout, this);
	t.detach();

	return 0;
}


int LoginDatabase::deinit()
{
	std::lock_guard<std::mutex> k(m_KeepAlive);
	KeepAlive = false;

	return 0;
}


int LoginDatabase::AddUser(std::string username, const std::string ip_address)
{
	std::transform(username.begin(), username.end(), username.begin(), ::tolower); 
	std::lock_guard<std::mutex> m(m_database);

	for (std::list <session>::iterator i = database.begin(); i != database.end(); i++)
	{
		if (i->username == username)
		{
			i->ip_address = ip_address;
			i->timeout = timeout;		
			return 0;
		}
	}

	session NewSession;

	NewSession.ip_address = ip_address;
	NewSession.timeout = timeout;
	NewSession.username = username;

	database.push_back(NewSession);

#ifdef __DEBUG
	printf("Added:%s\n", username.c_str());
#endif

	return 0;
}


int LoginDatabase::PollUser(std::string username, std::string &ip_address)
{
	std::transform(username.begin(), username.end(), username.begin(), ::tolower); 
	std::lock_guard<std::mutex> m(m_database);

	for (std::list <session>::iterator i = database.begin(); i != database.end(); i++)
	{
		if (i->username == username)
		{
#ifdef __DEBUG
			printf("Polled:%s\n", i->username.c_str());
#endif
			ip_address = i->ip_address;	
			return 0;
		}
	}

	return 1;
}


int LoginDatabase::RemoveUser(std::string username)
{
	std::transform(username.begin(), username.end(), username.begin(), ::tolower); 
	std::lock_guard<std::mutex> m(m_database);

	for (std::list <session>::iterator i = database.begin(); i != database.end(); i++)
	{
		if (i->username == username)
		{
#ifdef __DEBUG
			printf("Removed by request:%s\n", i->username.c_str());
#endif
			database.erase(i);
			return 0;
		}
	}

	return 0;
}


void LoginDatabase::HandleTimeout()
{
	m_KeepAlive.lock();

	while (KeepAlive)
	{
		m_KeepAlive.unlock();

		m_database.lock();

		std::list <session>::iterator i = database.begin();
		
		while (i != database.end())
		{
			if (i->timeout == 0)
			{
#ifdef __DEBUG
				printf("Removing:%s\n", i->username.c_str());
#endif
				std::list <session>::iterator wipe = i;
				i++;
				database.erase(wipe);
			}
			else
			{
				i->timeout--;
#ifdef __DEBUG
				printf("%s:%i\n", i->username.c_str(), i->timeout);
#endif
				i++;
			}
		}

		m_database.unlock();

		std::this_thread::sleep_for(std::chrono::seconds(1));

		m_KeepAlive.lock();
	}

	m_KeepAlive.unlock();

	return;
}
