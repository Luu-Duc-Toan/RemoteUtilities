#include "LoginSystem.h"

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
			std::cerr << "Failed to initialize MySQL driver!" << std::endl;
		}
		// Establish the connection to the database
		con = driver->connect("tcp://bkfy5wyg0h1ccu52fvxy-mysql.services.clever-cloud.com:3306",
			"urynz3xwolanyvcd",
			"pKqgv1j84Q10clZgc1zr");

		// Select the database
		con->setSchema("bkfy5wyg0h1ccu52fvxy");
	}
	catch (sql::SQLException& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << "SQLState: " << e.getSQLState() << std::endl;
		std::cerr << "Error Code: " << e.getErrorCode() << std::endl;
	}
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
	delete pstmt;
	delete res;
	if (password == passwordTmp)
	{
		cout << "Log in successfully!" << endl;
		account = Account(idTmp, userTmp, passwordTmp, emailTmp, clientListTmp);
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
void LoginSystem::UpdateAccount(const string user, const string password, const string email, const vector<string> clientList) {
	string clientListString = ConvertClientList(clientList);

	PreparedStatement* pstmt = con->prepareStatement(
		"UPDATE users SET password = ?, email = ?, clientList = ? WHERE user = ?"
	);
	pstmt->setString(1, password.c_str());
	pstmt->setString(2, email.c_str());
	pstmt->setString(3, clientListString.c_str());
	pstmt->setString(4, user.c_str());
	pstmt->executeUpdate();
	cout << "Values updated successfully!" << endl;
	delete pstmt;
}
int LoginSystem::GetClientID() {
	sql::PreparedStatement* pstmt = con->prepareStatement(
		"SELECT * FROM clientID"
	);
	ResultSet* res = pstmt->executeQuery();
	if (!res->next()) return -1;
	int clientID = res->getInt("id");
	pstmt = con->prepareStatement(
		"UPDATE clientID SET id = ? WHERE id = ?"
	);
	pstmt->setInt(1, clientID + 1);
	pstmt->setInt(2, clientID);
	pstmt->executeUpdate();
	delete pstmt;
	delete res;
	return clientID;
}
LoginSystem::LoginSystem() {
	ConnectDatabase();
}