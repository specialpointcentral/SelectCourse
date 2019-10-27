#include "CurlRequest.h"

map<string, string> CurlRequest::curl(string url, const map<string, string>& post_data, const vector<string>& header, string referer_url, string* cookieStr)
{
	CURL* hnd = curl_easy_init();
	std::stringstream stream;
	if (post_data.size() != 0) {
		curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
		// 设置一个有趣的东西
		string suffix("?jym2005=");
		double times = ((time(NULL) * 9301 + 49297) % 233280) / (233280.0);
		double rands = (rand() % INT_MAX) / (double)(INT_MAX - 1) ;
		times = (times + rands) * 9301;
		stringstream ss;
		ss.clear();
		ss.precision(16);
		ss << times;
		suffix.append(ss.str());
		url.append(suffix);
	}
	else {
		curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
	}
	curl_easy_setopt(hnd, CURLOPT_URL, url.data());
	string post_data_s;
	for (map<string, string>::const_iterator it = post_data.begin(); it != post_data.end();) {
		post_data_s.append(it->first + "=" + it->second);
		if (++it != post_data.end()) post_data_s.append("&");
	}
	curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, post_data_s.data());

	struct curl_slist* headers = NULL;
	for (vector<string>::const_iterator it = header.begin(); it != header.end(); ++it) {
		headers = curl_slist_append(headers, it->data());
	}
	headers = curl_slist_append(headers, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
	headers = curl_slist_append(headers, "Accept-Language: zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2");
	headers = curl_slist_append(headers, "Connection: keep-alive");
	headers = curl_slist_append(headers, "Cache-Control: no-cache");
	headers = curl_slist_append(headers, "Upgrade-Insecure-Requests: 1");
	headers = curl_slist_append(headers, "Pragma: no-cache");
	curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

	/**配置返回信息**/
	//curl_easy_setopt(hnd, CURLOPT_RETURNTRANSFER, 1); //成功只返回结果
	curl_easy_setopt(hnd, CURLOPT_HEADER, 0); //不返回header部分
	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &stream);

	srand((INT)time(NULL));
	/**配置超时**/
	curl_easy_setopt(hnd, CURLOPT_CONNECTTIMEOUT, 8 + rand() % 5); //连接前等待时间,0不等待
	curl_easy_setopt(hnd, CURLOPT_TIMEOUT, 5 + rand() % 10); //连接后等待时间,0不等待。如下载mp3

	/**配置页面重定向**/
	//curl_easy_setopt(hnd, CURLOPT_FOLLOWLOCATION, 1); //跟踪爬取重定向页面
	//curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 10); //指定最多的HTTP重定向的数量

	// curl_easy_setopt(curl, CURLOPT_COOKIELIST, cookies);

	/**配置Header、请求头、协议信息**/
	curl_easy_setopt(hnd, CURLOPT_ENCODING, "gzip, deflate"); //Accept-Encoding编码，支持"identity"/"deflate"/"gzip",空支持所有编码
	curl_easy_setopt(hnd, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:70.0) Gecko/20100101 Firefox/70.0"); //模拟浏览器头信息
	if (referer_url.length() > 0) curl_easy_setopt(hnd, CURLOPT_REFERER, referer_url.data()); //伪造来源地址

	if (cookieStr != nullptr && cookieStr->length() > 0)
	{
		curl_easy_setopt(hnd, CURLOPT_COOKIELIST, cookieStr->data());
	}
	curl_easy_setopt(hnd, CURLOPT_COOKIEFILE, ""); //启动cookie引擎 

	CURLcode ret, code = curl_easy_perform(hnd);

	struct curl_slist* cookies;
	ret = curl_easy_getinfo(hnd, CURLINFO_COOKIELIST, &cookies);
	if (cookieStr != nullptr && ret == CURLE_OK)
	{
		struct curl_slist* each = cookies;
		while (each)
		{
			if (cookieStr->length() > 0)
			{
				*cookieStr = *cookieStr + "\r\n";
			}
			//printf("cookie:%s\n", each->data);
			*cookieStr = *cookieStr + each->data;//获取cookie
			each = each->next;
		}
	}
	curl_slist_free_all(cookies);

	// printf("ret0:%d\n", ret);
	LONG nHttpCode = 0;
	// 取下HTTP返回状态码(200为成功)
	ret = curl_easy_getinfo(hnd, CURLINFO_RESPONSE_CODE, &nHttpCode);
	// printf("ret1:%d\n", ret);
	// printf("nHttpCode:%d\n", nHttpCode);
	curl_easy_cleanup(hnd);

	map<string, string> return_data;
	return_data.insert(make_pair<string,string>("code", to_string(code)));
	return_data.insert(make_pair<string, string>("data",stream.str()));
	return_data.insert(make_pair<string, string>("httpcode", to_string(nHttpCode)));
	return return_data;
}

size_t CurlRequest::write_data(char* ptr, size_t size, size_t nmemb, void* userdata) {
	std::stringstream* stream = (std::stringstream*)userdata;
	size_t count = size * nmemb;
	stream->write(ptr, count);
	return count;
}