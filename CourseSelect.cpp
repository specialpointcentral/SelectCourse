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
	// 1.�ɹ�		200		CURLE_OK	data���п����	out
	// 2.δ��¼		200		CURLE_OK	��				out
	// 3.��ʱ		xxx		other		��				loop
	// 4.�޷�ѡ��		200		CURLE_OK	data��Ϣ			out
	// 5.��Ȩ��		200		CURLE_OK	data			loop

	if (get["code"] == to_string(CURLE_OK) && get["httpcode"] == to_string(200)) {
		string data = get["data"];
		if (get["data"].length() == 0 || data.find("���ȵ�¼��ʹ��!!!") != data.npos) {
			// 2.δ��¼
			printf("Not login!\n");
			needStop = 1 << 1;
			return false;
		}
		else {
			if (data.find("<p align=\"center\">" + course + "</p>") == data.npos) {
				if (data.find("��Ŀǰû��Ȩ��ѡ��") == data.npos) {
					// 4.�޷�ѡ��
					string info = findErrorSignal(data);
					printf("[%s(%s)]Info: %s\n", course.data(), courseID.data(), info.data());
					needStop = 1 << 2;
					return false;
				}
				else {
					// 5.��Ȩ��
					printf("[%s(%s)]Info: No permmsion!Wait and Retry!\n", course.data(), courseID.data());
					needStop = 1 << 3;
					return false;
				}
			}
			else {
				// 1.�ɹ�
				needStop = 0;
				printf("[%s(%s)]Select over!\n", course.data(), courseID.data());
				return true;
			}
		}
	}
	else {
		// 3.��ʱ
		printf("[%s(%s)]Maybe Timeout, retry! [%s]\n", course.data(), courseID.data(), get["code"].data());
		needStop = 1 << 0;
	}
	return false;
}


bool CourseSelect::courseSelectProcess()
{
	// 1.�ɹ�		200		CURLE_OK	data���п����	out true
	// 2.δ��¼		200		CURLE_OK	��				out false
	// 3.��ʱ		xxx		other		��				loop
	// 4.�޷�ѡ��		200		CURLE_OK	data��Ϣ			out true
	// 5.��Ȩ��		200		CURLE_OK	data			loop
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
	// �ж��Ƿ���Ҫ��������
	switch (needStop)
	{
	case 1 << 1:
		// δ��¼
		return false;
		break;
	default:
		break;
	}
	return true;
}

string CourseSelect::findErrorSignal(string input) {
	smatch sm;
	regex e("(<font\\ color=\"#FF0000\">[\\ |\n|\r]*������ѡ�ε�[\\ |\n|\r]*<b>[\\ |\n|\r]*���ڲ���ѡ[\\ |\n|\r]*<\\/b>[\\ |\n|\r]*�׶�[\\ |\n|\r]*<br>[\\ |\n|\r]*ѡ��Ԥѡ�׶��趨�Ľ׶�ʱ���ǣ�[\\ |\n|\r]*<br>[\\ |\n|\r]*��ѡ�׶�ʱ�������ǣ�[\\ |\n|\r]*<br>[\\ |\n|\r]*����ѡ�׶�ʱ�������ǣ�[\\ |\n|\r]*<br>[\\ |\n|\r]*)(.*)([\\ |\n|\r]*<\\/font>)", regex::icase);
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