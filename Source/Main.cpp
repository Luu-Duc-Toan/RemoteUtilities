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
void Login();

void ClientRun(Role& role) {
	client.TurnOn();
	client.ConnectToServer();
	while (role == Role::CLIENT_SERVER) {
		if (!myCurl.emailQueue.empty()) {
			myCurl.Preprocess();
			fstream f(SystemPath, ios::out);
			if(myCurl.query == "3") f << myCurl.query + ";" + myCurl.receiverID + ';';
			else f << myCurl.query + ";";
			f.close();
			myCurl.emailQueue.pop();
			if (myCurl.ShouldSendToServer()) {
				client.Send(myCurl.query.c_str());
				if (myCurl.query == "12" || myCurl.query == "13" || myCurl.query == "20" || myCurl.query == "21") {
					client.Send(myCurl.subContent.c_str());
				}
				client.Receive();
			}
			myCurl.ClientProcess();
			string content = account.clientID + ";" + myCurl.query + ";" + myCurl.result + client.result + ";";
			cout << "Send email to " << myCurl.receiverID << endl;
			myCurl.SendEmail({ myCurl.receiverID }, content);
			f.open(SystemPath, ios::out);
			f << myCurl.query + "Y;";
			f.close();
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
	modifiedTime = filesystem::last_write_time(UIPath);
	myCurl.isAutoReceiving = true;
	thread serverThread(ServerRun, ref(account.role));
	thread clientThread(ClientRun, ref(account.role));
	cout << "S to break: ";
	while (true) {
		if (filesystem::last_write_time(UIPath) != modifiedTime) {
			this_thread::sleep_for(std::chrono::duration<double>(0.1));
			string res;
			fstream f(UIPath, ios::in);
			getline(f, res, ';');
			if (res == "B") {
				myCurl.isAutoReceiving = false;
				account.role = Role::NONE;
				modifiedTime = filesystem::last_write_time(UIPath);
				break;
			}
		}
	}
	client.Reset();
	server.Reset();
	clientThread.join();
	serverThread.join();
	return GetRole();
}
void ForgotPassword() {
	while (true) {
		if (filesystem::last_write_time(UIPath) != modifiedTime) {
			this_thread::sleep_for(std::chrono::duration<double>(0.1));
			modifiedTime = filesystem::last_write_time(UIPath);
			fstream f(UIPath, ios::in);
			string mode, user, email;
			getline(f, mode, ';');
			if (mode == "1") {
				cout << "Back!" << endl;
				f.close();
				return Login();
			}
			else if (mode == "2") {
				getline(f, user, ';');
				getline(f, email, ';');
				f.close();
				if (loginSystem.isAccountExist(user)) {
					if (loginSystem.isMatchGmail(user, email)) {
						int code = rand() % 1000000;
						string content = "Validation code: " + to_string(code);
						cout << "Code: " << code << endl;
						confirmationCurl.Send(email, content);
						f.open(SystemPath, ios::out);
						f << "Y;" + to_string(code) + ';';
						f.close();
						while (true) {
							if (filesystem::last_write_time(UIPath) != modifiedTime) {
								this_thread::sleep_for(std::chrono::duration<double>(0.1));
								modifiedTime = filesystem::last_write_time(UIPath);
								fstream f(UIPath, ios::in);
								string result;
								getline(f, result, ';');
								f.close();
								if (result == "Y") {
									while (true) {
										if (filesystem::last_write_time(UIPath) != modifiedTime) { //Set password
											this_thread::sleep_for(std::chrono::duration<double>(0.1));
											modifiedTime = filesystem::last_write_time(UIPath);
											f.open(UIPath, ios::in);
											string mode, newPassword;
											getline(f, mode, ';');
											if (mode == "1") {
												cout << "Back!" << endl;
												f.close();
												return Login();
											}
											else if (mode == "2") {
												getline(f, newPassword, ';');
												f.close();
												loginSystem.UpdatePassword(user, newPassword);
												cout << "Set password successfully!" << endl;
												fstream file(SystemPath, ios::out);
												file << "Y;";
												file.close();
												return Login();
											}
										}
									}
								}
								else {
									cout << "Back!" << endl;
									return Login();
								}
							}
						}
					}
					else {
						f.open(SystemPath, ios::out);
						f << "N;Email not match;"; //N;Email not match;
						f.close();
						cout << "Email not match!" << endl;
					}
				}
				else {
					f.open(SystemPath, ios::out);
					f << "N;Account not exist;"; //N;Account not exist;
					f.close();
					cout << "Account not exist!" << endl;
				}
			}
		}
	}
}
void Login() {
	auto modifiedTime = filesystem::last_write_time(UIPath);
	while (true) {
		if (filesystem::last_write_time(UIPath) != modifiedTime) {
			this_thread::sleep_for(std::chrono::duration<double>(0.1));
			modifiedTime = filesystem::last_write_time(UIPath);
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
				while (true) {
					if (filesystem::last_write_time(UIPath) != modifiedTime) {
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
							if (filesystem::last_write_time(UIPath) != modifiedTime) {
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
			else if (mode == "2") { //Login
				getline(f, user, ';');
				getline(f, password, ';');
				f.close();
				if (loginSystem.SearchAccount(user, password, account)) {
					string res = "Y;" + account.adminID + ';';
					res += to_string(account.clientList.size()) + ";";
					for (string& clientID : account.clientList) {
						res += clientID + ";";
					}
					f.open(SystemPath, ios::out);
					f << res;
					f.close();
					account.SetRole(Role::ADMIN);
					myCurl.UpdateSearchQuery(account.adminID);
					return AdminRun();
				}
				else {
					f.open(SystemPath, ios::out);
					f << "N;Wrong password;";
					f.close();
				}
			}
			else {
				f.close();
				return ForgotPassword();
			}
		}
	}
}
void AdminRun() {
	modifiedTime = filesystem::last_write_time(UIPath);
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
			int end = UIContent.find('#');
			password = UIContent.substr(0, end);
			newPassword = UIContent.substr(end + 1);
			cout << "Change password: " << password << " -> " << newPassword << endl;
			if (password == account.password) {
				cout << "Change password successfully!" << endl;
				fstream file(SystemPath, ios::out);
				file << "1;Y;";
				file.close();
				account.SetPassword(newPassword);
				loginSystem.UpdateAccount(account.user, account.password, account.email, account.clientList);
			}
			else {
				cout << "Change password failed!" << endl;
				fstream file(SystemPath, ios::out);
				file << "1;N;Wrong password!;";
				file.close();
			}
		}
		else if (query == 3) { //Add clientID
			string newClientID = UIContent;
			cout << "Add clientID: " << newClientID;
			bool validNewClientID = true;
			int newClientIDNum = ExtractIDNum(newClientID);
			int currentClientIDNum = ExtractIDNum(account.clientID);
			if (newClientIDNum > loginSystem.GetMaxClientID()) {
				fstream file(SystemPath, ios::out);
				file << "3;N;ClientID not exist!;";
				file.close();
				validNewClientID = false;
			}
			else if (newClientIDNum == currentClientIDNum) {
				fstream file(SystemPath, ios::out);
				file << "3;N;Can't add yourself;";
				file.close();
				validNewClientID = false;
			}
			else {
				for (string& clientID : account.clientList) {
					int clientIDNum = ExtractIDNum(clientID);
					if (newClientIDNum == clientIDNum) {
						fstream file(SystemPath, ios::out);
						file << "3;N;ClientID already exist!;";
						file.close();
						validNewClientID = false;
					}
				}
			}
			if (validNewClientID) {
				fstream file(SystemPath, ios::out);
				file << "3;Y;;";
				file.close();
				string content = account.adminID + ";" + to_string(query) + ";";
				myCurl.SendEmail({ newClientID }, content);
				stack<string> emailStack;
				while (true) {
					if (myCurl.emailQueue.empty()) continue;
					else {
						myCurl.Preprocess();
						if (myCurl.receiverID == newClientID) {
							if (myCurl.query == "3") {
								if (myCurl.subContent == "Y") {
									fstream file(UIPath, ios::out);
									file << "Y;";
									file.close();
									account.AddClientID(newClientID);
									loginSystem.UpdateAccount(account.user, account.password, account.email, account.clientList);
								}
								else {
									fstream file(UIPath, ios::out);
									file << "N;";
									file.close();
									cout << "Reject from clientID: " << newClientID << endl;
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
		}
		else if (query == 4) { //Remove clientID
			string removeClientID = UIContent;
			cout << "Remove clientID: " << removeClientID;
			if (account.RemoveClientID(removeClientID)) {
				loginSystem.UpdateAccount(account.user, account.password, account.email, account.clientList);
				fstream file(SystemPath, ios::out);
				file << "4;Y;" + removeClientID + ';';
				file.close();
				cout << "Remove clientID successfully!" << endl;
			}
			else {
				fstream file(SystemPath, ios::out);
				file << "4;N;Not be in your client list";
				file.close();
				cout << "Remove clientID failed!" << endl;
			}
		}
		else if (query == 5) { //Log out
			myCurl.isAutoReceiving = false;
			account.Reset();
			fstream file(SystemPath, ios::out);
			file << "5;;";
			file.close();
			return Login();
		}
		//6,7
		else if (query > 10) { //for 11 - ...
			//if query need to send email: listApp, add clientID, remove clientID,...
			vector<string> failedClientIDs;
			myCurl.result = UIContent;
			string content = account.adminID + ";" + to_string(query) + ";" + myCurl.result + ";";
			//Choose clientID
			myCurl.SendEmail(selectedClient, content);
			myCurl.AdminProcess(selectedClient, query, failedClientIDs);
			fstream file(SystemPath, ios::out);
			if (failedClientIDs.size() != 0) {
				string res = to_string(query) + ";N;" + to_string(failedClientIDs.size()) +';';
				for (string failedClientID : failedClientIDs) {
					res += failedClientID + ';';
				}
				file << res;
			}
			else 
				file << to_string(query) + ";Y;";
			file.close();
		}
	}
	return GetRole();
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
				cout << "Client - Server! ID " + account.clientID << endl;
				f.open(SystemPath, ios::out);
				f << "1;" + account.clientID;
				f.close();
				return Client_ServerRun();
			}
			else if (userInput == "2") { //Admin
				cout << "Admin!" << endl;
				f.open(SystemPath, ios::out);
				f << "2;";
				f.close();
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

