#include "CourseSelect.h"


void CourseSelect::setCookie(string* cookie)
{
	this->cookie = cookie;
}


void CourseSelect::setCourse(string course)
{
	this->course = course;
}


void CourseSelect::setCourseID(string courseID)
{
	this->courseID = courseID;
}


bool CourseSelect::courseSelectAttempt(UINT& needStop)
{
	// b'0000 - no error
	// b'0001 - timeout
	// b'0010 - not login
	// b'0100 - select error
	// b'1000 - no permission
	needStop = 0;
	map<string, string> course_info;
	vector<string> header;
	course_info.insert(pair<string, string>("p_qxrxk", course));
	course_info.insert(pair<string, string>("p_qxrxk_kxh", courseID));
	header.push_back("Content-Type:application/x-www-form-urlencoded");

	map<string, string> get = CurlRequest::curl("http://222.194.15.1:7777/pls/wwwbks/xk.CourseInput", course_info, header, "http://222.194.15.1:7777/pls/wwwbks/xk.CourseInput", cookie);
	// 1.成功		200		CURLE_OK	data中有课序号	out
	// 2.未登录		200		CURLE_OK	空				out
	// 3.超时		xxx		other		空				loop
	// 4.无法选择		200		CURLE_OK	data信息			out
	// 5.无权限		200		CURLE_OK	data			loop

	if (get["code"] == to_string(CURLE_OK) && get["httpcode"] == to_string(200)) {
		string data = get["data"];
		if (get["data"].length() == 0 || data.find("请先登录再使用!!!") != data.npos) {
			// 2.未登录
			printf("Not login!\n");
			needStop = 1 << 1;
			return false;
		}
		else {
			if (data.find("<p align=\"center\">" + course + "</p>") == data.npos) {
				if (data.find("你目前没有权限选课") == data.npos) {
					// 4.无法选择
					string info = findErrorSignal(data);
					printf("[%s(%s)]Info: %s\n", course.data(), courseID.data(), info.data());
					needStop = 1 << 2;
					return false;
				}
				else {
					// 5.无权限
					printf("[%s(%s)]Info: No permmsion!Wait and Retry!\n", course.data(), courseID.data());
					needStop = 1 << 3;
					return false;
				}
			}
			else {
				// 1.成功
				needStop = 0;
				printf("[%s(%s)]Select over!\n", course.data(), courseID.data());
				return true;
			}
		}
	}
	else {
		// 3.超时
		printf("[%s(%s)]Maybe Timeout, retry! [%s]\n", course.data(), courseID.data(), get["code"].data());
		needStop = 1 << 0;
	}
	return false;
}


bool CourseSelect::courseSelectProcess()
{
	// 1.成功		200		CURLE_OK	data中有课序号	out true
	// 2.未登录		200		CURLE_OK	空				out false
	// 3.超时		xxx		other		空				loop
	// 4.无法选择		200		CURLE_OK	data信息			out true
	// 5.无权限		200		CURLE_OK	data			loop
	srand((UINT)time(NULL));
	UINT needStop = 0;
	while (1) {
		if (needStop & 6) break;
		bool get = courseSelectAttempt(needStop);
		if (get) {
			break;
		}
		else {
			Sleep(500 + rand() % needStop * 500);
		}
	}
	// 判断是否需要继续请求
	switch (needStop)
	{
	case 1 << 1:
		// 未登录
		return false;
		break;
	default:
		break;
	}
	return true;
}

string CourseSelect::findErrorSignal(string input) {
	smatch sm;
	regex e("(<font\\ color=\"#FF0000\">[\\ |\n|\r]*现在是选课的[\\ |\n|\r]*<b>[\\ |\n|\r]*正在补退选[\\ |\n|\r]*<\\/b>[\\ |\n|\r]*阶段[\\ |\n|\r]*<br>[\\ |\n|\r]*选课预选阶段设定的阶段时间是：[\\ |\n|\r]*<br>[\\ |\n|\r]*正选阶段时间设置是：[\\ |\n|\r]*<br>[\\ |\n|\r]*补退选阶段时间设置是：[\\ |\n|\r]*<br>[\\ |\n|\r]*)(.*)([\\ |\n|\r]*<\\/font>)", regex::icase);
	regex_search(input, sm, e);
	if (sm.size() < 3) return "";
	if (sm[2].length() == 0) {
		return sm[2];
	}
	else {
		regex re("(.*)([\\ |\n|\r]*<BR>)", regex::icase);
		string info = sm[2];
		smatch m;
		regex_search(info, m, re);
		if (m.size() > 2)
			return m[1];
		else
			return "";
	}
}