#include "stdafx.h"
#include "Config.h"
#include "WebHostEmulator.h"

#define RESULT_PREFIX "<h2>"
#define RESULT_SUFFIX "</h2>"
#define MESSAGE_PREFIX "<p class=\"server_response\">"
#define MESSAGE_SUFFIX "</p>"

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
		return std::string(RESULT_PREFIX "Login successful" RESULT_SUFFIX MESSAGE_PREFIX + cfg::GetLoginMessage() + MESSAGE_SUFFIX);
	}

	if (Result == 1) {
		return std::string(RESULT_PREFIX "Login failed" RESULT_SUFFIX MESSAGE_PREFIX "Incorrect password" MESSAGE_SUFFIX);
	}

	if (Result == 2) {
		return std::string(RESULT_PREFIX "Login failed" RESULT_SUFFIX MESSAGE_PREFIX "Account does not exist" MESSAGE_SUFFIX);
	}

	if (Result == -1) {
		return std::string(RESULT_PREFIX "Server error: 1" RESULT_SUFFIX);
	}

	if (Result == -2) {
		return std::string(RESULT_PREFIX "Server error: 2" RESULT_SUFFIX);
	}

	return std::string(RESULT_PREFIX "Login failed" RESULT_SUFFIX MESSAGE_PREFIX "Unspecified error" MESSAGE_SUFFIX);
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
		std::string ErrorReply = RESULT_PREFIX "Resgister Failed" RESULT_SUFFIX MESSAGE_PREFIX;
			
		if (username.empty()) ErrorReply.append("username ");
		if (username.empty()) ErrorReply.append("email ");
		if (username.empty()) ErrorReply.append("pwd1 ");
		if (username.empty()) ErrorReply.append("pw2 ");

		ErrorReply.append("is missing" MESSAGE_SUFFIX);

		return ErrorReply;
	}

	if (password1 != password2) {
		return std::string(RESULT_PREFIX "Registration failed" RESULT_SUFFIX MESSAGE_PREFIX "Passwords do not match" MESSAGE_SUFFIX);
	}

	int Result = accountManager.Register(username, password1, email);

	if (Result == 0) {
		return std::string(RESULT_PREFIX "Registration successful" RESULT_SUFFIX MESSAGE_PREFIX "You can now login to the server" MESSAGE_SUFFIX);
	}

	if (Result == 1) {
		return std::string(RESULT_PREFIX "Registration failed" RESULT_SUFFIX MESSAGE_PREFIX "Username has already been registered" MESSAGE_SUFFIX);
	}

	if (Result == 2) {
		return std::string(RESULT_PREFIX "Registration failed" RESULT_SUFFIX MESSAGE_PREFIX "Email address has already been registered" MESSAGE_SUFFIX);
	}

	if (Result == -1) {
		return std::string(RESULT_PREFIX "Server error: 1" RESULT_SUFFIX);
	}

	if (Result == -2) {
		return std::string(RESULT_PREFIX "Server error: 2" RESULT_SUFFIX);
	}

	return std::string(RESULT_PREFIX "Registration failed" RESULT_SUFFIX MESSAGE_PREFIX "Unspecified error" MESSAGE_SUFFIX);
}


std::string WebHostEmulator::postForgot(const std::string &parameters)
{
	return std::string(RESULT_PREFIX "Forgot request not supported by server" RESULT_SUFFIX);
}


std::string WebHostEmulator::postChangePassword(const std::string &parameters)
{
	std::string oldpassword, password1, password2, username;

	username = ParseField(parameters, "username");
	oldpassword = ParseField(parameters, "oldpassword");
	password1 = ParseField(parameters, "password1");
	password2 = ParseField(parameters, "password2");

	if (username.empty() || oldpassword.empty() || password1.empty() || password2.empty()) {
		return std::string(RESULT_PREFIX "Change password failed" RESULT_SUFFIX MESSAGE_PREFIX "All fields must be completed" MESSAGE_SUFFIX);
	}

	if (password1 != password2) {
		return std::string(RESULT_PREFIX "Change password failed" RESULT_SUFFIX MESSAGE_PREFIX "New passwords do not match" MESSAGE_SUFFIX);
	}
	
	if (accountManager.Login(username, oldpassword) == 0) {
		accountManager.Change("password", username, password1, "");
		return std::string(RESULT_PREFIX "Success!" RESULT_SUFFIX MESSAGE_PREFIX"Your password has been changed." MESSAGE_SUFFIX);
	}
		
	return std::string(RESULT_PREFIX "Change password failed" RESULT_SUFFIX MESSAGE_PREFIX "An unknown error has occured." MESSAGE_SUFFIX);
}


std::string WebHostEmulator::postDelete(const std::string &parameters)
{
	std::string username1, username2, password;
	username1 = ParseField(parameters, "username1");
	username2 = ParseField(parameters, "username2");
	password = ParseField(parameters, "password");

	if (username1.empty() || username2.empty() || password.empty()) {
		return std::string(RESULT_PREFIX "Error" RESULT_SUFFIX MESSAGE_PREFIX "All fields must be completed" MESSAGE_SUFFIX);
	}

	std::string password_buffer = cfg::GetAdminPassword();
		
	if (password.compare(password_buffer)) {
		return std::string(RESULT_PREFIX "Error" RESULT_SUFFIX MESSAGE_PREFIX "Incorrect password" MESSAGE_SUFFIX);
	}

	if (username1 != username2) {
		return std::string(RESULT_PREFIX "Error" RESULT_SUFFIX MESSAGE_PREFIX "Username do not match" MESSAGE_SUFFIX);
	}

	int Result = accountManager.Delete(username1);
		
	if (Result == 0) {
		return std::string(RESULT_PREFIX "Success!" RESULT_SUFFIX MESSAGE_SUFFIX + username1  + " has been successfully deleted" MESSAGE_SUFFIX);
	}
	else if (Result == 1) {
		return std::string(RESULT_PREFIX "Error!" RESULT_SUFFIX MESSAGE_SUFFIX + username1 + " not found in database." MESSAGE_SUFFIX);
	}
	else if (Result == -1) {
		return std::string(RESULT_PREFIX "Error!" RESULT_SUFFIX MESSAGE_SUFFIX "Error: cannot open database." MESSAGE_SUFFIX);
	}
	else if (Result == -2) {
		return std::string(RESULT_PREFIX "Error!" RESULT_SUFFIX MESSAGE_SUFFIX "Error: database is corrupt." MESSAGE_SUFFIX);
	}

	return std::string(RESULT_PREFIX "Error!" RESULT_SUFFIX MESSAGE_SUFFIX "Error: Unknown error" MESSAGE_SUFFIX);
}
