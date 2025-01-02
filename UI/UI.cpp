#include "UI.h"
int SCREENWIDTH = 1500;
int SCREENHEIGHT = 900;

bool showCursor = true;
int fontSize = 40;
int queryFontSize = 25;
float cursorTimer = 0.0f;
Color background = { 244, 237, 211, 255 };
Color selectedColor = { 255, 102, 0, 255 };

int clientMagin = 15;
int maxClientInRow = 5;
int clientPage = 1;
int maxClientPage = (clientList.size() + maxClientInRow * 3 - 1) / (maxClientInRow * 3);
vector<Texture2D> textures;
////////////////////////////////////////////////////////////////////////// LOG IN
string user = "user";
string password = "pa";
string status = "";
bool isTypingUser = true;
bool isWaiting = false;
////////////////////////////////////////////////////////////////////////// SIGN UP
string gmail = "a";
bool isTypingGmail = false;
////////////////////////////////////////////////////////////////////////// Code
string code = "";
string codeInput = "";
////////////////////////////////////////////////////////////////////////// CLIENT
string clientQuery = "";
string yourClientID = "";
string receiverID = "";
////////////////////////////////////////////////////////////////////////// ADMIN
string yourAdminID = "";
bool showUserBox = false;
vector<string> adminMainQueryNames = { "Apps", "Processes", "Shutdown", "Reset", "Copy", "Delete", "Screenshot", "KeyloggerON", "KeyloggerOFF","WebcamON","WebcamOFF" };
vector<string> userQueryNames = { "Change Password", "Log Out" };
vector<string> userQueryNumbers = { "1", "5" };
vector<string> clientList = { "C35", "C231", "C23", "C123", "C1234", "C231", "C23", "C123", "C1234", "C231", "C23", "C123", "C1234", "C231", "C23", "C123", "C123", "C1234", "C231", "C23", "C12321412" };
vector<bool> clientSelected(clientList.size(), true);
vector<string> adminMainQueryNumbers = { "11", "14", "17", "18", "20", "21", "22", "23", "24", "26", "27" };
int clientSelectedCount = clientList.size();
bool isGettingFilePath = false;
string CopyOrDelete = "";
string filePath = "";
bool hasFailQuery = false;
bool isShowSuccessNotification = false;
int successNotificationQuery;
bool isGettingAppName = false;
string StartOrStop = "";
string appName = "";
bool isGettingClientID = false;
string AddOrRemove = "";
string newClientID = "";
bool isWaitingNewClientIDAccept = false;
bool isShowFailNotification = false;
bool isGettingNewPassword = false;
string currentPassword = "";
string newPassword = "";
bool isTypingNewPassword = false;
string failNoti = "";
vector<string> failClientList = {};
////////////////////////////////////////////////////////////////////////// FORGOT PASSWORD
bool isTypingConfirmPassword = false;
string confirmPassword = "";
////////////////////////////////////////////////////////////////////////// LIST APP
vector<pair<string, string>> apps = {};
int maxAppInPage = 10;
int appPage = 1;
int maxAppPage = (apps.size() + maxAppInPage - 1) / maxAppInPage;
int startAppY = 210;
int startAppX = 150;
int appMargin = 10;
/////////////////////////////////////////////////////////////////////////////////// Animation
vector<float> frameTimes = { 0.04f, 0.018f, 0.03f };
vector<int> totalFrames = { 60, 150, 35 };
vector<int> columns = { 5, 5, 5 };
vector<int> currentFrames = { 0, 0, 0 };
vector<float> frameWidths = { 300, 400, 300 };
vector<float> frameHeights = { 300, 400, 300 };
vector<float> timers = { 0.0f, 0.0f, 0.0f };
vector<Rectangle> sourceRects = { {0.0f, 0.0f, frameWidths[0], frameHeights[0]}, {0.0f, 0.0f, frameWidths[1], frameHeights[1]},
	{0.0f, 0.0f, frameWidths[2], frameHeights[2]} };
