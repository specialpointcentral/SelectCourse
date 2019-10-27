// SelectCourse.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <tchar.h>
#include <strsafe.h>
#include "baseHeader.h"
#include "CurlRequest.h"
#include "Login.h"
#include "CourseSelect.h"

int main(int argc, char* argv[])
{
	cout << "+-------------------------------------+" << endl
		<< "|            选课系统 v1.1            |" << endl
		<< "+-------------------------------------+" << endl;

	string username, password;
	vector<pair<string, string>> courselist;
	// 参数部分
	bool u = false, p = false, c = false;
	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "-u") == 0) {
			if (i + 1 >= argc) break;
			username = string(argv[++i]);
			u = true;
		}
		else if (strcmp(argv[i], "-p") == 0) {
			if (i + 1 >= argc) break;
			password = string(argv[++i]);
			p = true;
		}
		else {
			if (i + 1 >= argc) break;
			string course = argv[i], id = argv[++i];
			courselist.push_back(pair<string, string>(course, id));
			c = true;
		}
	}

	if (!(u && p && c)) {
		if (argc > 1) {
			cout << "参数输入格式：" << endl;
			cout << "-u 用户名 -p 密码 课程号 课序号 课程号 课序号 ..." << endl;
			cout << "您输入的参数有误，您需要再次输入！" << endl;
		}
		cout << "学号:";
		cin >> username;
		cout << "密码:";
		cin >> password;
		cout << "选课录入:（使用 ctrl+z 结束输入）" << endl;

		courselist.clear();
		string course, courseID;
		while (1) {
			cout << "┌ 课程号:";
			if (!(cin >> course)) break;
			cout << "└ 课序号:";
			if (!(cin >> courseID)) break;
			courselist.push_back(pair<string, string>(course, courseID));
		}
	}

	TCHAR szOldTitle[MAX_PATH];
	TCHAR szNewTitle[MAX_PATH];
	GetConsoleTitle(szOldTitle, MAX_PATH);
	StringCchPrintf(szNewTitle, MAX_PATH, TEXT("%s [%s]"), szOldTitle, username.data());
	SetConsoleTitle(szNewTitle);

	cout << "\n你录入了" << courselist.size() << "门课程" << endl;
	bool logint = false, courset = false;
	string cookie;
	Login login;
	CourseSelect courseSelect;
	login.setCookie(&cookie);
	courseSelect.setCookie(&cookie);

	login.setPassword(password);
	login.setUsername(username);
	vector<pair<string, string>>::iterator it = courselist.begin();
	while (!courset && it != courselist.end()) {
		cookie.clear();
		logint = login.loginProcess();
		if (!logint) break;
		for (; it != courselist.end(); ++it) {
			courseSelect.setCourse(it->first);
			courseSelect.setCourseID(it->second);
			courset = courseSelect.courseSelectProcess();
			if (!courset) break;
			srand((INT)time(NULL));
			Sleep(800 + rand() % 800);
		}
	}

	/*
	map<string, string> tmp;
	vector<string> header;
	static string cookieStr = "";
	tmp.insert(make_pair<string, string>("stuid", "160820321"));
	tmp.insert(make_pair<string, string>("pwd", "hu990114"));
	header.push_back("Content-Type:application/x-www-form-urlencoded");

	//CurlRequest::curl("http://222.194.15.1:7777/pls/wwwbks/bks_login2.login",tmp, vector<string>(),"", &cookieStr);
	//CurlRequest::curl("http://222.194.15.1:7777/pls/wwwbks/bks_login2.loginmessage", map<string,string>(),vector<string>(), "", &cookieStr);

	map<string, string> course;
	course.insert(make_pair<string, string>("p_qxrxk", "1"));
	course.insert(make_pair<string, string>("p_qxrxk_kxh", "0"));
	CurlRequest::curl("http://222.194.15.1:7777/pls/wwwbks/xk.CourseInput", course, vector<string>(), "", &cookieStr);
	*/
	cout << "任务结束！" << endl;
	cout << "按任意键退出程序！";
	cin.clear();
	cin.sync();
	cin.get();
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
