﻿#pragma once
#ifndef SOCKET_H
#define SOCKET_H



#define DEFAULT_PORT "8080"
#include "Header.h"
#include <winsock2.h>
#include <ws2tcpip.h> //for IP Helper API
#include <iphlpapi.h> //for IP Helper API

#pragma comment(lib, "Ws2_32.lib") //Winsock Library
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

extern bool initWinsock;
const int maxBufferSize = 1024;

struct ServerSocket {
	SOCKET listenSocket;
	SOCKET clientSocket;
	bool isServerOn;
	bool isKeyloggerOn = false;
	bool isWebcamOn = false;
	thread keyloggerThread;
	thread webcamThread;
	addrinfo* serverAddr = NULL;
	string result;
	char buffer[maxBufferSize];
	void GetServerAddrInfo() {
		addrinfo hints;
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		if (getaddrinfo(NULL, DEFAULT_PORT, &hints, &serverAddr) != 0) {
			cout << "getaddrinfo failed: %d\n";
			WSACleanup();
			exit(1);
		}
	}
	void SetSocketOption() {
		int opt = 1;
		if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
			cout << "Set socket options failed! Error: " << WSAGetLastError() << endl;
			closesocket(listenSocket);
			WSACleanup();
		}
	}
	void BindSocket() {
		if (::bind(listenSocket, serverAddr->ai_addr, (int)serverAddr->ai_addrlen) == SOCKET_ERROR) {
			cout << "Bind failed: " << WSAGetLastError() << endl;
			closesocket(listenSocket);
			WSACleanup();
			exit(1);
		}
		freeaddrinfo(serverAddr);
		serverAddr = NULL;
	}
	void Accept() {
		clientSocket = accept(listenSocket, NULL, NULL);
		if (clientSocket == INVALID_SOCKET) {
			cout << "Accept failed: " << WSAGetLastError() << endl;
			closesocket(listenSocket);
			WSACleanup();
		}
		cout << "Client connected!" << endl;
	}
	void Shutdown() {
		if (shutdown(clientSocket, SD_SEND) == SOCKET_ERROR) {
			printf("shutdown failed with error: %d\n", WSAGetLastError());
			closesocket(clientSocket);
			WSACleanup();
		}
	}
	void Listen() {
		if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
			cout << "Listen failed: " << WSAGetLastError() << endl;
			closesocket(listenSocket);
			WSACleanup();
		}
		cout << "Server is listening on port " << DEFAULT_PORT << endl;
		Accept();
	}
	void Send() {
		if (send(clientSocket, result.c_str(), (int)result.size(), 0) == SOCKET_ERROR) {
			cout << "send failed: %d" << endl;
			WSAGetLastError();
			closesocket(clientSocket);
			WSACleanup();
		}
		cout << "Message: " << result << endl;
	}
	void ProcessClientMessage();
	void Receive() {
		int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
		if (bytesReceived > 0) {
			buffer[bytesReceived] = '\0';
			cout << "Message from client: " << buffer << endl;
			ProcessClientMessage();
		}
		else if (bytesReceived == 0) {
			cout << "Connection closed" << endl;
		}
		else {
			cout << "Receive failed: " << WSAGetLastError() << endl;
		}
	}
	void Close() {
		if (clientSocket != INVALID_SOCKET) {
			closesocket(clientSocket);
			clientSocket = INVALID_SOCKET;
		}
		if (listenSocket != INVALID_SOCKET) {
			closesocket(listenSocket);
			listenSocket = INVALID_SOCKET;
		}
	}
	ServerSocket();
	void Reset() {
		Shutdown();
		Close();
		isKeyloggerOn = false;
		isWebcamOn = false;
		isServerOn = false;
		GetServerAddrInfo();
		listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		SetSocketOption();
		if (listenSocket == INVALID_SOCKET) {
			cout << "Socket creation failed: " << WSAGetLastError() << endl;
			WSACleanup();
		}
		BindSocket();
		keyloggerThread.join();
		//webcamThread.join();
	}
};
struct ClientSocket {
	SOCKET clientSocket;
	addrinfo* serverAddr = NULL;
	char buffer[512];

	void GetHostName(char*& hostname) {
		const int length = 256;
		hostname = new char[256];
		if (gethostname(hostname, length) == SOCKET_ERROR) {
			cout << "gethostname failed: " << WSAGetLastError() << endl;
			WSACleanup();
			exit(1);
		}
	}
	void GetServerAddrInfo() {
		addrinfo hints;
		char* hostName = NULL;
		GetHostName(hostName);
		cout << "Host name: " << hostName << endl;
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		if (getaddrinfo(hostName, DEFAULT_PORT, &hints, &serverAddr) != 0) {
			cout << "getaddrinfo failed: %d\n";
			WSACleanup();
			exit(1);
		}
	}
	void ConnectToServer() {
		cout << "Waiting Server..." << endl;
		while(connect(clientSocket, serverAddr->ai_addr, (int)serverAddr->ai_addrlen) == SOCKET_ERROR)
		{
			Sleep(10);
		}
		cout << "Connected to server!" << endl;
		if (serverAddr) 
			freeaddrinfo(serverAddr);
	}
	void Shutdown() {
		if (shutdown(clientSocket, SD_SEND) == SOCKET_ERROR) {
			printf("shutdown failed with error: %d\n", WSAGetLastError());
			closesocket(clientSocket);
			WSACleanup();
		}
	}
	void Send(const char* message) {
		send(clientSocket, message, strlen(message), 0);
		cout << "Message: " << message << endl;
	}
	void Receive() {
		int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
		if (bytesReceived > 0) {
			buffer[bytesReceived] = '\0';
			cout << "Message from server: " << buffer << endl;
			if (buffer[0] == 'F') {
				//file
				size_t fileSize = 0;
				int index = 1;
				while (buffer[index] != '\0') {
					fileSize = fileSize * 10 + buffer[index] - '0';
					index++;
				}
				string bitString = "";
				while (bitString.size() != fileSize) {
					recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
					bitString += string(buffer);
				}
				//For testing
				fstream output("_Data/output.txt", ios::binary | ios::out);
				output << bitString;
				output.close();
			}
			else if (buffer[0] == 'L') {
				//List app,...
			}
		}
		else if (bytesReceived == 0) {
			cout << "Connection closed" << endl;
		}
		else {
			cout << "Receive failed: " << WSAGetLastError() << endl;
		}
	}
	void Close() {
		if (clientSocket != INVALID_SOCKET) {
			closesocket(clientSocket);
		}
	}
	ClientSocket() {
		GetServerAddrInfo();
		clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (clientSocket == INVALID_SOCKET) {
			cout << "Socket creation failed: " << WSAGetLastError() << endl;
			WSACleanup();
		}
	}
	void Reset() {
		Shutdown();
		Close();
		GetServerAddrInfo();
		clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (clientSocket == INVALID_SOCKET) {
			cout << "Socket creation failed: " << WSAGetLastError() << endl;
			WSACleanup();
		}
	}
};
void InitWinsock(WSADATA& wsadata);
void CloseWinsock();
#endif // !SOCKET_H
