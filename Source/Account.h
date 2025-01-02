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
	void AddClientID(const string clientID) {
		clientList.push_back(clientID);
	}
	bool RemoveClientID(const string clientID) {
		int n = clientList.size();
		for (int i = 0; i < n; i++) {
			if (clientID == clientList[i]) {
				clientList.erase(clientList.begin() + i);
				return true;
			}
		}
		return false;
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
			cerr << "File does not exist or cannot be opened.\n";
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
	void Reset() {
		this->adminID = "";
		this->user = "";
		this->password = "";
		this->email = "";
		this->clientList = {};
		role = Role::NONE;
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
};
#endif