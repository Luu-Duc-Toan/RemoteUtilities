#include "GmailAPI.h"

string imapsURL = "imaps://imap.gmail.com:993/RemoteUltilities";
string smtpURL = "smtp://smtp.gmail.com:587";
string email_payload_text = "";
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	((string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}
void ExtractEmailBody(string& buffer) {
	size_t bodyStart = buffer.find("\r\n\r\n");
	size_t bodyEnd = buffer.find("\r\n\r\n", bodyStart + 4);
	buffer = buffer.substr(bodyStart + 4, bodyEnd - bodyStart);
}

void MyCurl::CreateEmail(const string id, const string content) {
	email_payload_text =
		"Subject:" + id + "\r\n"
		"\r\n"
		+ content + "\r\n";
}
void MyCurl::ConfirmEmail(string& recipent, string &content) {
	email_payload_text =
		"Subject: Confirm email\r\n"
		"\r\n"
		+ content + "\r\n";
	curl_easy_setopt(sender, CURLOPT_MAIL_RCPT, recipent);
	res = curl_easy_perform(sender);
	// Check for errors
	if (res != CURLE_OK) {
		cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
	}
	else {
		cout << "Email sent successfully!" << endl;
	}
	curl_easy_setopt(sender, CURLOPT_MAIL_RCPT, recipients);
}

void MyCurl::InitSender() {
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
	curl_easy_setopt(receiver, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
	curl_easy_setopt(receiver, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(receiver, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(receiver, CURLOPT_WRITEDATA, &receiveBuffer);
}
void MyCurl::CleanSession(CURL*& session, string& buffer) {
	buffer.clear();
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
		CreateEmail(id, content);
		res = curl_easy_perform(sender);
		// Check for errors
		if (res != CURLE_OK) {
			cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
		}
		else {
			cout << "Email sent successfully!" << endl;
		}
	}
}
void MyCurl::ReadEmail(bool &isAppOn) {
	if (!isAutoReceiving) return;
	while (isAppOn) {
		if (searchQuery.empty()) return;
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
			cout << "Email Body " << UID << ": " << receiveBuffer << endl;
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
	searchQuery = "UID SEARCH UNSEEN" + id;
}
void MyCurl::ClientProcess() {
	string emailContent = emailQueue.front();
	int start = 0;
	int end = emailContent.find(';', start);
	receiverID = emailContent.substr(start, end - start);
	start = end + 1;
	end = emailContent.find(';', start);
	query = emailContent.substr(start, end - start);
	start = end + 1;
	end = emailContent.find(';', start);
	subContent = emailContent.substr(end + 1);
}
void MyCurl::AdminProcess() {
	string emailContent = emailQueue.front();
	int start = 0;
	int end = emailContent.find(';', start);
	receiverID = emailContent.substr(start, end - start);
	start = end + 1;
	end = emailContent.find(';', start);
	query = emailContent.substr(start, end - start);
	start = end + 1;
	end = emailContent.find(';', start);
	subContent = emailContent.substr(end + 1);
}
bool MyCurl::ShouldSendToServer() {
	int queryInt = stoi(query);
	if (queryInt == 3) {
		cout << "Accept control from " << receiverID << "? (y/n): ";
		while (true) {
			if (_kbhit()) {
				char ch = _getch();
				if (ch == 'y') {
					result = 'Y';
					break;
				}
				else if (ch == 'n') {
					result = 'N';
					break;
				}
			}
		}
		return false;
	}
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