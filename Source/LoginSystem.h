#pragma once
#ifndef LOGINSYSTEM_H
#define LOGINSYSTEM_H
#include "Header.h"
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/prepared_statement.h>
using namespace sql;

int CompareID(string& id1, string& id2);
int ExtractIDNum(string& id);
string ConvertClientList(const vector<string> clientList);
vector<string> ConvertClientList(const string clientList);
struct LoginSystem {
    sql::mysql::MySQL_Driver* driver = nullptr;
    sql::Connection* con = nullptr;
	void ConnectDatabase();
	bool isAccountExist(const string user);
	void UpdatePassword(const string user, const string password);
	bool isMatchGmail(const string user, const string gmail);
	bool SearchAccount(const string user, const string password, Account& account);
	void InsertAccount(const string user, const string password, const string gmail);
	void DeleteAccount(const string user);
	void UpdateClientID(const string user, const vector<string> clientList);
	int GetMaxClientID();
	int UpdateMaxClientID();
	LoginSystem();
};
#endif