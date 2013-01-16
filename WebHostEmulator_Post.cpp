#include "stdafx.h"
#include "Config.h"
#include "WebHostEmulator.h"

// #define __DEBUG // Comment this out to disable debug text in console

std::string WebHostEmulator::postLogin(const std::string &parameters, const std::string ip_addr)
{
	std::string password, username;

	username = ParseField(parameters, "username");
	password = ParseField(parameters, "password");

#ifdef __DEBUG
		printf("Login-> Username:[%s] Password:[%s]\n", username.c_str(), password.c_str());
#endif

	int Result = accountManager.Login(username, password);

	if (Result == 0) {
		loginDatabase->AddUser(username, ip_addr);
		return std::string("<p>Login successful<br /><br />" + cfg::GetLoginMessage() + "</p>");
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


std::string WebHostEmulator::postRegister(const std::string &parameters)
{
	std::string email, password1, password2, username;

	username = ParseField(parameters, "username");
	email = ParseField(parameters, "email");
	password1 = ParseField(parameters, "password1");
	password2 = ParseField(parameters, "password2");

#ifdef __DEBUG
	printf("Register-> Username:[%s] Email:[%s] Password1:[%s] Password2:[%s]\n",
		username.c_str(), email.c_str(), password1.c_str(), password2.c_str());
#endif

	if (username.empty() || email.empty() || password1.empty() || password2.empty()) {
		std::string ErrorReply = "<p>Resgister Failed<br />";
			
		if (username.empty()) ErrorReply.append("username ");
		if (username.empty()) ErrorReply.append("email ");
		if (username.empty()) ErrorReply.append("pwd1 ");
		if (username.empty()) ErrorReply.append("pw2 ");

		ErrorReply.append("is missing");

		return ErrorReply;
	}

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


std::string WebHostEmulator::postForgot(const std::string &parameters)
{
	return std::string("<p>Forgot request not supported by server</p>");
}


std::string WebHostEmulator::postChangePassword(const std::string &parameters)
{
	std::string oldpassword, password1, password2, username;

	username = ParseField(parameters, "username");
	oldpassword = ParseField(parameters, "oldpassword");
	password1 = ParseField(parameters, "password1");
	password2 = ParseField(parameters, "password2");

	if (username.empty() || oldpassword.empty() || password1.empty() || password2.empty()) {
		return std::string("<p>Error<br />All fields must be completed</p>");
	}

	if (password1 != password2) {
		return std::string("<p>Error<br />New passwords do not match</p>");
	}
	
	if (accountManager.Login(username, oldpassword) == 0) {
		accountManager.Change("password", username, password1, "");
		return std::string("<p>Password has been changed</p>");
	}
		
	return std::string("<p>An error has occured</p>");
}


std::string WebHostEmulator::postDelete(const std::string &parameters)
{
	std::string username1, username2, password;
	username1 = ParseField(parameters, "username1");
	username2 = ParseField(parameters, "username2");
	password = ParseField(parameters, "password");

	if (username1.empty() || username2.empty() || password.empty()) {
		return std::string("<p>Error<br />All fields must be completed</p>");
	}

	std::string password_buffer = cfg::GetAdminPassword();
	printf("Comparing passwords:%s:%s:\ncompared together: %i", password_buffer.c_str(), password.c_str(),password.compare(password_buffer));
		
	if (password.compare(password_buffer)) {
		return std::string("<p>Error<br />Incorrect password</p>");
	}

	if (username1 != username2) {
		return std::string("<p>Error<br />Username do not match</p>");
	}

	int Result = accountManager.Delete(username1);
		
	if (Result == 0) {
		return std::string("<p>" + username1 + " has been successfully deleted");
	}
	else if (Result == 1) {
		return std::string("<p>" + username1 + " not found in database.");
	}
	else if (Result == -1) {
		return std::string("<p>Error: cannot open database.");
	}
	else if (Result == -2) {
		return std::string("<p>Error: database is corrupt.");
	}

	return std::string("<p>Error: Unknown error</p>");
}