vector<Texture2D> animations;
///////////////////////////////////////////////////////////////////////////////////////////
function<void()> Draw = DrawStartWindow;
string userInput;
bool isUserInputChanged = false;
filesystem::file_time_type modifiedTime;
///////////////////////////////////////////////////////////////////////////////////////////
void DrawAnimation(int i, float x, float y) {
	timers[i] += GetFrameTime();
	if (timers[i] >= frameTimes[i]) {
		timers[i] = 0.0f;
		currentFrames[i] = (currentFrames[i] + 1) % totalFrames[i];
		sourceRects[i].x = currentFrames[i] % columns[i] * frameWidths[i];
		sourceRects[i].y = currentFrames[i] / columns[i] * frameHeights[i];
	}
	DrawTextureRec(animations[i], sourceRects[i], { x, y }, WHITE);
}
void DrawWaitingAnimation() {
	DrawRectangle(0, 0, SCREENWIDTH, SCREENHEIGHT, { 0, 0, 0, 200 });
	DrawAnimation(2, (SCREENWIDTH - frameWidths[2]) / 2, 300);
}
void ResetLogin() {
	user = "";
	password = "";
	status = "";
	isTypingUser = true;
}
void LoadList(const string mode) {
	string path = ComPath + "List/" + mode;
	unordered_map<string, pair<string, int>> listApp;
	for (int i = 0; i < clientList.size(); i++) {
		if (!clientSelected[i]) continue;
		fstream file(path + clientList[i], ios::binary | ios::in);
		string appName, appPath;
		while (!file.eof()) {
			getline(file, appName, ';');
			getline(file, appPath, ';');
			if (appName.empty() || appPath.empty()) continue;
			listApp[appName].second++;
			listApp[appName].first = appPath;
		}
		file.close();
	}
	apps.clear();
	for (auto& app : listApp) {
		if (app.second.second != clientSelectedCount) continue;
		apps.push_back({ app.first, app.second.first });
	}
	maxAppPage = (apps.size() + maxAppInPage - 1) / maxAppInPage;
}
void ResetSignUp() {
	user = "";
	password = "";
	gmail = "";
	status = "";
	isTypingUser = true;
	isTypingGmail = false;
}
void ResetConfirmation() {
	code = "";
	codeInput = "";
	status = "";
}
void ResetGetNewPassword() {
	currentPassword = "";
	newPassword = "";
	isTypingNewPassword = false;
	isGettingNewPassword = false;
}
void ResetForgotPassword() {
	user = "";
	gmail = "";
	code = "";
	codeInput = "";
	status = "";
	isTypingUser = true;
	isTypingGmail = false;
}
void ResetSetPassword() {
	newPassword = "";
	confirmPassword = "";
	status = "";
	isTypingNewPassword = false;
}
void SetClientList(fstream& f) {
	clientList.clear();
	clientSelected.clear();
	string size;
	getline(f, size, ';');
	int n = stoi(size);
	string clientID;
	for (int i = 0; i < n; i++) {
		getline(f, clientID, ';');
		clientList.push_back(clientID);
		clientSelected.push_back(true);
	}
	maxClientPage = (n + maxClientInRow * 3 - 1) / (maxClientInRow * 3);
	clientSelectedCount = n;
}
void DrawCursor(int x, int y) {
	cursorTimer += GetFrameTime();
	if (cursorTimer >= 0.5f) {
		showCursor = !showCursor;
		cursorTimer = 0.0f;
	}
	if (showCursor) {
		DrawText("|", x, y, fontSize, BLACK);
	}
}
void DrawStartWindow() {
	Vector2 mousePosition = GetMousePosition();
	Color boxSelectedColor = selectedColor; // { 74, 73, 71, 255 };
	int titleFontSize = 120;
	if (isWaiting) {
		mousePosition = { -1, -1 };
		if (filesystem::last_write_time(SystemPath) != modifiedTime) {
			this_thread::sleep_for(std::chrono::duration<double>(0.1));
			modifiedTime = filesystem::last_write_time(SystemPath);
			fstream f(SystemPath, ios::in);
			string mode, y;
			getline(f, mode, ';');
			if (mode == "1") {
				getline(f, yourClientID, ';');
				Draw = DrawServerClientWindow;
			}
			else if (mode == "2") {
				Draw = DrawLoginWindow;
			}
			isWaiting = false;
		}
	}
	//Title
	string title = "REMOTE UTILITIES";
	DrawText(title.c_str(), (SCREENWIDTH - MeasureText(title.c_str(), titleFontSize)) / 2, 70, titleFontSize, BLACK);
	title = "CHOOSE 1 MODE";
	DrawText(title.c_str(), (SCREENWIDTH - MeasureText(title.c_str(), fontSize)) / 2, 150 + titleFontSize, fontSize, BLACK);
	//Vẽ hộp Client
	Rectangle box = { 175, 380, 500, 450 }; //x, y, width, height
	string key = "SERVER - CLIENT";
	Color defaultBoxcolor = BEIGE;
	Color color = defaultBoxcolor;
	if (CheckCollisionPointRec(mousePosition, box)) {
		color = boxSelectedColor;
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			isUserInputChanged = true;
			userInput = "1;";
			isWaiting = true;
		}
	}
	DrawRectangleRec(box, color);
	DrawTexture(textures[0], box.x + (box.width - textures[0].width) / 2, box.y + 30, WHITE);
	DrawText(key.c_str(), box.x + (box.width - MeasureText(key.c_str(), fontSize)) / 2,
		box.y + (box.height + 30 + textures[0].height - fontSize) / 2, fontSize, BLACK);
	//Vẽ hộp Admin
	box.x += box.width + 150;
	key = "ADMIN";
	color = defaultBoxcolor;
	if (CheckCollisionPointRec(mousePosition, box)) {
		color = boxSelectedColor;
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			isUserInputChanged = true;
			userInput = "2;";
			isWaiting = true;
		}
	}
	DrawRectangleRec(box, color);
	DrawTexture(textures[1], box.x + (box.width - textures[0].width) / 2, box.y + 30, WHITE);
	DrawText(key.c_str(), box.x + (box.width - MeasureText(key.c_str(), fontSize)) / 2,
		box.y + (box.height + 30 + textures[0].height - fontSize) / 2, fontSize, BLACK);
	if(isWaiting)
		DrawWaitingAnimation();
}
void DrawLoginWindow() {
	Vector2 mousePosition = GetMousePosition();
	int input = GetKeyPressed();
	if (isWaiting) {
		mousePosition = { -1, -1 };
		input = 0;
		if (filesystem::last_write_time(SystemPath) != modifiedTime) {
			this_thread::sleep_for(std::chrono::duration<double>(0.1));
			fstream f(SystemPath, ios::in);
			string y;
			getline(f, y, ';');
			if (y == "Y") { 
				getline(f, yourAdminID, ';');
				SetClientList(f);
				Draw = DrawAdminWindow;
			}
			else if (y == "N") { //N;status;
				getline(f, status, ';');
			}
			isWaiting = false;
			f.close();
			modifiedTime = filesystem::last_write_time(SystemPath);
		}
	}
	if (input != 0) {
		if (input == KEY_BACKSPACE) {
			if (isTypingUser) {
				if (user.size() != 0) user.pop_back();
			}
			else
			{
				if (password.size() == 0) isTypingUser = true;
				else password.pop_back();
			}
		}
		else if (input == KEY_ENTER) {
			if (isTypingUser) isTypingUser = false;
			else {
				userInput = "2;" + user + ";" + password + ";";
				ResetLogin();
				isUserInputChanged = true;
				isWaiting = true;
				modifiedTime = filesystem::last_write_time(SystemPath);
			}
		}
		else if (input >= KEY_A && input <= KEY_Z) {
			if (isTypingUser) {
				user += input + 32;
			}
			else {
				password += input + 32;
			}
		}
		else {
			if (isTypingUser) {
				user += input;
			}
			else {
				password += input;
			}
		}
	}
	//Back button
	Rectangle backButton = { 0, 0, textures[2].width, textures[2].height };
	DrawTexture(textures[2], 0, 0, WHITE);
	if (CheckCollisionPointRec(mousePosition, backButton)) {
		DrawRectangleRec(backButton, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			userInput = "1;";
			isUserInputChanged = true;
			ResetLogin();
			Draw = DrawStartWindow;
		}
	}
	//Title
	int titleFontSize = 80;
	string title = "LOGIN";
	DrawText(title.c_str(), (SCREENWIDTH - MeasureText(title.c_str(), titleFontSize)) / 2, 100, titleFontSize, BLACK);
	//Vẽ hộp chữ
	Rectangle box = { 400, 250, 800, 100 }; //x, y, width, height
	DrawText("User", box.x - 120, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	DrawRectangleRec(box, WHITE);
	box.y += 150;
	DrawText("Password", box.x - 220, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	DrawRectangleRec(box, WHITE);
	//Vẽ chữ
	int cursorX = box.x + 10;
	int cursorY = box.y + (box.height - fontSize) / 2;
	if (isTypingUser) {
		cursorX += MeasureText(user.c_str(), fontSize);
		cursorY -= 150;
	}
	else {
		cursorX += MeasureText(password.c_str(), fontSize);
	}
	DrawCursor(cursorX, cursorY);
	DrawText(user.c_str(), box.x + 10, box.y + (box.height - fontSize) / 2 - 150, fontSize, BLACK);
	DrawText(password.c_str(), box.x + 10, box.y + (box.height - fontSize) / 2, fontSize, BLACK);

	//Log in button
	box.y += 150;
	box.height = 100;
	box.width = 200;
	box.x = 650;
	DrawRectangleRec(box, BEIGE);
	if (CheckCollisionPointRec(mousePosition, box)) {
		DrawRectangleRec(box, selectedColor);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			userInput = "2;" + user + ";" + password + ";";
			isUserInputChanged = true;
			ResetLogin();
			isWaiting = true;
			modifiedTime = filesystem::last_write_time(SystemPath);
		}
	}
	DrawText("Log in", box.x + (box.width - MeasureText("Log in", fontSize)) / 2, box.y + (box.height - fontSize) / 2, fontSize, BLACK);

	//Draw status
	DrawText(status.c_str(), (SCREENWIDTH - MeasureText(status.c_str(), fontSize)) / 2, box.y + 150, fontSize, RED);
	//Forgot password button
	box.x = 900;
	box.y += 200;
	box.height = 100;
	box.width = 370;
	DrawRectangleRec(box, BEIGE);
	if (CheckCollisionPointRec(mousePosition, box)) {
		DrawRectangleRec(box, selectedColor);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			userInput = "4;";
			isUserInputChanged = true;
			ResetLogin();
			Draw = DrawForgotPasswordWindow;
		}
	}
	DrawText("Forgot password", box.x + (box.width - MeasureText("Forgot password", fontSize)) / 2, box.y + (box.height - fontSize) / 2, fontSize, BLACK);

	//Sign up button
	box.x = 200;
	DrawRectangleRec(box, BEIGE);
	if (CheckCollisionPointRec(mousePosition, box)) {
		DrawRectangleRec(box, selectedColor);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			userInput = "3;";
			isUserInputChanged = true;
			ResetLogin();
			Draw = DrawSignUpWindow;
		}
	}
	DrawText("Sign up", box.x + (box.width - MeasureText("Sign up", fontSize)) / 2, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	if (isWaiting) DrawWaitingAnimation();
}
void DrawForgotPasswordWindow() {
	Vector2 mousePosition = GetMousePosition();
	int input = GetKeyPressed();
	if (isWaiting) {
		mousePosition = { -1, -1 };
		input = 0;
		if (filesystem::last_write_time(SystemPath) != modifiedTime) {
			this_thread::sleep_for(std::chrono::duration<double>(0.1));
			fstream f(SystemPath, ios::in);
			string result;
			if (codeInput.empty()) {
				getline(f, result, ';');
				if (result == "N") {
					getline(f, status, ';');
					isTypingGmail = true;
				}
				else {
					getline(f, code, ';');
				}
			}
			f.close();
			isWaiting = false;
			modifiedTime = filesystem::last_write_time(SystemPath);
		}
	}
	if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V)) {
		if (isTypingUser) {
			user += GetClipboardText();
		}
		else if (isTypingGmail) {
			gmail += GetClipboardText();
		}
		else {
			codeInput += GetClipboardText();
		}
		input = 0;
	}
	if (input != 0) {
		if (input == KEY_BACKSPACE) {
			if (isTypingUser) {
				if (user.size() != 0) user.pop_back();
			}
			else if (isTypingGmail) {
				if (gmail.size() != 0) gmail.pop_back();
				else isTypingUser = true;
			}
			else
			{
				if (codeInput.size() == 0) isTypingGmail = true;
				else codeInput.pop_back();
			}
		}
		else if (input == KEY_ENTER) {
			if (isTypingUser) {
				isTypingUser = false;
				isTypingGmail = true;
			}
			else if (isTypingGmail) {
				isTypingGmail = false;
				userInput = "2;" + user + ";" + gmail + "@gmail.com" + ";";
				isUserInputChanged = true;
				isWaiting = true;
				code = "";
				status = "";
				modifiedTime = filesystem::last_write_time(SystemPath);
			}
			else {
				if (codeInput != code) status = "Wrong code!";
				else {
					userInput = "Y;";
					isUserInputChanged = true;
					ResetForgotPassword();
					Draw = DrawSetPasswordWindow;
					modifiedTime = filesystem::last_write_time(SystemPath);
				}
			}
		}
		else if (input >= KEY_A && input <= KEY_Z) {
			if (isTypingUser) {
				user += input + 32;
			}
			else if (isTypingGmail) {
				gmail += input + 32;
			}
		}
		else if (input == KEY_LEFT_CONTROL || input == KEY_RIGHT_CONTROL) {}
		else {
			if (isTypingUser) {
				user += input;
			}
			else if (isTypingGmail) {
				gmail += input;
			}
			else {
				codeInput += input;
			}
		}
	}
	//Back button
	Rectangle backButton = { 0, 0, textures[2].width, textures[2].height };
	DrawTexture(textures[2], 0, 0, WHITE);
	if (CheckCollisionPointRec(mousePosition, backButton)) {
		DrawRectangleRec(backButton, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			userInput = "1;";
			isUserInputChanged = true;
			ResetForgotPassword();
			Draw = DrawLoginWindow;
		}
	}
	//Title
	int titleFontSize = 80;
	string title = "FORGOT PASSWORD";
	DrawText(title.c_str(), (SCREENWIDTH - MeasureText(title.c_str(), titleFontSize)) / 2, 100, titleFontSize, BLACK);
	//Vẽ hộp chữ
	Rectangle box = { 400, 250, 800, 100 }; //x, y, width, height
	DrawText("User", box.x - 120, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	DrawRectangleRec(box, WHITE);
	box.y += 150;
	DrawText("Gmail", box.x - 120, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	DrawRectangleRec(box, WHITE);
	box.y += 150;
	DrawText("Code", box.x - 120, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	DrawRectangleRec(box, WHITE);
	//Vẽ chữ
	int cursorX = box.x + 10;
	int cursorY = box.y + (box.height - fontSize) / 2;
	if (isTypingUser) {
		cursorX += MeasureText(user.c_str(), fontSize);
		cursorY -= 300;
	}
	else if (isTypingGmail) {
		cursorX += MeasureText(gmail.c_str(), fontSize);
		cursorY -= 150;
	}
	else {
		cursorX += MeasureText(codeInput.c_str(), fontSize);
	}
	DrawCursor(cursorX, cursorY);
	DrawText(user.c_str(), box.x + 10, box.y + (box.height - fontSize) / 2 - 300, fontSize, BLACK);
	DrawText(gmail.c_str(), box.x + 10, box.y + (box.height - fontSize) / 2 - 150, fontSize, BLACK);
	DrawText("@gmail.com", box.x + MeasureText(gmail.c_str(), fontSize) + 10, box.y + (box.height - fontSize) / 2 - 150, fontSize, GRAY);
	DrawText(codeInput.c_str(), box.x + 10, box.y + (box.height - fontSize) / 2, fontSize, BLACK);

	//Enter button
	box.y += 150;
	box.height = 100;
	box.width = 200;
	box.x = (SCREENWIDTH - box.width) / 2 - 100;
	DrawRectangleRec(box, BEIGE);
	DrawText("Get code", box.x + (box.width - MeasureText("Get code", fontSize)) / 2, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	if (CheckCollisionPointRec(mousePosition, box)) {
		DrawRectangleRec(box, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			if (code.empty()) {
				isTypingGmail = false;
				userInput = "2;" + user + ";" + gmail + "@gmail.com" + ";";
				isUserInputChanged = true;
				isWaiting = true;
				code = "";
				status = "";
				modifiedTime = filesystem::last_write_time(SystemPath);
			}
			else {
				status = "Already got code! Backspace -> Enter to get again";
			}
		}
	}
	box.x += 300;
	DrawRectangleRec(box, BEIGE);
	DrawText("Enter", box.x + (box.width - MeasureText("Enter", fontSize)) / 2, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	if (CheckCollisionPointRec(mousePosition, box)) {
		DrawRectangleRec(box, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			if (code.empty()) status = "Get code first!";
			else if (codeInput != code) {
				status = "Wrong code!";
			}
			else {
				userInput = "Y;";
				isUserInputChanged = true;
				ResetForgotPassword();
				Draw = DrawSetPasswordWindow;
				modifiedTime = filesystem::last_write_time(SystemPath);
			}
		}
	}
	//Draw status
	DrawText(status.c_str(), (SCREENWIDTH - MeasureText(status.c_str(), fontSize)) / 2, box.y + 150, fontSize, RED);
	//Draw waiting animation
	if (isWaiting) DrawWaitingAnimation();
}
void DrawSetPasswordWindow() {
	Vector2 mousePosition = GetMousePosition();
	int input = GetKeyPressed();
	if (isWaiting) {
		mousePosition = { -1, -1 };
		input = 0;
		if (filesystem::last_write_time(SystemPath) != modifiedTime) {
			this_thread::sleep_for(std::chrono::duration<double>(0.1));
			modifiedTime = filesystem::last_write_time(SystemPath);
			fstream f(SystemPath, ios::in);
			string result;
			getline(f, result, ';');
			if (result == "Y") {
				ResetSetPassword();
				Draw = DrawLoginWindow;
			}
			else {
				getline(f, status, ';');
			}
			f.close();
			isWaiting = false;
		}
	}
	if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V)) {
		if (isTypingConfirmPassword) {
			confirmPassword += GetClipboardText();
		}
		else {
			newPassword += GetClipboardText();
		}
		input = 0;
	}
	if (input != 0) {
		if (input == KEY_BACKSPACE) {
			if (isTypingConfirmPassword) {
				if (confirmPassword.size() != 0) confirmPassword.pop_back();
				else isTypingConfirmPassword = false;
			}
			else {
				if (newPassword.size() != 0) newPassword.pop_back();
			}
		}
		else if (input == KEY_ENTER) {
			if (isTypingConfirmPassword) {
				status = "";
				if (confirmPassword != newPassword) {
					status = "Password not match!";
				}
				else {
					userInput = "2;" + newPassword + ';';
					ResetSetPassword();
					isUserInputChanged = true;
					isWaiting = true;
				}
			}
			else {
				isTypingConfirmPassword = true;
			}
		}
		else if (input >= KEY_A && input <= KEY_Z) {
			if (isTypingConfirmPassword) {
				confirmPassword += input + 32;
			}
			else {
				newPassword += input + 32;
			}
		}
		else if (input == KEY_LEFT_CONTROL || input == KEY_RIGHT_CONTROL) {}
		else {
			if (isTypingConfirmPassword) {
				confirmPassword += input;
			}
			else {
				newPassword += input;
			}
		}
	}
	//Back button
	Rectangle backButton = { 0, 0, textures[2].width, textures[2].height };
	DrawTexture(textures[2], 0, 0, WHITE);
	if (CheckCollisionPointRec(mousePosition, backButton)) {
		DrawRectangleRec(backButton, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			userInput = "1;";
			isUserInputChanged = true;
			ResetSetPassword();
		}
	}
	//Title
	int titleFontSize = 80;
	string title = "SET PASSWORD";
	DrawText(title.c_str(), (SCREENWIDTH - MeasureText(title.c_str(), titleFontSize)) / 2, 100, titleFontSize, BLACK);
	//Vẽ hộp chữ
	Rectangle box = { 400, 250, 800, 100 }; //x, y, width, height
	DrawText("New password", box.x - 300, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	DrawRectangleRec(box, WHITE);
	box.y += 150;
	DrawText("Confirm", box.x - 155, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	DrawRectangleRec(box, WHITE);
	//Vẽ chữ
	int cursorX = box.x + 10;
	int cursorY = box.y + (box.height - fontSize) / 2;
	if (isTypingConfirmPassword) {
		cursorX += MeasureText(confirmPassword.c_str(), fontSize);
	}
	else {
		cursorX += MeasureText(newPassword.c_str(), fontSize);
		cursorY -= 150;
	}
	DrawCursor(cursorX, cursorY);
	DrawText(newPassword.c_str(), box.x + 10, box.y + (box.height - fontSize) / 2 - 150, fontSize, BLACK);
	DrawText(confirmPassword.c_str(), box.x + 10, box.y + (box.height - fontSize) / 2, fontSize, BLACK);

	//Enter button
	box.y += 150;
	box.height = 100;
	box.width = 200;
	box.x = (SCREENWIDTH - box.width) / 2;
	DrawRectangleRec(box, BEIGE);
	DrawText("Enter", box.x + (box.width - MeasureText("Enter", fontSize)) / 2, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	if (CheckCollisionPointRec(mousePosition, box)) {
		DrawRectangleRec(box, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			if (isTypingConfirmPassword) {
				status = "";
				if (confirmPassword != newPassword) {
					status = "Password not match!";
				}
				else {
					userInput = "2;" + newPassword + ';';
					isUserInputChanged = true;
					isWaiting = true;
					ResetSetPassword();
					Draw = DrawLoginWindow;
				}
			}
			else {
				status = "Enter confirm password first!";
			}
		}
	}
	//Draw status
	DrawText(status.c_str(), (SCREENWIDTH - MeasureText(status.c_str(), fontSize)) / 2, box.y + 150, fontSize, RED);
	//Draw waiting animation
	if (isWaiting) DrawWaitingAnimation();
}
void DrawSignUpWindow() {
	Vector2 mousePosition = GetMousePosition();
	int input = GetKeyPressed();
	if (isWaiting) { //CODE HERE
		//Animation waiting
		mousePosition = { -1, -1 };
		input = 0;
		if (filesystem::last_write_time(SystemPath) != modifiedTime) {
			this_thread::sleep_for(std::chrono::duration<double>(0.1));
			fstream f(SystemPath, ios::in);
			string result;
			getline(f, result, ';');
			if (result == "N") {
				getline(f, status, ';');
			}
			else {
				getline(f, code, ';');
				Draw = DrawConfirmationWindow;
			}
			f.close();
			isWaiting = false;
			modifiedTime = filesystem::last_write_time(SystemPath);
		}
	}
	if (input != 0) {
		if (input == KEY_BACKSPACE) {
			if (isTypingUser) {
				if (user.size() != 0) user.pop_back();
			}
			else if (isTypingGmail) {
				if (gmail.size() == 0) isTypingGmail = false;
				else gmail.pop_back();
			}
			else {
				if (password.size() == 0) isTypingUser = true;
				else password.pop_back();
			}
		}
		else if (input == KEY_ENTER) {
			if (isTypingUser) isTypingUser = false;
			else if (isTypingGmail) {
				userInput = "2;" + user + ";" + password + ";" + gmail + ";";
				isUserInputChanged = true;
				ResetSignUp();
				isWaiting = true;
			}
			else {
				isTypingGmail = true;
			}
		}
		else if (input >= KEY_A && input <= KEY_Z) {
			if (isTypingUser) {
				user += input + 32;
			}
			else if (isTypingGmail) {
				gmail += input + 32;
			}
			else {
				password += input + 32;
			}
		}
		else {
			if (isTypingUser) {
				user += input;
			}
			else if (isTypingGmail) {
				gmail += input;
			}
			else {
				password += input;
			}
		}
	}
	//Back button
	Rectangle backButton = { 0, 0, textures[2].width, textures[2].height };
	DrawTexture(textures[2], 0, 0, WHITE);
	if (CheckCollisionPointRec(mousePosition, backButton)) {
		DrawRectangleRec(backButton, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			this_thread::sleep_for(std::chrono::duration<double>(0.5));
			userInput = "1;";
			isUserInputChanged = true;
			Draw = DrawLoginWindow;
		}
	}
	//Title
	int titleFontSize = 80;
	string title = "SIGN UP";
	DrawText(title.c_str(), (SCREENWIDTH - MeasureText(title.c_str(), titleFontSize)) / 2, 100, titleFontSize, BLACK);
	//Vẽ hộp chữ
	Rectangle box = { 400, 250, 800, 100 }; //x, y, width, height
	DrawText("User", box.x - 120, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	DrawRectangleRec(box, WHITE);
	DrawText(user.c_str(), box.x + 10, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	box.y += 150;
	DrawText("Password", box.x - 220, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	DrawRectangleRec(box, WHITE);
	DrawText(password.c_str(), box.x + 10, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	box.y += 150;
	DrawText("Gmail", box.x - 120, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	DrawRectangleRec(box, WHITE);
	DrawText(gmail.c_str(), box.x + 10, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	DrawText("@gmail.com", box.x + 10 + MeasureText(gmail.c_str(), fontSize), box.y + (box.height - fontSize) / 2, fontSize, GRAY);
	//Vẽ chữ
	int cursorX = box.x + 10;
	int cursorY = box.y + (box.height - fontSize) / 2;
	if (isTypingUser) {
		cursorX += MeasureText(user.c_str(), fontSize);
		cursorY -= 300;
	}
	else if (isTypingGmail) {
		cursorX += MeasureText(gmail.c_str(), fontSize);
	}
	else {
		cursorX += MeasureText(password.c_str(), fontSize);
		cursorY -= 150;
	}
	DrawCursor(cursorX, cursorY);

	//Enter button
	box.y += 150;
	box.height = 100;
	box.width = 200;
	box.x = (SCREENWIDTH - box.width) / 2;
	DrawRectangleRec(box, BEIGE);
	DrawText("Enter", box.x + (box.width - MeasureText("Enter", fontSize)) / 2, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	if (CheckCollisionPointRec(mousePosition, box)) {
		DrawRectangleRec(box, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			userInput = "2;" + user + ";" + password + ";" + gmail + ";";
			isUserInputChanged = true;
			ResetSignUp();
			isWaiting = true;
		}
	}
	//Draw status
	DrawText(status.c_str(), (SCREENWIDTH - MeasureText(status.c_str(), fontSize)) / 2, box.y + 150, fontSize, RED);
	//Draw waiting animation
	if (isWaiting) DrawWaitingAnimation();
}
void DrawConfirmationWindow() {
	Vector2 mousePosition = GetMousePosition();
	int input = GetKeyPressed();
	//Back button
	Rectangle backButton = { 0, 0, textures[2].width, textures[2].height };
	DrawTexture(textures[2], 0, 0, WHITE);
	if (CheckCollisionPointRec(mousePosition, backButton)) {
		DrawRectangleRec(backButton, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			userInput = "N;";
			isUserInputChanged = true;
			ResetConfirmation();
			Draw = DrawLoginWindow;
		}
	}
	//Title
	int titleFontSize = 80;
	string title = "CONFIRMATION";
	DrawText(title.c_str(), (SCREENWIDTH - MeasureText(title.c_str(), titleFontSize)) / 2, 100, titleFontSize, BLACK);
	DrawText("Please enter the code sent to your email", (SCREENWIDTH - MeasureText("Please enter the code sent to your email", fontSize)) / 2, 330, fontSize, GRAY);
	//Vẽ hộp chữ
	Rectangle box = { 400, 400, 800, 100 }; //x, y, width, height
	DrawText("Code", box.x - 120, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	DrawRectangleRec(box, WHITE);
	//Ve chu
	int cursorX = box.x + 10 + MeasureText(user.c_str(), fontSize);
	int cursorY = box.y + (box.height - fontSize) / 2;
	DrawCursor(cursorX, cursorY);
	DrawText(codeInput.c_str(), box.x + 10, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	//Enter button
	box.y += 150;
	box.height = 100;
	box.width = 200;
	box.x = (SCREENWIDTH - box.width) / 2;
	DrawRectangleRec(box, BEIGE);
	DrawText("Enter", box.x + (box.width - MeasureText("Enter", fontSize)) / 2, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	if (CheckCollisionPointRec(mousePosition, box)) {
		DrawRectangleRec(box, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			if (codeInput == code) {
				userInput = "Y;";
				isUserInputChanged = true;
				DrawText(status.c_str(), (SCREENWIDTH - MeasureText(status.c_str(), fontSize)) / 2, box.y + 150, fontSize, GREEN);
				this_thread::sleep_for(std::chrono::duration<double>(2));
				ResetConfirmation();
				Draw = DrawLoginWindow;
			}
			else {
				codeInput = "";
				status = "Wrong code!";
			}
		}
	}
	//Draw status
	DrawText(status.c_str(), (SCREENWIDTH - MeasureText(status.c_str(), fontSize)) / 2, box.y + 150, fontSize, RED);
	if (input != 0) {
		if (input == KEY_BACKSPACE) {
			if (codeInput.size() != 0) codeInput.pop_back();
		}
		else if (input == KEY_ENTER) {
			if (codeInput == code) {
				userInput = "Y;";
				isUserInputChanged = true;
				status = "Sign up successfully!";
				DrawText(status.c_str(), (SCREENWIDTH - MeasureText(status.c_str(), fontSize)) / 2, box.y + 150, fontSize, GREEN);
				this_thread::sleep_for(std::chrono::duration<double>(2));
				ResetConfirmation();
				Draw = DrawLoginWindow;
			}
			else {
				codeInput = "";
				status = "Wrong code!";
			}
		}
		else if (input >= KEY_A && input <= KEY_Z) {
			codeInput += input + 32;
		}
		else {
			codeInput += input;
		}
	}

}
void DrawGetFilePathWindow() {
	DrawRectangle(0, 0, SCREENWIDTH, SCREENHEIGHT, { 0, 0, 0, 200 });
	Vector2 mousePosition = GetMousePosition();
	int input = GetKeyPressed();
	if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V)) {
		filePath += GetClipboardText();
		input = 0;
	}
	if (input != 0) {
		if (input == KEY_BACKSPACE) {
			if (!filePath.empty()) filePath.pop_back();
		}
		else if (input == KEY_ENTER) {
			userInput = CopyOrDelete + ";" + to_string(clientSelectedCount) + ';';
			for (int i = 0; i < clientList.size(); i++) {
				if (clientSelected[i])
					userInput += clientList[i] + ";";
			}
			userInput += filePath + ";";
			filePath = "";
			isUserInputChanged = true;
			isGettingFilePath = false;
			isWaiting = true;
		}
		else if (input >= KEY_A && input <= KEY_Z) {
			filePath += input + 32;
		}
		else if (input == KEY_LEFT_CONTROL || input == KEY_RIGHT_CONTROL) {
		}
		else {
			filePath += input;
		}
	}
	//Get File Box
	Rectangle box = { 100, 300, 1300, 300 };
	DrawRectangleRec(box, WHITE);
	DrawText("Enter the file path", box.x + (box.width - MeasureText("Enter the file path", fontSize)) / 2, box.y + 50, fontSize, GRAY);
	box = { 350, 450, 1000, 60 };
	DrawText("File path: ", box.x - 200, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	DrawRectangleLinesEx(box, 4, BLACK);
	DrawText(filePath.c_str(), box.x + 10, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	DrawCursor(box.x + MeasureText(filePath.c_str(), fontSize) + 10, box.y + (box.height - fontSize) / 2);
	//Back button
	Rectangle backButton = { 0, 0, textures[2].width, textures[2].height };
	DrawTexture(textures[2], 0, 0, WHITE);
	if (CheckCollisionPointRec(mousePosition, backButton)) {
		DrawRectangleRec(backButton, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			filePath = "";
			isGettingFilePath = false;
		}
	}
}
void DrawSuccessNotification() {
	if (successNotificationQuery == 11 || successNotificationQuery == 14) {
		isShowSuccessNotification = false;
	}
	DrawRectangle(0, 0, SCREENWIDTH, SCREENHEIGHT, { 0, 0, 0, 200 });
	auto mousePosition = GetMousePosition();
	//Back button
	Rectangle backButton = { 0, 0, textures[2].width, textures[2].height };
	DrawTexture(textures[2], 0, 0, WHITE);
	if (CheckCollisionPointRec(mousePosition, backButton)) {
		DrawRectangleRec(backButton, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			isShowSuccessNotification = false;
		}
	}
	//Box
	string mainNoti, subNoti;
	if (successNotificationQuery == 1) {
		mainNoti = "Change password successfully!";
	}
	else if (successNotificationQuery == 3) {
		mainNoti = "Add client successfully!";
	}
	else if(successNotificationQuery == 4) {
		mainNoti = "Remove client successfully!";
	}
	else if (successNotificationQuery == 12 || successNotificationQuery == 15) {
		mainNoti = "Start program succesfully";
	}
	else if (successNotificationQuery == 13 || successNotificationQuery == 16) {
		mainNoti = "Stop program succesfully";
	}
	else if (successNotificationQuery == 17) {
		mainNoti = "Shutdown successfully!";
	}
	else if (successNotificationQuery == 18) {
		mainNoti = "Reset successfully!";
	}
	else if (successNotificationQuery == 20) {
		mainNoti = "Copy successfully!";
		subNoti = "All copy files is saved at " + ComPath + "Copy";
	}
	else if (successNotificationQuery == 21) {
		mainNoti = "Delete successfully!";
	}
	else if (successNotificationQuery == 22) {
		mainNoti = "Capture screen successfully!";
		subNoti = "All images is saved at " + ComPath + "Capture";
	}
	else if (successNotificationQuery == 23) {
		mainNoti = "Keylogger on!";
	}
	else if (successNotificationQuery == 24) {
		mainNoti = "Keylogger off!";
		subNoti = "All files is saved at " + ComPath + "Keylogger";
	}
	else if (successNotificationQuery == 26) {
		mainNoti = "Webcam on!";
	}
	else if (successNotificationQuery == 27) {
		mainNoti = "Webcam off!";
	}
	int y = 200;
	DrawText(mainNoti.c_str(), (SCREENWIDTH - MeasureText(mainNoti.c_str(), fontSize)) / 2, y, fontSize, GREEN);
	int i = 0;
	int maxLength = 1000;
	y += 50;
	while (i < subNoti.size()) {
		string tmp = "";
		while (MeasureText(tmp.c_str(), fontSize) < maxLength) {
			tmp += subNoti[i];
			i++;
			if (i >= subNoti.size()) break;
		}
		DrawText(tmp.c_str(), (SCREENWIDTH - MeasureText(tmp.c_str(), fontSize)) / 2, y, fontSize, GREEN);
		y += 50;
	}
}
void DrawFailNotification() {
	DrawRectangle(0, 0, SCREENWIDTH, SCREENHEIGHT, { 0, 0, 0, 200 });
	Vector2 mousePosition = GetMousePosition();
	//Back button
	Rectangle backButton = { 0, 0, textures[2].width, textures[2].height };
	DrawTexture(textures[2], 0, 0, WHITE);
	if (CheckCollisionPointRec(mousePosition, backButton)) {
		DrawRectangleRec(backButton, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			newClientID = "";
			isShowFailNotification = false;
		}
	}
	//Show noti
	DrawText(failNoti.c_str(), (SCREENWIDTH - MeasureText(failNoti.c_str(), fontSize)) / 2, 300, fontSize, RED);
}
void DrawGetAppNameWindow() {
	DrawRectangle(0, 0, SCREENWIDTH, SCREENHEIGHT, { 0, 0, 0, 200 });
	Vector2 mousePosition = GetMousePosition();
	int input = GetKeyPressed();
	if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V)) {
		filePath += GetClipboardText();
		input = 0;
	}
	if (input != 0) {
		if (input == KEY_BACKSPACE) {
			if (!appName.empty()) appName.pop_back();
		}
		else if (input == KEY_ENTER) {
			userInput = StartOrStop + ";" + to_string(clientSelectedCount) + ';';
			for (int i = 0; i < clientList.size(); i++) {
				if (clientSelected[i])
					userInput += clientList[i] + ";";
			}
			userInput += appName + ";";
			appName = "";
			isUserInputChanged = true;
			isGettingAppName = false;
			isWaiting = true;
		}
		else if (input >= KEY_A && input <= KEY_Z) {
			appName += input + 32;
		}
		else if (input == KEY_LEFT_CONTROL || input == KEY_RIGHT_CONTROL) {
		}
		else {
			appName += input;
		}
	}
	//Get File Box
	Rectangle box = { 100, 300, 1300, 300 };
	DrawRectangleRec(box, WHITE);
	DrawText("Enter the program path", box.x + (box.width - MeasureText("Enter the program name", fontSize)) / 2, box.y + 50, fontSize, GRAY);
	box = { 350, 450, 1000, 60 };
	DrawText("Program: ", box.x - 200, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	DrawRectangleLinesEx(box, 4, BLACK);
	DrawText(appName.c_str(), box.x + 10, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	DrawCursor(box.x + MeasureText(appName.c_str(), fontSize) + 10, box.y + (box.height - fontSize) / 2);
	//Back button
	Rectangle backButton = { 0, 0, textures[2].width, textures[2].height };
	DrawTexture(textures[2], 0, 0, WHITE);
	if (CheckCollisionPointRec(mousePosition, backButton)) {
		DrawRectangleRec(backButton, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			appName = "";
			isGettingAppName = false;
		}
	}
}
void DrawGetClientIDWindow() {
	DrawRectangle(0, 0, SCREENWIDTH, SCREENHEIGHT, { 0, 0, 0, 200 });
	Vector2 mousePosition = GetMousePosition();
	int input = GetKeyPressed();
	if (input != 0) {
		if (input == KEY_BACKSPACE) {
			if (!newClientID.empty()) newClientID.pop_back();
		}
		else if (input == KEY_ENTER) {
			userInput = AddOrRemove + ";0;C";
			userInput += newClientID + ";";
			isUserInputChanged = true;
			isGettingClientID = false;
			isWaiting = true;
		}
		else if (input >= KEY_ZERO && input <= KEY_NINE) {
			newClientID += input;
		}
	}
	//Get File Box
	Rectangle box = { 100, 300, 1300, 300 };
	DrawRectangleRec(box, WHITE);
	DrawText("Enter the clientID", box.x + (box.width - MeasureText("Enter the clientID", fontSize)) / 2, box.y + 50, fontSize, GRAY);
	box = { 350, 450, 1000, 60 };
	DrawText("ClientID: ", box.x - 200, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	DrawRectangleLinesEx(box, 4, BLACK);
	DrawText("C", box.x + 10, box.y + (box.height - fontSize) / 2, fontSize, GRAY);
	DrawText(newClientID.c_str(), box.x + 35, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	DrawCursor(box.x + MeasureText(newClientID.c_str(), fontSize) + 35, box.y + (box.height - fontSize) / 2);
	//Back button
	Rectangle backButton = { 0, 0, textures[2].width, textures[2].height };
	DrawTexture(textures[2], 0, 0, WHITE);
	if (CheckCollisionPointRec(mousePosition, backButton)) {
		DrawRectangleRec(backButton, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			newClientID = "";
			isGettingClientID = false;
		}
	}
}
void DrawWaitingNewClientIDAcceptWindow() {
	DrawRectangle(0, 0, SCREENWIDTH, SCREENHEIGHT, { 0, 0, 0, 200 });
	if (filesystem::last_write_time(SystemPath) != modifiedTime) {
		this_thread::sleep_for(std::chrono::duration<double>(0.1));
		modifiedTime = filesystem::last_write_time(SystemPath);
		fstream f(SystemPath, ios::in);
		string res;
		getline(f, res, ';');
		if (res == "Y") {
			successNotificationQuery = 3;
			isShowSuccessNotification = true;
			DrawSuccessNotification();
			clientList.push_back("C" + newClientID);
			clientSelected.push_back(true);
			int n = clientList.size();
			maxClientPage = (n + maxClientInRow * 3 - 1) / (maxClientInRow * 3);
			clientSelectedCount++;
		}
		else {
			isShowFailNotification = true;
			failNoti = "Recjected";
		}
		newClientID = "";
		isWaitingNewClientIDAccept = false;
	}
	else {
		DrawText("Waiting for new clientID accept", (SCREENWIDTH - MeasureText("Waiting for new clientID accept", fontSize)) / 2, 200, fontSize, WHITE);
		//Animation
		DrawAnimation(1, 550.0f, 300.0f);
	}
}
void DrawGetNewPassword() {
	DrawRectangle(0, 0, SCREENWIDTH, SCREENHEIGHT, { 0, 0, 0, 200 });
	Vector2 mousePosition = GetMousePosition();
	int input = GetKeyPressed();
	if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V)) {
		filePath += GetClipboardText();
		input = 0;
	}
	if (input != 0) {
		if (input == KEY_BACKSPACE) {
			if (isTypingNewPassword) {
				if (!newPassword.empty()) newPassword.pop_back();
				else isTypingNewPassword = false;
			}
			else if (!currentPassword.empty()) currentPassword.pop_back();
		}
		else if (input == KEY_ENTER) {
			if (isTypingNewPassword) {
				userInput = "1;0;" + currentPassword + '#' + newPassword + ';';
				isUserInputChanged = true;
				ResetGetNewPassword();
				isWaiting = true;
			}
			else {
				isTypingNewPassword = true;
			}
		}
		else if (input >= KEY_A && input <= KEY_Z) {
			if (isTypingNewPassword) {
				newPassword += input + 32;
			}
			else {
				currentPassword += input + 32;
			}
		}
		else if (input == KEY_LEFT_CONTROL || input == KEY_RIGHT_CONTROL) {
		}
		else {
			if (isTypingNewPassword) {
				newPassword += input;
			}
			else {
				currentPassword += input;
			}
		}
	}
	//Get File Box
	Rectangle box = { 100, 300, 1300, 500 };
	DrawRectangleRec(box, WHITE);
	DrawText("Change password", box.x + (box.width - MeasureText("Change password", fontSize)) / 2, box.y + 50, fontSize, GRAY);
	box = { 420, 450, 900, 60 };
	DrawText("Password: ", box.x - 210, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	DrawRectangleLinesEx(box, 4, BLACK);
	DrawText(currentPassword.c_str(), box.x + 10, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	if (!isTypingNewPassword) DrawCursor(box.x + MeasureText(currentPassword.c_str(), fontSize) + 10, box.y + (box.height - fontSize) / 2);
	box = { 420, 550, 900, 60 };
	DrawText("New password: ", box.x - 300, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	DrawRectangleLinesEx(box, 4, BLACK);
	DrawText(newPassword.c_str(), box.x + 10, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
	if (isTypingNewPassword) DrawCursor(box.x + MeasureText(newPassword.c_str(), fontSize) + 10, box.y + (box.height - fontSize) / 2);
	//Back button
	Rectangle backButton = { 0, 0, textures[2].width, textures[2].height };
	DrawTexture(textures[2], 0, 0, WHITE);
	if (CheckCollisionPointRec(mousePosition, backButton)) {
		DrawRectangleRec(backButton, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			ResetGetNewPassword();
		}
	}
}
void DrawAdminWindow() {
	Vector2 mousePosition = GetMousePosition();
	if (isWaiting) {
		mousePosition = { -1, -1 };
		if (filesystem::last_write_time(SystemPath) != modifiedTime) {
			this_thread::sleep_for(std::chrono::duration<double>(0.1));
			modifiedTime = filesystem::last_write_time(SystemPath);
			fstream f(SystemPath, ios::in);
			string query;
			getline(f, query, ';');
			if (query == "1") {
				string y;
				getline(f, y, ';');
				if (y == "N") {
					getline(f, failNoti, ';');
					isShowFailNotification = true;
				}
				else {
					successNotificationQuery = stoi(query);
					isShowSuccessNotification = true;
				}
			}
			else if (query == "3") {
				string y;
				getline(f, y, ';');
				if (y == "Y") {
					isWaitingNewClientIDAccept = true;
				}
				else {
					isShowFailNotification = true;
					getline(f, failNoti, ';');
				}
			}
			else if (query == "4") {
				string y;
				getline(f, y, ';');
				if (y == "Y") {
					string removedClientID;
					getline(f, removedClientID, ';');
					auto it = find(clientList.begin(), clientList.end(), removedClientID);
					int index = distance(clientList.begin(), it);
					clientList.erase(it);
					if (clientSelected[index]) clientSelectedCount--;
					clientSelected.erase(clientSelected.begin() + index);
					maxClientPage = (clientList.size() + maxClientInRow * 3 - 1) / (maxClientInRow * 3);
					if (clientPage > maxClientPage) clientPage = maxClientPage;
				}
				else {
					isShowFailNotification = true;
					getline(f, failNoti, ';');
				}
				newClientID = "";
			}
			else if (query == "5") {
				Draw = DrawLoginWindow;
			}
			else if (query == "11") {
				LoadList("ListApp");
				Draw = DrawListAppWindow;
			}
			else if(query == "14") {
				LoadList("ListService");
				Draw = DrawListServiceWindow;
			}
			string y;
			getline(f, y, ';');
			if (y == "Y") {
				successNotificationQuery = stoi(query);
				isShowSuccessNotification = true;
			}
			else if (y == "N") {
				string size;
				getline(f, size, ';');
				int n = stoi(size);
				string clientID;
				failNoti = "Execution failed for client IDs:";
				for (int i = 0; i < n; i++) {
					getline(f, clientID, ';');
					failNoti += " " + clientID;
				}
				isShowFailNotification = true;
			}
			f.close();
			isWaiting = false;
		}
	}
	if (isGettingFilePath || isShowSuccessNotification || isGettingAppName || isGettingClientID || isWaitingNewClientIDAccept
		|| isShowFailNotification || isGettingNewPassword) {
		mousePosition = { -1, -1 };
	}
	//user button
	Color color = WHITE;
	Rectangle box = { 0, 0, textures[14].width, textures[14].height };
	DrawTexture(textures[14], 0, 0, color);
	if (CheckCollisionPointRec(mousePosition, box)) {
		DrawRectangleRec(box, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			showUserBox = !showUserBox;
		}
	}
	//Admin ID
	box = { 0, (float)textures[14].height + 30, 140, 60};
	DrawRectangleRec(box, color);
	DrawText(("ID: " + yourAdminID).c_str(), box.x + (box.width - MeasureText(("ID: " + yourAdminID).c_str(), queryFontSize)) / 2, box.y + (box.height - queryFontSize) / 2, queryFontSize, BLACK);
	//Client icon
	string label = "Clients";
	box = { 150, 10, 180, 180 }; //x, y, width, height
	DrawRectangle(box.x, box.y, box.width, box.height, color);
	DrawTexture(textures[15], box.x + (box.width - textures[15].width) / 2, box.y + 10, WHITE);
	DrawText(label.c_str(), box.x + (box.width - MeasureText(label.c_str(), queryFontSize)) / 2,
		box.y + (box.height + 10 + textures[15].height - queryFontSize) / 2, queryFontSize, BLACK);
	//client list
	box = { 340, 10, 1000, 180 };
	DrawRectangleRec(box, color);
	//AddClient
	box = { 1350, 10, 50, 50 };
	DrawRectangleRec(box, GREEN);
	DrawText("+", box.x + (box.width - MeasureText("+", queryFontSize)) / 2, box.y + (box.height - queryFontSize) / 2, queryFontSize, BLACK);
	if (CheckCollisionPointRec(mousePosition, box)) {
		DrawRectangleRec(box, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			isGettingClientID = true;
			AddOrRemove = "3";
		}
	}
	//RemoveClient
	box = { 1410, 10, 50, 50 };
	DrawRectangleRec(box, RED);
	DrawText("-", box.x + (box.width - MeasureText("-", queryFontSize)) / 2, box.y + (box.height - queryFontSize) / 2, queryFontSize, BLACK);
	if (CheckCollisionPointRec(mousePosition, box)) {
		DrawRectangleRec(box, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			isGettingClientID = true;
			AddOrRemove = "4";
		}
	}
	box = { 350, -15, (float)queryFontSize, (float)queryFontSize };
	int maxClientShow = min(clientPage * maxClientInRow * 3, (int)clientList.size());
	for (int i = (clientPage - 1) * maxClientInRow * 3; i < maxClientShow; i++) {
		if (i % maxClientInRow == 0) {
			box.y += box.height * 2;
			box.x = 350;
		}
		else {
			box.x += box.width + 180;
		}
		DrawRectangleLinesEx(box, 2, BLACK);
		DrawText(clientList[i].c_str(), box.x + box.width + 10, box.y, queryFontSize, BLACK);
		if (clientSelected[i]) DrawText("x", box.x + (box.width - MeasureText("x", queryFontSize)) / 2, box.y, queryFontSize, GREEN);
		if (CheckCollisionPointRec(mousePosition, box)) {
			DrawRectangleRec(box, GRAY);
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				if (clientSelected[i]) {
					clientSelectedCount--;
				}
				else {
					clientSelectedCount++;
				}
				clientSelected[i] = !clientSelected[i];
			}
		}
	}
	Vector2 v1 = { 1370, 100 };
	Vector2 v2 = { 1400, 115 };
	Vector2 v3 = { 1400, 85 };
	DrawTriangle(v1, v2, v3, GRAY);
	if (CheckCollisionPointTriangle(mousePosition, v1, v2, v3)) {
		DrawTriangle(v1, v2, v3, DARKGRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			clientPage--;
			if (clientPage < 1) {
				clientPage = maxClientPage;
			}
		}
	}
	v1 = { 1410, 85 };
	v2 = { 1410, 115 };
	v3 = { 1440, 100 };
	DrawTriangle(v1, v2, v3, GRAY);
	if (CheckCollisionPointTriangle(mousePosition, v1, v2, v3)) {
		DrawTriangle(v1, v2, v3, DARKGRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			clientPage++;
			if (clientPage > maxClientPage) {
				clientPage = 1;
			}
		}
	}

	//Queries
	int maxBoxInRow = 4;
	int startX = 240;
	box = { 240, 0, 180, 180 };
	for (int i = 0; i < adminMainQueryNames.size(); i++) {
		color = WHITE;
		if (i % maxBoxInRow == 0) {
			box.y += box.height + 50;
			box.x = startX;
		}
		else {
			box.x += box.width + 100;
		}
		if (CheckCollisionPointRec(mousePosition, box)) {
			color = selectedColor;
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				if (adminMainQueryNumbers[i] == "20" || adminMainQueryNumbers[i] == "21") {
					isGettingFilePath = true;
					CopyOrDelete = adminMainQueryNumbers[i];
					filePath = "";
				}
				else if (adminMainQueryNumbers[i] == "12" || adminMainQueryNumbers[i] == "13") {
					isGettingAppName = true;
					StartOrStop = adminMainQueryNumbers[i];
					appName = "";
				}
				else {
					userInput = adminMainQueryNumbers[i] + ";" + to_string(clientSelectedCount) + ";";
					for (int i = 0; i < clientList.size(); i++) {
						if (!clientSelected[i]) continue;
						userInput += clientList[i] + ";";
					}
					isUserInputChanged = true;
					isWaiting = true;
				}
			}
		}
		DrawRectangle(box.x, box.y, box.width, box.height, color);
		DrawTexture(textures[i + 3], box.x + (box.width - textures[i + 3].width) / 2, box.y + 10, WHITE);
		DrawText(adminMainQueryNames[i].c_str(), box.x + (box.width - MeasureText(adminMainQueryNames[i].c_str(), queryFontSize)) / 2,
			box.y + (box.height + 10 + textures[i + 3].height - queryFontSize) / 2, queryFontSize, BLACK);
	}
	//User box
	if (showUserBox) {
		box = { 0, 0, (float)textures[12].width, (float)textures[12].height };
		box.width = 300;
		for (int i = 0; i < userQueryNames.size(); i++) {
			box.y += box.height;
			if (CheckCollisionPointRec(mousePosition, box)) {
				color = selectedColor;
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					if (userQueryNumbers[i] == "1") {
						ResetGetNewPassword();
						isGettingNewPassword = true;
					}
					else if (userQueryNumbers[i] == "5") {
						userInput = "5;0;;";
						isUserInputChanged = true;
						isWaiting = true;
						showUserBox = false;
					}
				}
			}
			DrawRectangle(box.x, box.y, box.width, box.height, color);
			DrawRectangleLinesEx(box, 2, BLACK);
			DrawText(userQueryNames[i].c_str(), box.x + (box.width - MeasureText(userQueryNames[i].c_str(), queryFontSize)) / 2,
				box.y + (box.height - queryFontSize) / 2, queryFontSize, BLACK);
			color = WHITE;
		}
	}
	if (isWaiting)
		DrawWaitingAnimation();
	else if (isGettingFilePath) {
		DrawGetFilePathWindow();
	}
	else if (isShowSuccessNotification) {
		DrawSuccessNotification();
	}
	else if (isShowFailNotification) {
		DrawFailNotification();
	}
	else if (isGettingAppName) {
		DrawGetAppNameWindow();
	}
	else if (isGettingClientID) {
		DrawGetClientIDWindow();
	}
	else if (isWaitingNewClientIDAccept) {
		DrawWaitingNewClientIDAcceptWindow();
	}
	else if (isGettingNewPassword) {
		DrawGetNewPassword();
	}
}
void DrawServerClientWindow() {
	Vector2 mousePosition = GetMousePosition();
	if (!clientQuery.empty()) {
		if (clientQuery == "3") {
			DrawRectangle(0, 0, SCREENWIDTH, SCREENHEIGHT, { 0, 0, 0, 200 });
			Rectangle box = { 100, 300, 1300, 500 };
			DrawRectangleRec(box, WHITE);
			string text = "Accept connect from " + receiverID + "?";
			DrawText(text.c_str(), box.x + (box.width - MeasureText(text.c_str(), fontSize)) / 2, box.y + 200, fontSize, BLACK);
			box = { 600, box.y + 100, 100, 100 };
			DrawRectangleRec(box, GREEN);
			DrawText("Yes", box.x + (box.width - MeasureText("Yes", fontSize)) / 2, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
			if (CheckCollisionPointRec(mousePosition, box)) {
				DrawRectangleRec(box, GRAY);
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					this_thread::sleep_for(std::chrono::duration<double>(0.5));
					userInput = "Y;";
					isUserInputChanged = true;
					clientQuery = "4";
				}
			}
			box.x += 200;
			DrawRectangleRec(box, RED);
			DrawText("No", box.x + (box.width - MeasureText("No", fontSize)) / 2, box.y + (box.height - fontSize) / 2, fontSize, BLACK);
			if (CheckCollisionPointRec(mousePosition, box)) {
				DrawRectangleRec(box, GRAY);
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					this_thread::sleep_for(std::chrono::duration<double>(0.5));
					userInput = "N;";
					isUserInputChanged = true;
					clientQuery = "4";
				}
			}
			mousePosition = { -1, -1 };
		}
		else if (filesystem::last_write_time(SystemPath) != modifiedTime) {
			this_thread::sleep_for(std::chrono::duration<double>(0.1));
			modifiedTime = filesystem::last_write_time(SystemPath);
			clientQuery = "";
		}
		else {
			string text = "Excute request from admin...";
			DrawText(text.c_str(), (SCREENWIDTH - MeasureText(text.c_str(), fontSize)) / 2, 400, fontSize, BLACK);
			//Executing Animation
		}
	}
	else {
		string text = "Waiting for request from admin...";
		DrawText(text.c_str(), (SCREENWIDTH - MeasureText(text.c_str(), fontSize)) / 2, 400, fontSize, BLACK);
		//Empty Animation
		DrawAnimation(0, 600.0f, 500.0f);
		if (filesystem::last_write_time(SystemPath) != modifiedTime) {
			this_thread::sleep_for(std::chrono::duration<double>(0.1));
			modifiedTime = filesystem::last_write_time(SystemPath);
			fstream file(SystemPath, ios::in);
			getline(file, clientQuery, ';');
			if (clientQuery == "3")  getline(file, receiverID, ';');
		}
	}
	//Back button
	Rectangle backButton = { 0, 0, textures[2].width, textures[2].height };
	DrawTexture(textures[2], 0, 0, WHITE);
	if (CheckCollisionPointRec(mousePosition, backButton)) {
		DrawRectangleRec(backButton, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			userInput = "B;";
			isUserInputChanged = true;
			this_thread::sleep_for(std::chrono::duration<double>(0.1));
			Draw = DrawStartWindow;
		}
	}
	//Title
	DrawText("SERVER - CLIENT", (SCREENWIDTH - MeasureText("SERVER - CLIENT", 80)) / 2, 100, 80, BLACK);
	DrawText(("ID: " + yourClientID).c_str(), (SCREENWIDTH - MeasureText(("ID: " + yourClientID).c_str(), fontSize)) / 2, 220, fontSize, BLACK);
}
void DrawListAppWindow() { //Change variable to list service
	Vector2 mousePosition = GetMousePosition();
	if (isWaiting) {
		mousePosition = { -1, -1 };
		if (filesystem::last_write_time(SystemPath) != modifiedTime) {
			this_thread::sleep_for(std::chrono::duration<double>(0.1));
			modifiedTime = filesystem::last_write_time(SystemPath);
			fstream f(SystemPath, ios::in);
			string query;
			getline(f, query, ';');
			string y;
			getline(f, y, ';');
			if (y == "Y") {
				isShowSuccessNotification = true;
				successNotificationQuery = stoi(query);
			}
			else {
				string size;
				getline(f, size, ';');
				int n = stoi(size);
				string clientID;
				failNoti = "Execution failed for client IDs:";
				for (int i = 0; i < n; i++) {
					getline(f, clientID, ';');
					failNoti += " " + clientID;
				}
				isShowFailNotification = true;
			}
			f.close();
			isWaiting = false;
		}
	}
	if (isShowFailNotification || isShowSuccessNotification) {
		mousePosition = { -1, -1 };
	}
	//Back button
	Rectangle backButton = { 0, 0, textures[2].width, textures[2].height };
	DrawTexture(textures[2], 0, 0, WHITE);
	if (CheckCollisionPointRec(mousePosition, backButton)) {
		DrawRectangleRec(backButton, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			isWaiting = false;
			Draw = DrawAdminWindow;
		}
	}
	//Title
	DrawText("LIST APPS", (SCREENWIDTH - MeasureText("LIST APPS", 80)) / 2, 100, 80, BLACK);
	//Apps
	Rectangle box = { 150, 200, 1200, 650 }; //x, y, width, height
	DrawRectangleLinesEx(box, 5, BLACK);
	DrawLineEx({ box.x + 300, box.y }, { box.x + 300, box.y + box.height }, 5, BLACK);
	int appY = box.y - fontSize + 12;
	int maxApp = min(appPage * maxAppInPage, (int)apps.size());
	Rectangle startBox = { box.x + 10, box.y - fontSize + 8, 135, fontSize + 10 };
	int startX = startBox.x + (startBox.width - MeasureText("Start", fontSize)) / 2;
	Rectangle stopBox = { startBox.x + startBox.width + 10, startBox.y, startBox.width, startBox.height };
	int stopX = stopBox.x + (stopBox.width - MeasureText("Stop", fontSize)) / 2;

	for (int i = (appPage - 1) * maxAppInPage; i < maxApp; i++) {
		appY += fontSize + 20;
		DrawText(apps[i].first.c_str(), box.x + 310, appY, fontSize, BLACK);
		//Start
		startBox.y += startBox.height + 10;
		DrawRectangleRec(startBox, GREEN);
		DrawText("Start", startX, appY, fontSize, BLACK);
		mousePosition = GetMousePosition();
		if (CheckCollisionPointRec(mousePosition, startBox)) {
			DrawRectangleRec(startBox, GRAY);
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				userInput = "12;" + to_string(clientSelectedCount) + ';';
				for (int i = 0; i < clientList.size(); i++) {
					if (!clientSelected[i]) continue;
					userInput += clientList[i] + ";";
				}
				userInput += apps[i].second + ";";
				isUserInputChanged = true;
				isWaiting = true;
			}
		}
		//Stop
		stopBox.y += stopBox.height + 10;
		DrawRectangleRec(stopBox, RED);
		DrawText("Stop", stopX, appY, fontSize, BLACK);
		if (CheckCollisionPointRec(mousePosition, stopBox)) {
			DrawRectangleRec(stopBox, GRAY);
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				userInput = "13;" + to_string(clientSelectedCount) + ';';
				for (int i = 0; i < clientList.size(); i++) {
					if (!clientSelected[i]) continue;
					userInput += clientList[i] + ";";
				}
				userInput += apps[i].second + ";";
				isUserInputChanged = true;
				isWaiting = true;
			}
		}
	}

	//Arrow
	Vector2 v1 = { 40, 495 };
	Vector2 v2 = { 70, 510 };
	Vector2 v3 = { 70, 480 };
	DrawTriangle(v1, v2, v3, GRAY);
	if (CheckCollisionPointTriangle(mousePosition, v1, v2, v3)) {
		DrawTriangle(v1, v2, v3, DARKGRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			appPage--;
			if (appPage < 1) {
				appPage = maxAppPage;
			}
		}
	}
	v1 = { 80, 480 };
	v2 = { 80, 510 };
	v3 = { 110, 495 };
	DrawTriangle(v1, v2, v3, GRAY);
	if (CheckCollisionPointTriangle(mousePosition, v1, v2, v3)) {
		DrawTriangle(v1, v2, v3, DARKGRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			appPage++;
			if (appPage > maxAppPage) {
				appPage = 1;
			}
		}
	}
	//Draw waiting animation
	if (isWaiting) DrawWaitingAnimation();
	else if (isShowFailNotification) {
		DrawFailNotification();
	}
	else if (isShowSuccessNotification) {
		DrawSuccessNotification();
	}
}
void DrawListServiceWindow() { //Change variable to list service
	Vector2 mousePosition = GetMousePosition();
	if (isWaiting) {
		mousePosition = { -1, -1 };
		if (filesystem::last_write_time(SystemPath) != modifiedTime) {
			this_thread::sleep_for(std::chrono::duration<double>(0.1));
			modifiedTime = filesystem::last_write_time(SystemPath);
			fstream f(SystemPath, ios::in);
			string query;
			getline(f, query, ';');
			string y;
			getline(f, y, ';');
			if (y == "Y") {
				isShowSuccessNotification = true;
				successNotificationQuery = stoi(query);
			}
			else {
				string size;
				getline(f, size, ';');
				int n = stoi(size);
				string clientID;
				failNoti = "Execution failed for client IDs:";
				for (int i = 0; i < n; i++) {
					getline(f, clientID, ';');
					failNoti += " " + clientID;
				}
				isShowFailNotification = true;
			}
			f.close();
			isWaiting = false;
		}
	}
	if (isShowFailNotification || isShowSuccessNotification) {
		mousePosition = { -1, -1 };
	}
	//Back button
	Rectangle backButton = { 0, 0, textures[2].width, textures[2].height };
	DrawTexture(textures[2], 0, 0, WHITE);
	if (CheckCollisionPointRec(mousePosition, backButton)) {
		DrawRectangleRec(backButton, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			isWaiting = false;
			Draw = DrawAdminWindow;
		}
	}
	//Title
	DrawText("LIST SERVICES", (SCREENWIDTH - MeasureText("LIST SERVICES", 80)) / 2, 100, 80, BLACK);
	//Apps
	Rectangle box = { 150, 200, 1200, 650 }; //x, y, width, height
	DrawRectangleLinesEx(box, 5, BLACK);
	DrawLineEx({ box.x + 300, box.y }, { box.x + 300, box.y + box.height }, 5, BLACK);
	int appY = box.y - fontSize + 12;
	int maxApp = min(appPage * maxAppInPage, (int)apps.size());
	Rectangle startBox = { box.x + 10, box.y - fontSize + 8, 135, fontSize + 10 };
	int startX = startBox.x + (startBox.width - MeasureText("Start", fontSize)) / 2;
	Rectangle stopBox = { startBox.x + startBox.width + 10, startBox.y, startBox.width, startBox.height };
	int stopX = stopBox.x + (stopBox.width - MeasureText("Stop", fontSize)) / 2;

	for (int i = (appPage - 1) * maxAppInPage; i < maxApp; i++) {
		appY += fontSize + 20;
		DrawText(apps[i].first.c_str(), box.x + 310, appY, fontSize, BLACK);
		//Start
		startBox.y += startBox.height + 10;
		DrawRectangleRec(startBox, GREEN);
		DrawText("Start", startX, appY, fontSize, BLACK);
		mousePosition = GetMousePosition();
		if (CheckCollisionPointRec(mousePosition, startBox)) {
			DrawRectangleRec(startBox, GRAY);
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				userInput = "15;" + to_string(clientSelectedCount) + ';';
				for (int i = 0; i < clientList.size(); i++) {
					if (!clientSelected[i]) continue;
					userInput += clientList[i] + ";";
				}
				userInput += apps[i].second + ";";
				isUserInputChanged = true;
				isWaiting = true;
			}
		}
		//Stop
		stopBox.y += stopBox.height + 10;
		DrawRectangleRec(stopBox, RED);
		DrawText("Stop", stopX, appY, fontSize, BLACK);
		if (CheckCollisionPointRec(mousePosition, stopBox)) {
			DrawRectangleRec(stopBox, GRAY);
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				userInput = "16;" + to_string(clientSelectedCount) + ';';
				for (int i = 0; i < clientList.size(); i++) {
					if (!clientSelected[i]) continue;
					userInput += clientList[i] + ";";
				}
				userInput += apps[i].second + ";";
				isUserInputChanged = true;
				isWaiting = true;
			}
		}
	}

	//Arrow
	Vector2 v1 = { 40, 495 };
	Vector2 v2 = { 70, 510 };
	Vector2 v3 = { 70, 480 };
	DrawTriangle(v1, v2, v3, GRAY);
	if (CheckCollisionPointTriangle(mousePosition, v1, v2, v3)) {
		DrawTriangle(v1, v2, v3, DARKGRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			appPage--;
			if (appPage < 1) {
				appPage = maxAppPage;
			}
		}
	}
	v1 = { 80, 480 };
	v2 = { 80, 510 };
	v3 = { 110, 495 };
	DrawTriangle(v1, v2, v3, GRAY);
	if (CheckCollisionPointTriangle(mousePosition, v1, v2, v3)) {
		DrawTriangle(v1, v2, v3, DARKGRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			appPage++;
			if (appPage > maxAppPage) {
				appPage = 1;
			}
		}
	}
	//Draw waiting animation
	if (isWaiting) DrawWaitingAnimation();
	else if (isShowFailNotification) {
		DrawFailNotification();
	}
	else if (isShowSuccessNotification) {
		DrawSuccessNotification();
	}
}