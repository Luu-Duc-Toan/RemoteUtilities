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

filesystem::file_time_type modifiedTime;

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
				if (myCurl.query ==  "12" || myCurl.query == "20" || myCurl.query == "21") {
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
	int query = -1;
	vector<string> selectedClient;
	string UIContent = "";
	while (true) {
		query = -1;
		if (filesystem::last_write_time(UIPath) != modifiedTime) {
			this_thread::sleep_for(std::chrono::duration<double>(0.1));
			modifiedTime = filesystem::last_write_time(UIPath);
			cout << "Query: ";
			string queryStr, selectedSize;
			fstream f(UIPath, ios::in);
			getline(f, queryStr, ';');
			query = stoi(queryStr);
			getline(f, selectedSize, ';');
			int n = stoi(selectedSize);
			string clientID;
			for (int i = 0; i < n; i++) {
				getline(f, clientID, ';');
				selectedClient.push_back(clientID);
			}
			getline(f, UIContent, ';');
			f.close();
		}
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
		else if (query == 2) { //Change email
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
		else if (query == 3) { //Add clientID
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
		else if (query == 4) { //Remove clientID
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
		else if (query == 5) { //Log out
			myCurl.isAutoReceiving = false;
			account.Reset();
			break;
		}
		else if (query == 6) { //Select client

		}
		//6,7
		else if (query > 10) { //for 11 - ...
			//if query need to send email: listApp, add clientID, remove clientID,...
			myCurl.result = UIContent;
			string content = account.adminID + ";" + to_string(query) + ";" + myCurl.result + ";";
			//Choose clientID
			myCurl.SendEmail(selectedClient, content);
			myCurl.AdminProcess(selectedClient, query);
			fstream file(SystemPath, ios::out);
			file << to_string(query) + ";Y;";
		}
	}
	return GetRole();
}
void Login() {
	auto NewModifiedTime = filesystem::last_write_time(UIPath);
	while (true) {
		auto NewModifiedTime = filesystem::last_write_time(UIPath);
		if (NewModifiedTime == modifiedTime) {
			modifiedTime = NewModifiedTime;
			continue;
		}
		this_thread::sleep_for(std::chrono::duration<double>(0.1));
		string mode, user, password;
		fstream f(UIPath, ios::in);
		getline(f, mode, ';');
		if (mode == "1") { //Back
			cout << "Back!" << endl;
			f.close();
			return GetRole();
		}
		else if (mode == "3") { //Sign up
			f.close();
			modifiedTime = filesystem::last_write_time(UIPath);
			while (true) {
				NewModifiedTime = filesystem::last_write_time(UIPath);
				if (NewModifiedTime != modifiedTime) {
					this_thread::sleep_for(std::chrono::duration<double>(0.1));
					modifiedTime = filesystem::last_write_time(UIPath);
					f.open(UIPath, ios::in);
					string mode;
					getline(f, mode, ';');
					if (mode == "1") {
						cout << "Back!" << endl;
						f.close();
						return Login();
					}
					string user, password, email;
					getline(f, user, ';');
					getline(f, password, ';');
					getline(f, email, ';');
					f.close();
					if (loginSystem.isAccountExist(user)) {
						f.open(SystemPath, ios::out);
						f << "N;Account already exist!;";
						f.close();
						cout << "Account already exist!" << endl;
						continue;
					}
					email += "@gmail.com";
					int code = rand() % 1000000;
					string content = "Validation code: " + to_string(code);
					cout << "Code: " << code << endl;
					confirmationCurl.Send(email, content);
					f.open(SystemPath, ios::out);
					f << "Y;" + to_string(code) + ';';
					f.close();
					while (true) {
						NewModifiedTime = filesystem::last_write_time(UIPath);
						if (NewModifiedTime != modifiedTime) {
							this_thread::sleep_for(std::chrono::duration<double>(0.1));
							modifiedTime = filesystem::last_write_time(UIPath);
							fstream f(UIPath, ios::in);
							string result;
							getline(f, result, ';');
							f.close();
							if (result == "Y") {
								loginSystem.InsertAccount(user, password, email);
								cout << "Sign up successfully!" << endl;
								return Login();
							}
							else {
								cout << "Sign up failed!" << endl;
							}
						}
					}
				}
			}
		}
		else {
			getline(f, user, ';');
			getline(f, password, ';');
			f.close();
			if (loginSystem.SearchAccount(user, password, account)) {
				string res = "Y;";
				res += to_string(account.clientList.size()) + ";";
				for (string &clientID : account.clientList) {
					res += clientID + ";";
				}
				f.open(SystemPath, ios::out);
				f << res;
				f.close();
				account.SetRole(Role::ADMIN);
				myCurl.UpdateSearchQuery(account.adminID);
				modifiedTime = filesystem::last_write_time(UIPath);
				return AdminRun();
			}
			else {
				f.open(SystemPath, ios::out);
				f << "N;Wrong password;";
				modifiedTime = filesystem::last_write_time(UIPath);
			}
		}
	}
}
void GetRole() {
	myCurl.isAutoReceiving = false;
	cout << "0: CLIENT_SERVER, 1: ADMIN" << endl;
	modifiedTime = filesystem::last_write_time(UIPath);
	while (true) {
		if (filesystem::last_write_time(UIPath) != modifiedTime) {
			this_thread::sleep_for(std::chrono::duration<double>(0.1));
			modifiedTime = filesystem::last_write_time(UIPath);
			cout << modifiedTime << endl;
			fstream f(UIPath, ios::in);
			string userInput;
			getline(f, userInput, ';');
			f.close();
			if (userInput == "1") { //Client - Server
				account.SetRole(Role::CLIENT_SERVER);
				myCurl.UpdateSearchQuery(account.clientID);
				Client_ServerRun();
				cout << "Client - Server!" << endl;
			}
			else if (userInput == "2") { //Admin
				cout << "Admin!" << endl;
				return Login();
			}
			else {
				break;
			}
		}
	}
}


int main() {
	srand((unsigned)time(0));
	InitWinsock(wsaData);
	//First time using
	if (account.clientID.empty()) {
		int clientID = loginSystem.GetMaxClientID();
		loginSystem.UpdateMaxClientID();
		account.SetClientID("C" + to_string(clientID));
		cout << "Your clientID: " << account.clientID << endl;
	}
	//UIPath
	ComPath = filesystem::current_path().string();
	for (char& c : ComPath) {
		c = (c == '\\') ? '/' : c;
	}
	while (ComPath.back() != '/') {
		ComPath.pop_back();
	}
	ComPath += "Common/";
	UIPath = ComPath + "UI.txt";
	SystemPath = ComPath + "System.txt";
	modifiedTime = filesystem::last_write_time(UIPath);
	fstream f(SystemPath, ios::out);
	f << "Y;";
	f.close();
	//Main loop
	GetRole();
	isAppOn = false;
	CloseWinsock();
}

