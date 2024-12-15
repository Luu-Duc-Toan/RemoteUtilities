#include "Header.h"
MyCurl myCurl;
LoginSystem loginSystem;
Account account;
WSADATA wsaData;
ClientSocket client;
ServerSocket server;
#define COMMENT
void GetRole();
void ClientRun(Role& role);
void ServerRun(Role& role);
void AdminRun();
void Client_ServerRun();

void ClientRun(Role& role) {
	client.ConnectToServer();
	while (role == Role::CLIENT_SERVER) {
		if (!myCurl.emailQueue.empty()) {
			myCurl.ClientProcess();
			myCurl.emailQueue.pop();
			string content = account.clientID;
			if (myCurl.ShouldSendToServer()) {
				client.Send(myCurl.query.c_str());
				client.Receive();
				content += ";" + string(client.buffer);
			}
			else {
				content += ";" + myCurl.result;
			}
			myCurl.SendEmail({ myCurl.receiverID }, content);
		}
	}
}
void ServerRun(Role& role) {
	server.Listen();
	while (role == Role::CLIENT_SERVER) {
		server.Receive();
	}
}
void Client_ServerRun() {
	myCurl.isAutoReceiving = true;
	thread serverThread(ServerRun, ref(account.role));
	thread clientThread(ClientRun, ref(account.role));
	while (true) {
		if (_kbhit()) {
			char ch = _getch();
			if (ch == 's') {
				myCurl.isAutoReceiving = false;
				account.role = Role::NONE;
				break;
			}
		}
	}
	clientThread.join();
	serverThread.join();
	client.Reset();
	server.Reset();
	return GetRole();
}
void AdminRun() {
	myCurl.isAutoReceiving = true;
	while (true) {
		int query = -1;
		cout << "Query: ";  cin >> query;
		if (query == -1) continue;
		else if (query == -2) { //exit
			//log out,
			myCurl.isAutoReceiving = false;
			account.role = Role::NONE;
			break;
		}
		else if (query == 1) { //for 1 - 10: dont need to send email: change password, email
			string password, newPassword;
			while (true) {
				cout << "Password: "; cin >> password;
				cout << "New password: "; cin >> newPassword;
				if(password != account.password)
					cout << "Wrong password!" << endl;
				else break;
			}
			account.SetPassword(newPassword);
			loginSystem.UpdateAccount(account.user, account.password, account.email, account.clientList);
		}
		else if (query == 2) {
			string email, newEmail;
			while (true)
			{
				cout << "Email: "; cin >> email;
				cout << "New email: "; cin >> newEmail;
				if (email != account.email)
					cout << "Wrong email!" << endl;
				else break;
			}
			account.SetEmail(newEmail);
			loginSystem.UpdateAccount(account.user, account.password, account.email, account.clientList);
		}
		else if (query == 3) {
			string newClientID;
			cout << "New clientID: "; cin >> newClientID;
			string content = account.adminID + ";" + to_string(query);
			myCurl.SendEmail({newClientID}, content);
			myCurl.ReadEmail();
			account.AddClientID(newClientID);
			loginSystem.UpdateAccount(account.user, account.password, account.email, account.clientList);
		}
		else if (query == 4) {
			string removeClientID;
			cout << "Remove clientID: "; cin >> removeClientID;
			account.RemoveClientID(removeClientID);
			loginSystem.UpdateAccount(account.user, account.password, account.email, account.clientList);
		}
		else if (query == 23) { //for 11 - ...
			//if query need to send email: listApp, add clientID, remove clientID,...
			string content = account.adminID + ";" + to_string(query);
			myCurl.SendEmail(account.clientList, content);
		}
	}
	return GetRole();
}
void GetRole() {
	myCurl.isAutoReceiving = false;
	int roleIndex = -1;
	cout << "0: CLIENT_SERVER, 1: ADMIN" << endl;
	cout << "Role: ";
	cin >> roleIndex;
	if (roleIndex == 0) {
		account.SetRole(Role::CLIENT_SERVER);
		myCurl.UpdateSearchQuery(account.clientID);
		Client_ServerRun();
	}
	else if (roleIndex == 1) {
		account.SetRole(Role::ADMIN);
		//Login
		string userLogin = ""; 
		string passwordLogin = "";
		while (true) {
			cout << "User: "; cin >> userLogin;
			cout << "Password: ";  cin >> passwordLogin;
			if (loginSystem.SearchAccount(userLogin, passwordLogin, account)) {
				myCurl.UpdateSearchQuery(account.adminID);
				break;
			}
		}
		AdminRun();
	}
	else return; //close app;
}
int main() {
	InitWinsock(wsaData);

	//loginSystem.InsertAccount("user", "password", "gmail");
	//loginSystem.UpdateAccount("user", "password", "gmail", {"C1", "C2"});
	//loginSystem.SearchAccount("user", "password");
	//loginSystem.DeleteAccount("user");
	//cout << loginSystem.GetClientID();

	//First time using
	if (account.clientID.empty()) {
		int clientID = loginSystem.GetClientID();
		account.SetClientID("C" + to_string(clientID));
	}
	GetRole();

	CloseWinsock();
}

#ifndef COMMENT
WSADATA wsaData;

int main() {
	InitWinsock(wsaData);

	ServerSocket server;
	server.Listen();
	const char* welcomeMessage = "Hello, Client!";
	while (true) {
		server.Receive();
		char input = 'o';
		cin >> input;
		cout << input;
		if (input == ' ') break;
		if (input == 's') {
			server.Send(welcomeMessage);
		}
	}

	CloseWinsock();
}
#endif 

#ifndef COMMENT
//int main() {
//	string content = "Hello, this is a test email!";
//	vector<string> clientIDs = { "C1", "C2" };
//	string id = "A1";
//	MyCurl myCurl;
//	myCurl.UpdateSearchQuery(id);
//	for (auto& id : clientIDs) {
//		myCurl.AddClientId(id);
//	}
//	myCurl.SendEmail(content);
//	while (true) {
//		myCurl.ReadEmail();
//	}
//}
#endif


