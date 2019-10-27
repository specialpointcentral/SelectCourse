#pragma once
#include "baseHeader.h"
#include "CurlRequest.h"
using namespace std;

class Login
{
public:
	Login();
	void setUsername(std::string username);
	void setPassword(std::string password);
	void setCookie(std::string* cookie);

	bool loginProcess();

private:
	std::string username;
	std::string password;
	std::string* cookie;

	bool loginAttemp(bool &neetStop);

};

