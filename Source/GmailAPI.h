#pragma once
#ifndef GMAILAPI_H
#define GMAILAPI_H
#include "Header.h"
#include <curl/curl.h>

extern string imapsURL;
extern string smtpURL;
extern string email_payload_text;
extern string confirmation_payload_text;

struct upload_status {
	int lines_read;
};

static size_t payload_source(void* ptr, size_t size, size_t nmemb, void* userp) {
	upload_status* upload_ctx = (upload_status*)userp;
	const char* data;

	if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1)) {
		return 0;
	}
	data = &email_payload_text[upload_ctx->lines_read];
	if (data) {
		size_t len = strlen(data);
		memcpy(ptr, data, len);
		upload_ctx->lines_read += len;
		return len;
	}
	return 0;
}
static size_t payload_source_confirmation(void* ptr, size_t size, size_t nmemb, void* userp) {
	upload_status* upload_ctx = (upload_status*)userp;
	const char* data;

	if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1)) {
		return 0;
	}
	data = &confirmation_payload_text[upload_ctx->lines_read];
	if (data) {
		size_t len = strlen(data);
		memcpy(ptr, data, len);
		upload_ctx->lines_read += len;
		return len;
	}
	return 0;
}
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
void ExtractEmailBody(string& buffer);
struct MyCurl {
	bool isAutoReceiving;
	Role role;
	thread autoReceivingThread;
	CURLcode res = CURLE_OK;
	string query;
	string subContent;
	string result;
	//Sender
	string receiverID;
	string serverAccount;
	string serverPassword;
	string clientAccount;
	string clientPassword;
	CURL* sender = nullptr;
	curl_slist* recipients = nullptr;
	upload_status upload_ctx = { 0 };
	//Reader
	CURL* receiver = nullptr;
	vector<string> clientIDs;
	vector<int> UIDs;
	string searchQuery;
	string receiveBuffer;
	queue<string> emailQueue;
	stack<string> emailStack;
	
	void CreateEmail(const string id, const string content);
	void InitSender();
	void InitReceiverSession(string& URL);
	void CleanSession(CURL*& session, string& buffer);
	void UpdateSearchQuery(string clientIDs);
	vector<int> GetListUIDUnseen();
	void SendEmail(const vector<string> IDs, const string content);
	void Preprocess();
	void ClientProcess();
	bool ShouldSendToServer();
	void AdminProcess();
	void ReadEmail(bool& isAppOn);
	void ConfirmEmail(string& recipent, string &content);
	MyCurl(bool& isAppOn);
	~MyCurl();
};
struct ConfirmationCurl {
	CURL* sender = nullptr;
	curl_slist* recipients = nullptr;
	upload_status upload_ctx = { 0 };
	string account = "remoteutilitiescofirmation@gmail.com";
	string password = "rejg nmof kmsy syfy";
	void InitSender(string &email) {
		//Set the SMTP server URL
		sender = curl_easy_init();
		curl_easy_setopt(sender, CURLOPT_URL, smtpURL.c_str());
		// Enable TLS for secure connection
		curl_easy_setopt(sender, CURLOPT_USE_SSL, CURLUSESSL_ALL);
		//Set the login credentials
		curl_easy_setopt(sender, CURLOPT_USERNAME, account.c_str());
		curl_easy_setopt(sender, CURLOPT_PASSWORD, password.c_str());
		//Set the sender email address
		curl_easy_setopt(sender, CURLOPT_MAIL_FROM, account.c_str());
		//Add recipient(s)
		recipients = curl_slist_append(recipients, email.c_str());
		curl_easy_setopt(sender, CURLOPT_MAIL_RCPT, recipients);
		// Specify the payload
		curl_easy_setopt(sender, CURLOPT_READFUNCTION, payload_source_confirmation);
		curl_easy_setopt(sender, CURLOPT_READDATA, &upload_ctx);
		curl_easy_setopt(sender, CURLOPT_UPLOAD, 1L);
	}
	void Send(string& email, string& content) {
		InitSender(email);
		confirmation_payload_text =
			"Subject: Confirm email\r\n"
			"\r\n"
			+ content + "\r\n";
		CURLcode res = curl_easy_perform(sender);
		// Check for errors
		if (res != CURLE_OK) {
			cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
		}
		else {
			cout << "Email sent successfully!" << endl;
		}
		if (recipients) {
			curl_slist_free_all(recipients);
			recipients = nullptr;
		}
		if (sender) {
			curl_easy_cleanup(sender);
			sender = nullptr;
		}
	}
};

#endif