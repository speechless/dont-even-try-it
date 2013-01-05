#include "stdafx.h"
#include "AccountManager.h"
#include "deti.pb.h"


AccountManager::AccountManager(void)
{
}


AccountManager::~AccountManager(void)
{
}


int AccountManager::SetDatabase(const std::string location)
{
	std::lock_guard<std::mutex> m(m_file);
	_database = location;
	return 0;
}


// Return: negative for system error, 0 for success, 1+ for change error
int AccountManager::Change(std::string change, std::string username, const std::string password, std::string email)
{
	if ( (username.empty() && password.empty()) || (username.empty() && email.empty()) ) {
		return 1; // Not enough parameters.
	}

	std::transform(change.begin(), change.end(), change.begin(), ::tolower);

	if (change != std::string("username") && change != std::string("password") && change != std::string("email")) {
		return 2; // Invalid change request.
	}

	std::fstream file;
	DETI::Database database;
	std::lock_guard<std::mutex> m(m_file);

	file.open(_database, std::ios::in | std::ios::binary);
	if(!file.good()) {
		file.close();
		return -1; // Cannot open database file.
	}

	if (!database.ParseFromIstream(&file)) {
		file.close();
		return -2; // Cannot parse database data.
	}

	file.close();

	int entry = 0;
	std::string _email, _username;

	if (change == std::string("username")) {
		std::transform(email.begin(), email.end(), email.begin(), ::tolower);
		std::transform(username.begin(), username.end(), username.begin(), ::tolower);

		while (entry < database.account_size()) {
			if (database.account(entry).has_email()) {

				_email = database.account(entry).email();
				std::transform(_email.begin(), _email.end(), _email.begin(), ::tolower);

				if (email == _email) {
					break;
				}
				else {
					entry++;
				}
			}
			else {
				entry++;
				continue;
			}
		}

		std::cout << "Entry:" << entry << std::endl;

		if (entry >= database.account_size()) {
			return 3; // Account not found.
		}

		DETI::Account *account = database.mutable_account(entry);
		account->set_username(username);
	}
	else if (change == std::string("password")) {
		std::transform(username.begin(), username.end(), username.begin(), ::tolower);

		while (entry < database.account_size()) {
			if (database.account(entry).has_username()) {

				_username = database.account(entry).username();
				std::transform(_username.begin(), _username.end(), _username.begin(), ::tolower);

				if (username == _username) {
					break;
				}
				else {
					entry++;
				}
			}
			else {
				entry++;
				continue;
			}
		}

		std::cout << "Entry:" << entry << std::endl;

		if (entry >= database.account_size()) {
			return 3; // Account not found.
		}

		DETI::Account *account = database.mutable_account(entry);
		account->set_password(password);
	}
	else if (change == std::string("email")) {
		std::transform(email.begin(), email.end(), email.begin(), ::tolower);
		std::transform(username.begin(), username.end(), username.begin(), ::tolower);

		while (entry < database.account_size()) {
			if (database.account(entry).has_username()) {

				_username = database.account(entry).username();
				std::transform(_username.begin(), _username.end(), _username.begin(), ::tolower);

				if (username == _username) {
					break;
				}
				else {
					entry++;
				}
			}
			else {
				entry++;
				continue;
			}
		}

		std::cout << "Entry:" << entry << std::endl;

		if (entry >= database.account_size()) {
			return 3; // Account not found.
		}

		DETI::Account *account = database.mutable_account(entry);
		account->set_email(email);
	}

	file.open(_database + ".bck", std::ios::out | std::ios::binary | std::ios::trunc);

	if (!file.good()) {
		file.close();
		return -1; // Cannot open database file.
	}

	database.SerializeToOstream(&file);

	file.close();

	file.open(_database, std::ios::out | std::ios::binary | std::ios::trunc);

	if (!file.good()) {
		file.close();
		return -1; // Cannot open database file.
	}

	database.SerializeToOstream(&file);

	file.close();

	return 0; // Register successful.
}


// Return: negative for system error, 0 for success, 1+ for delete error
int AccountManager::Delete(std::string username) {
	std::fstream file;
	std::string _username;
	DETI::Database database;

	std::transform(username.begin(), username.end(), username.begin(), ::tolower);
	std::lock_guard<std::mutex> m(m_file);

	file.open(_database, std::ios::in | std::ios::binary);
	if(!file.good()) {
		file.close();
		return -1; // Cannot open database file.
	}

	if (!database.ParseFromIstream(&file)) {
		file.close();
		return -2; // Cannot parse database data.
	}

	file.close();

	int entry = 0;
	
	while (entry < database.account_size()) {
		if (database.account(entry).has_username()) {

			_username = database.account(entry).username();
			std::transform(_username.begin(), _username.end(), _username.begin(), ::tolower);

			if (username == _username) {
				break;
			}
			else {
				entry++;
			}
		}
		else {
			entry++;
			continue;
		}	
	}

	if (entry >= database.account_size()) {
		return 1; // User not found.
	}

	database.mutable_account()->SwapElements(entry, database.account_size() - 1);
	database.mutable_account()->RemoveLast();

	file.open(_database + ".bck", std::ios::out | std::ios::binary | std::ios::trunc);
	if(!file.good()) {
		file.close();
		return -1; // Cannot open database file.
	}

	database.SerializeToOstream(&file);

	file.close();

	file.open(_database, std::ios::out | std::ios::binary | std::ios::trunc);
	if(!file.good()) {
		file.close();
		return -1; // Cannot open database file.
	}

	database.SerializeToOstream(&file);

	file.close();

	return 0; // Login successful.
}


