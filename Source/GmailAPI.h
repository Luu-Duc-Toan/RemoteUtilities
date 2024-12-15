#pragma once
#ifndef GMAILAPI_H
#define GMAILAPI_H
#include "Header.h"
#include <curl/curl.h>

extern string imapsURL;
extern string smtpURL;
extern string email_payload_text;

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
	void ClientProcess();
	bool ShouldSendToServer();
	void AdminProcess();
	void ReadEmail();
	MyCurl();
	~MyCurl();
};
#endif