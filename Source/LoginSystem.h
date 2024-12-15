#pragma once
#ifndef LOGINSYSTEM_H
#define LOGINSYSTEM_H
#include "Header.h"
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/prepared_statement.h>
using namespace sql;

string ConvertClientList(const vector<string> clientList);
vector<string> ConvertClientList(const string clientList);
struct LoginSystem {
    sql::mysql::MySQL_Driver* driver = nullptr;
    sql::Connection* con = nullptr;
	void ConnectDatabase();
	bool SearchAccount(const string user, const string password, Account& account);
	void InsertAccount(const string user, const string password, const string email);
	void DeleteAccount(const string user);
	void UpdateAccount(const string user, const string password, const string email, const vector<string> clientList);
	int GetClientID();
	LoginSystem();
};
#endif