// Return: negative for system error, 0 for success, 1+ for get error
int AccountManager::GetUserPass(std::string email, std::string &username, std::string &password)
{
	std::fstream file;
	std::string _email;
	DETI::Database database;

	std::transform(email.begin(), email.end(), email.begin(), ::tolower);
	std::lock_guard<std::mutex> m(m_file);

	file.open(_database, std::ios::in | std::ios::binary);
	if(!file.good()) {
		file.close();
		return -1; // Cannot open database file.
	}

	if (!database.ParseFromIstream(&file)) {
		file.close();
		return -2; // Cannot parse database data.
	}

	file.close();

	for (int i = 0; i < database.account_size(); i++)
	{
		if (database.account(i).has_email() && database.account(i).has_password() && database.account(i).has_username())
		{
			_email = database.account(i).email();
			std::transform(_email.begin(), _email.end(), _email.begin(), ::tolower);

			if (email == _email) {
				password = database.account(i).password();
				username = database.account(i).username();
				return 0; // Found username and password.
			}
		}
	}

	return 1; // Did not find relevant match.
}

// Return: negative for system error, 0 for success, 1+ for login error
int AccountManager::Login(std::string username, const std::string password)
{
	std::fstream file;
	std::string _username;
	DETI::Database database;

	std::transform(username.begin(), username.end(), username.begin(), ::tolower);
	std::lock_guard<std::mutex> m(m_file);

	file.open(_database, std::ios::in | std::ios::binary);
	if(!file.good()) {
		file.close();
		return -1; // Cannot open database file.
	}

	if (!database.ParseFromIstream(&file)) {
		file.close();
		return -2; // Cannot parse database data.
	}

	file.close();

	for (int i = 0; i < database.account_size(); i++) {
		if (database.account(i).has_username() && database.account(i).has_password()) {

			_username = database.account(i).username();
			std::transform(_username.begin(), _username.end(), _username.begin(), ::tolower);

			if (username == _username) {
				 if(database.account(i).password() == password) {
					 return 0; // Password Accepted.
				 }
				 else {
					 return 1; // Incorrect Password.
				 }
			}
		}	
	}

	return 2; // User not found.
}

// Return: negative for system error, 0 for success, 1+ for register error
int AccountManager::Register(std::string username, const std::string password, std::string email)
{
	std::fstream file;
	std::string _email, _username;
	DETI::Database database;
	DETI::Account *account;

	std::transform(email.begin(), email.end(), email.begin(), ::tolower);
	std::transform(username.begin(), username.end(), username.begin(), ::tolower);

	std::lock_guard<std::mutex> m(m_file);

	file.open(_database, std::ios::in | std::ios::binary);
	if (!file.good()) {
		file.close();
		return -1; // Cannot open database file.
	}

	if (!database.ParseFromIstream(&file)) {
		file.close();
		return -2; // Cannot parse database data.
	}

	file.close();

	for (int i = 0; i < database.account_size(); i++) {
		if (database.account(i).has_email() && database.account(i).has_username()) {
			_email = database.account(i).email();
			std::transform(_email.begin(), _email.end(), _email.begin(), ::tolower);

			_username = database.account(i).username();
			std::transform(_username.begin(), _username.end(), _username.begin(), ::tolower);

			if (username == _username) {
				return 1; // Username already in use.
			}
			else if (email == _email) {
				return 2; // Email address already in use.
			}
		}
	}

	account = database.add_account();

	account->set_activated(false);
	account->set_email(email);
	account->set_password(password);
	account->set_username(username);

	file.open(_database + ".bck", std::ios::out | std::ios::binary | std::ios::trunc);

	if (!file.good()) {
		file.close();
		return -1; // Cannot open database file.
	}

	database.SerializeToOstream(&file);

	file.close();

	file.open(_database, std::ios::out | std::ios::binary | std::ios::trunc);

	if (!file.good()) {
		file.close();
		return -1; // Cannot open database file.
	}

	database.SerializeToOstream(&file);

	file.close();

	return 0; // Register successful.
}