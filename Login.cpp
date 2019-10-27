#include "Login.h"

Login::Login()
{
	username.clear();
	password.clear();
	cookie = nullptr;
}

void Login::setUsername(std::string username)
{
	this->username = username;
}


void Login::setPassword(std::string password)
{
	this->password = password;
}


void Login::setCookie(std::string* cookie)
{
	this->cookie = cookie;
}

bool Login::loginProcess()
{
	srand((UINT)time(NULL));
	bool needStop = false;
	while (1) {
		if (needStop) break;
		bool get = loginAttemp(needStop);
		if (get) {
			break;
		}
		else {
			Sleep(500 + rand() % 500);
		}
	}
	// 判断是否需要继续请求
	if (needStop) {
		return false;
	}
	else {
		printf("[Login]Login successfully!\n");
		return true;
	}
}

bool Login::loginAttemp(bool &needStop)
{
	needStop = false;
	map<string, string> login_info;
	vector<string> header;
	login_info.insert(pair<string, string>("stuid", username));
	login_info.insert(pair<string, string>("pwd", password));
	header.push_back("Content-Type:application/x-www-form-urlencoded");

	map<string, string> get = CurlRequest::curl("http://222.194.15.1:7777/pls/wwwbks/bks_login2.login", login_info, header, "http://222.194.15.1:7777/zhxt_bks/xk_login.html", cookie);
	if (get["httpcode"] == "302") return true;
	if (get["code"] == to_string(CURLE_OK) && get["httpcode"] == "200") {
		printf("[Login]Login error! Username or Password incorrect!\n"); 
		needStop = true;
	}
	else { 
		printf("[Login]Maybe Timeout, retry! [%s]\n", get["code"].data()); 
	}
	return false;
}
