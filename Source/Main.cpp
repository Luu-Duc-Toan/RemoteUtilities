#include "Header.h"
#define COMMENT

bool isAppOn = true;
MyCurl myCurl(isAppOn);
LoginSystem loginSystem;
Account account;
WSADATA wsaData;
ClientSocket client;
ServerSocket server;
ConfirmationCurl confirmationCurl;
void GetRole();
void ClientRun(Role& role);
void ServerRun(Role& role);
void AdminRun();
void Client_ServerRun();

void ClientRun(Role& role) {
	client.TurnOn();
	client.ConnectToServer();
	while (role == Role::CLIENT_SERVER) {
		if (!myCurl.emailQueue.empty()) {
			myCurl.Preprocess();
			myCurl.emailQueue.pop();
			if (myCurl.ShouldSendToServer()) {
				client.Send(myCurl.query.c_str());
				if (myCurl.query == "20" || myCurl.query == "21") {
					client.Send(myCurl.subContent.c_str());
				}
				client.Receive();
			}
			myCurl.ClientProcess();
			string content = account.clientID + ";" + myCurl.query + ";" + myCurl.result + client.result + ";";
			cout << "Send email to " << myCurl.receiverID << endl;
			myCurl.SendEmail({ myCurl.receiverID }, content);
		}
	}
}
void ServerRun(Role& role) {
	server.TurnOn();
	server.Listen();
	while (role == Role::CLIENT_SERVER) {
		server.WaitQuery();
	}
}
void Client_ServerRun() {
	myCurl.isAutoReceiving = true;
	thread serverThread(ServerRun, ref(account.role));
	thread clientThread(ClientRun, ref(account.role));
	cout << "S to break: ";
	while (true) {
		char ch;
		cin >> ch; //Not this
		if (ch == 's') {
			myCurl.isAutoReceiving = false;
			account.role = Role::NONE;
			break;
		}
	}
	client.Reset();
	server.Reset();
	clientThread.join();
	serverThread.join();
	return GetRole();
}
void AdminRun() {
	myCurl.isAutoReceiving = true;
	while (true) {
		int query = -1;
		cout << "Query: ";
		cin >> query;
		if (query == -1) continue;
		else if (query == 1) { //for 1 - 10: dont need to send email: change password, email
			string password, newPassword;
			while (true) {
				cout << "Password: "; cin >> password;
				cout << "New password: "; cin >> newPassword;
				if (password != account.password)
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
				bool exitLoop = false;
				if (!account.email.empty()) {
					while (true) {
						cout << "Email: "; cin >> email;
						if (email != account.email)
							cout << "Wrong email!" << endl;
						else break;
					}
				}
				cout << "New email: "; cin >> newEmail;
				int code = rand() % 1000000;
				cout << "Code: " << code << endl;
				string content = "Validation code: " + to_string(code);
				confirmationCurl.Send(newEmail, content);
				int inputCode;
				while (true) {
					cin >> inputCode;
					if (inputCode == code) {
						cout << "Change email successfully!" << endl;
						exitLoop = true;
						break;
					}
					else {
						cout << "Wrong code!" << endl;
					}
				}
				if (exitLoop) break;
			}
			account.SetEmail(newEmail);
			loginSystem.UpdateAccount(account.user, account.password, account.email, account.clientList);
		}
		else if (query == 3) { //Cho xac nhan?
			string newClientID;
			while (true) {
				cout << "New clientID: "; cin >> newClientID;
				bool validNewClientID = true;
				int newClientIDNum = ExtractIDNum(newClientID);
				int currentClientIDNum = ExtractIDNum(account.clientID);
				if (newClientIDNum > loginSystem.GetMaxClientID()) {
					cout << "ClientID not exist!" << endl;
					validNewClientID = false;
				}
				else if (newClientIDNum == currentClientIDNum) {
					cout << "Can't add this pc's clientID" << endl;
					validNewClientID = false;
				}
				else {
					for (string& clientID : account.clientList) {
						int clientIDNum = ExtractIDNum(clientID);
						if (newClientIDNum == clientIDNum) {
							cout << "ClientID already exist!" << endl;
							validNewClientID = false;
						}
					}
				}
				if (validNewClientID) break;
			}
			string content = account.adminID + ";" + to_string(query) + ";";
			myCurl.SendEmail({ newClientID }, content);
			stack<string> emailStack;
			auto start = chrono::high_resolution_clock::now();
			while (true) {
				auto end = chrono::high_resolution_clock::now();
				auto duration = chrono::duration_cast<chrono::seconds>(end - start);
				if (duration.count() >= 600) {
					cout << "Time out!" << endl;
					cout << "Resend email? (send again/wait/exit): " << endl;
					char input = 'o';
					cin >> input;
					if (input == 's') {
						myCurl.SendEmail({ newClientID }, content);
						start = chrono::high_resolution_clock::now();
					}
					else if (input == 'w') {
						start = chrono::high_resolution_clock::now();
					}
					else {
						break;
					}
				}
				if (myCurl.emailQueue.empty()) continue;
				else {
					myCurl.Preprocess();
					if (myCurl.receiverID == newClientID) {
						if (myCurl.query == "3") {
							if (myCurl.subContent == "Y") {
								cout << "Add new clientID" << newClientID << endl;
								account.AddClientID(newClientID);
								loginSystem.UpdateAccount(account.user, account.password, account.email, account.clientList);
							}
							else {
								cout << "Receive from " << newClientID << " " << myCurl.subContent << endl;
								cout << "Don't add new clientID" << newClientID << endl;
							}
							while (!emailStack.empty()) {
								myCurl.emailQueue.push(emailStack.top());
								emailStack.pop();
							}
							break;
						}
					}
					else {
						emailStack.push(myCurl.emailQueue.front());
						myCurl.emailQueue.pop();
					}
				}
			}
		}
		else if (query == 4) {
			string removeClientID;
			cout << "Remove clientID: ";
			while (true) {
				cin >> removeClientID;
				if (account.RemoveClientID(removeClientID)) {
					loginSystem.UpdateAccount(account.user, account.password, account.email, account.clientList);
				}
				else {
					cout << "ClientID not exist!" << endl;
					cout << "Remove clientID: ";
				}
			}
		}
		else if (query == 5) {
			myCurl.isAutoReceiving = false;
			account.Reset();
			break;
		}
		else if (query > 10) { //for 11 - ...
			//if query need to send email: listApp, add clientID, remove clientID,...
			myCurl.result = "";
			if (query == 20 || query == 21) {
				cout << "Nhap duong dan file: ";
				string filePath;
				cin >> myCurl.result;
			}
			string content = account.adminID + ";" + to_string(query) + ";" + myCurl.result + ";";
			//Choose clientID
			myCurl.SendEmail(account.clientList, content);
			myCurl.AdminProcess(account.clientList, query);
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
	srand((unsigned)time(0));
	InitWinsock(wsaData);

	//loginSystem.InsertAccount("user", "password", "gmail");
	//loginSystem.UpdateAccount("user", "password", "gmail", {"C1", "C2"});
	//loginSystem.SearchAccount("user", "password");
	//loginSystem.DeleteAccount("user");
	//cout << loginSystem.GetClientID();

	//First time using
	if (account.clientID.empty()) {
		int clientID = loginSystem.GetMaxClientID();
		loginSystem.UpdateMaxClientID();
		account.SetClientID("C" + to_string(clientID));
		cout << "Your clientID: " << account.clientID << endl;
	}
	GetRole();
	isAppOn = false;
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