#pragma once
#include "baseHeader.h"
#include "CurlRequest.h"

using namespace std;

class CourseSelect
{
public:
	void setCookie(string* cookie);
	void setCourse(string course);
	void setCourseID(string courseID);
private:
	string* cookie;
	string course;
	string courseID;
public:
	bool courseSelectAttempt(UINT& needStop);
	bool courseSelectProcess();
private:
	string findErrorSignal(string input);
};

