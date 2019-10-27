#pragma once
#include "baseHeader.h"

using namespace std;

class CurlRequest
{
public:
	static map<string, string> curl(string url, const map<string, string>& post_data = map<string, string>(), const vector<string>& header = vector<string>(), string referer_url = "", string* cookieStr = nullptr);
private:
	static size_t write_data(char* ptr, size_t size, size_t nmemb, void* userdata);
};

