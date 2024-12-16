#pragma once
#ifndef ACCOUNT_H
#define ACCOUNT_H
#include "Header.h"
enum class Role {
	CLIENT_SERVER,
	ADMIN,
	NONE
};

struct Account {
	Role role;
	string clientID; 
	string adminID;
	string user;
	string password;
	string email;
	vector<string> clientList;
	void SetRole(Role role) {
		this->role = role;
	}
	void SetClientID(const string clientID) {
		this->clientID = clientID;
	}
	void AddClientID(const string clientIDs) {
		clientList.push_back(clientIDs);
	}
	void RemoveClientID(const string clientID) {
		int n = clientList.size();
		for (int i = 0; i < n; i++) {
			if (clientID == clientList[i]) {
				clientList.erase(clientList.begin() + i);
				break;
			}
		}
	}
	void SetPassword(const string password) {
		this->password = password;
	}
	void SetEmail(const string email) {
		this->email = email;
	}
	void LoadClientID() {
		fstream file;
		file.open("_Data/ClientID.bin", ios::binary | ios::in);

		if (!file.is_open()) {
			std::cerr << "File does not exist or cannot be opened.\n";
			return;
		}

		size_t length;
		file.read(reinterpret_cast<char*>(&length), sizeof(length));
		clientID.resize(length);
		file.read(&clientID[0], length);
		file.close();
	}
	void Save() {
		fstream file;
		file.open("_Data/ClientID.bin", ios::binary | ios::out);
		size_t length = clientID.size();
		file.write(reinterpret_cast<char*>(&length), sizeof(length));
		file.write(&clientID[0], length);
		file.close();
	}
	Account(const string id, const string user, const string password, 
		const string email, const vector<string> clientList) {
		this->adminID = id;
		this->user = user;
		this->password = password;
		this->email = email;
		this->clientList = clientList;
		LoadClientID();
		role = Role::ADMIN;
	}
	Account() {
		this->adminID = "";
		this->user = "";
		this->password = "";
		this->email = "";
		this->clientList = {};
		LoadClientID();
		role = Role::NONE;
	}
	~Account() {
		Save();
	}
};
#endif