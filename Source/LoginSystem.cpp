#include "LoginSystem.h"

int CompareID(string& id1, string& id2) {
	int n = min(id1.size(), id2.size());
	for (int i = 1; i < n; i++) {
		if (id1[i] < id2[i]) return -1;
		if (id1[i] > id2[i]) return 1;
	}
	return 0;
}
int ExtractIDNum(string& id) {
	int num = 0;
	int n = id.size();
	for (int i = 1; i < n; i++) {
		num = num * 10 + id[i] - '0';
	}
	return num;
}
string ConvertClientList(const vector<string> clientList) {
	string clientListString = "";
	for (int i = 0; i < clientList.size(); i++) {
		clientListString += clientList[i];
	}
	return clientListString;
}
vector<string> ConvertClientList(const string clientList) {
	vector<string> clientListVector;
	int start = 0;
	int end = 0;
	while (end != -1) {
		end = clientList.find("C", start + 1);
		clientListVector.push_back(clientList.substr(start, end - start));
		start = end;
	}
	return clientListVector;
}
void LoginSystem::ConnectDatabase() {
	try {
		// Initialize the MySQL driver
		driver = sql::mysql::get_mysql_driver_instance();
		if (!driver) {
			cerr << "Failed to initialize MySQL driver!" << endl;
		}
		// Establish the connection to the database
		con = driver->connect("tcp://bkfy5wyg0h1ccu52fvxy-mysql.services.clever-cloud.com:3306",
			"urynz3xwolanyvcd",
			"pKqgv1j84Q10clZgc1zr");

		// Select the database
		con->setSchema("bkfy5wyg0h1ccu52fvxy");
	}
	catch (sql::SQLException& e) {
		cerr << "Error: " << e.what() << endl;
		cerr << "SQLState: " << e.getSQLState() << endl;
		cerr << "Error Code: " << e.getErrorCode() << endl;
	}
}
bool LoginSystem::isMatchGmail(const string user, const string gmail){
	sql::PreparedStatement* pstmt = con->prepareStatement(
		"SELECT * FROM users WHERE user = ?"
	);
	// Bind the user parameter and execute the statement
	pstmt->setString(1, user.c_str());
	// Execute the query and get the results
	sql::ResultSet* res = pstmt->executeQuery();
	if (!res->next()) {
		cout << "Invalid account" << endl;
		return false;
	}
	string emailTmp = res->getString("email");
	if (emailTmp != gmail) {
		return false;
	}
	delete pstmt;
	delete res;
	return true;
}
bool LoginSystem::isAccountExist(const string user) {
	// Prepare the SELECT statement with a WHERE clause
	sql::PreparedStatement* pstmt = con->prepareStatement(
		"SELECT email FROM users WHERE user = ?"
	);
	// Bind the user parameter and execute the statement
	pstmt->setString(1, user.c_str());
	// Execute the query and get the results
	sql::ResultSet* res = pstmt->executeQuery();
	if (!res->next()) {
		cout << "Invalid account" << endl;
		return false;
	}
	delete pstmt;
	delete res;
	return true;
}
bool LoginSystem::SearchAccount(const string user, const string password, Account &account) {
	// Prepare the SELECT statement with a WHERE clause
	sql::PreparedStatement* pstmt = con->prepareStatement(
		"SELECT * FROM users WHERE user = ?"
	);
	// Bind the user parameter and execute the statement
	pstmt->setString(1, user.c_str());
	// Execute the query and get the results
	sql::ResultSet* res = pstmt->executeQuery();
	if (!res->next()) {
		cout << "Invalid account" << endl;
		return false;
	}
	string idTmp = "A" + to_string(res->getInt("id"));
	string userTmp = res->getString("user");
	string passwordTmp = res->getString("password");
	string emailTmp = res->getString("email");
	vector<string> clientListTmp = ConvertClientList(res->getString("clientList"));
	if (clientListTmp.back().empty()) clientListTmp.pop_back(); 
	delete pstmt;
	delete res;
	if (password == passwordTmp)
	{
		cout << "Log in successfully!" << endl;
		account.adminID = idTmp;
		account.user = userTmp;
		account.password = passwordTmp;
		account.email = emailTmp;
		account.clientList = clientListTmp;
		return true;
	}
	else {
		cout << "Wrong password" << endl;
		return false;
	}
}
void LoginSystem::InsertAccount(const string user, const string password, const string email) {
	// Prepare the INSERT statement
	PreparedStatement* pstmt = con->prepareStatement(
		"INSERT INTO users (user, password, email, clientList) VALUES (?, ?, ?, ?)"
	);
	// Bind values and execute the statement
	pstmt->setString(1, user.c_str());
	pstmt->setString(2, password.c_str());
	pstmt->setString(3, email.c_str());
	pstmt->setString(4, "");
	pstmt->executeUpdate();
	cout << "Values inserted successfully!" << endl;
	delete pstmt;
}
void LoginSystem::DeleteAccount(const string user) {
	//Need SearchAccount before???

	PreparedStatement* pstmt = con->prepareStatement(
		"DELETE FROM users WHERE user = ?"
	);
	pstmt->setString(1, user.c_str());
	pstmt->executeUpdate();
	cout << "Values deleted successfully!" << endl;
	delete pstmt;
}
void LoginSystem::UpdatePassword(const string user, const string password) {
	PreparedStatement* pstmt = con->prepareStatement(
		"UPDATE users SET password = ? WHERE user = ?"
	);
	pstmt->setString(1, password.c_str());
	pstmt->setString(2, user.c_str());
	pstmt->executeUpdate();
	delete pstmt;
}
void LoginSystem::UpdateClientID(const string user, const vector<string> clientList) {
	string clientListString = ConvertClientList(clientList);

	PreparedStatement* pstmt = con->prepareStatement(
		"UPDATE users SET clientList = ? WHERE user = ?"
	);
	pstmt->setString(1, clientListString.c_str());
	pstmt->setString(2, user.c_str());
	pstmt->executeUpdate();
	cout << "Values updated successfully!" << endl;
	delete pstmt;
}
int LoginSystem::GetMaxClientID() {
	sql::PreparedStatement* pstmt = con->prepareStatement(
		"SELECT * FROM clientID"
	);
	ResultSet* res = pstmt->executeQuery();
	if (!res->next()) return -1;
	int clientID = res->getInt("id");
	delete pstmt;
	delete res;
	return clientID;
}
int LoginSystem::UpdateMaxClientID() {
	int clientID = GetMaxClientID();
	sql::PreparedStatement* pstmt = con->prepareStatement(
		"UPDATE clientID SET id = ? WHERE id = ?"
	);
	pstmt->setInt(1, clientID + 1);
	pstmt->setInt(2, clientID);
	pstmt->executeUpdate();
	ResultSet* res = pstmt->executeQuery();
	delete pstmt;
	delete res;
	return clientID;
}
LoginSystem::LoginSystem() {
	ConnectDatabase();
}