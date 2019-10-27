#pragma once
#ifdef  _DEBUG
#include "DLL Debug/include/curl.h"
#pragma comment ( lib, "DLL Debug/libcurld.lib" )
#else
#include "DLL Release/include/curl.h"
#pragma comment ( lib, "DLL Release/libcurl.lib" )
#endif //  _DEBUG

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <regex>
#include <climits>
#include <sstream>
