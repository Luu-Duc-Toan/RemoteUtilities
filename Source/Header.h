#pragma once
#ifndef HEADER_H
#define HEADER_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 
#endif

#include <vector>
#include <string>
#include <cstdlib> 
#include <ctime>
#include <unordered_map>
#include <bitset>
#include <fstream>
#include <shlobj.h>
#include <shlwapi.h>
#include <io.h>
#include <fcntl.h>
#include <queue>
#include <stack>
#include <sstream>
#include <regex>
#include <dshow.h>
#include <filesystem>
#include <thread> 
#include <chrono>
#include <conio.h>
#include <atlimage.h>
#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <Windows.h>
#include <shellapi.h>

using namespace std;
extern string ComPath;
extern string UIPath;
extern string SystemPath;
extern filesystem::file_time_type modifiedTime;

#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "Shlwapi.lib") 
#pragma comment(lib, "Shell32.lib")

#include "WindowAPI.h"
#include "Socket.h"
#include "Account.h"
#include "LoginSystem.h"
#include "GmailAPI.h"

#endif