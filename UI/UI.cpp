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
////////////////////////////////////////////////////////////////////////// Confirmation
string code = "";
string codeInput = "";
////////////////////////////////////////////////////////////////////////// ADMIN
bool showUserBox = false;
vector<string> adminMainQueryNames = { "Apps", "Processes", "Shutdown", "Reset", "Copy", "Delete", "Screenshot", "Keylogger", "Webcam" };
vector<string> userQueries = { "Change Password", "Change Email", "Log Out" };
vector<string> clientList = { "C35", "C231", "C23", "C123", "C1234", "C231", "C23", "C123", "C1234", "C231", "C23", "C123", "C1234", "C231", "C23", "C123", "C123", "C1234", "C231", "C23", "C12321412" };
vector<bool> clientSelected(clientList.size(), true);
vector<string> adminMainQueryNumbers = { "11", "14", "17", "18", "20", "21", "22", "23", "25" };
int clientSelectedCount = clientList.size();
bool isGettingFilePath = false;
string CopyOrDelete = "";
string filePath = "";
bool hasFailQuery = false;
vector<string> failClientList = {};
////////////////////////////////////////////////////////////////////////// 
vector<string> apps = { "App1", "App2", "App3", "App4", "App5", "App6", "App7", "App8", "App9", "App10", "App11", "App12", "App13", "App14", "App15", "App16", "App17", "App18", "App19", "App20" };
int maxAppInPage = 10;
int appPage = 1;
int maxAppPage = (apps.size() + maxAppInPage - 1) / maxAppInPage;
int startAppY = 210;
int startAppX = 150;
int appMargin = 10;
/////////////////////////////////////////////////////////////////////////////////// Animation
int totalFrames = 60;
int columns = 5;
int currentFrame = 0;
float frameWidth = 300;
float frameHeight = 300;
float frameTime = 0.05f;
float timer = 0.0f;
Rectangle sourceRect = { 0.0f, 0.0f, frameWidth, frameHeight };
vector<Texture2D> animations;
///////////////////////////////////////////////////////////////////////////////////////////
function<void()> Draw = DrawStartWindow;
string userInput;
bool isUserInputChanged = false;
filesystem::file_time_type modifiedTime;
///////////////////////////////////////////////////////////////////////////////////////////
void ResetLogin() {
	user = "";
	password = "";
	status = "";
	isTypingUser = true;
}
void LoadListApp() {
	unordered_map<string, int> listApp;
	for (int i = 0; i < clientList.size(); i++) {
		if (!clientSelected[i]) continue;
		cout << ComPath + "List/ListApp" + clientList[i];
		fstream file(ComPath + "List/ListApp" + clientList[i], ios::binary | ios::in);
		string app;
		while (!file.eof()) {
			getline(file, app);
			if (app.empty()) continue;
			app.pop_back();
			listApp[app]++;
		}
		file.close();
	}
	apps.clear();
	for (auto& app : listApp) {
		if (app.second != clientSelectedCount) continue;
		apps.push_back(app.first);
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
void SetClientList(fstream& f) {
	clientList.clear();
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
			Draw = DrawServerClientWindow;
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
			Draw = DrawLoginWindow;
			modifiedTime = filesystem::last_write_time(SystemPath);
		}
	}
	DrawRectangleRec(box, color);
	DrawTexture(textures[1], box.x + (box.width - textures[0].width) / 2, box.y + 30, WHITE);
	DrawText(key.c_str(), box.x + (box.width - MeasureText(key.c_str(), fontSize)) / 2,
		box.y + (box.height + 30 + textures[0].height - fontSize) / 2, fontSize, BLACK);
}
void DrawLoginWindow() {
	Vector2 mousePosition = GetMousePosition();
	int input = GetKeyPressed();
	if (isWaiting) {
		//Animation waiting
		mousePosition = { -1, -1 };
		input = 0;
		if (filesystem::last_write_time(SystemPath) != modifiedTime) {
			this_thread::sleep_for(std::chrono::duration<double>(0.1));
			fstream f(SystemPath, ios::in);
			string y;
			getline(f, y, ';');
			if (y == "Y") { //Y;size;clientID1;clientID2;...;
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
			isWaiting = true;
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
			//Mouse on forgot password button
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
}
void DrawForgotPasswordWindow() {
	//Back button
	Rectangle backButton = { 0, 0, textures[2].width, textures[2].height };
	DrawTexture(textures[2], 0, 0, WHITE);
	Vector2 mousePosition = GetMousePosition();
	if (CheckCollisionPointRec(mousePosition, backButton)) {
		DrawRectangleRec(backButton, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			//Mouse on back buttond
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
		cursorY -= 150;
	}
	else {
		cursorX += MeasureText(password.c_str(), fontSize);
	}
	DrawCursor(cursorX, cursorY);
	DrawText(user.c_str(), box.x + 10, box.y + (box.height - fontSize) / 2 - 150, fontSize, BLACK);
	DrawText(password.c_str(), box.x + 10, box.y + (box.height - fontSize) / 2, fontSize, BLACK);

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
			//Mouse on forgot password button
		}
	}
	//Draw status
	DrawText(status.c_str(), (SCREENWIDTH - MeasureText(status.c_str(), fontSize)) / 2, box.y + 150, fontSize, RED);
}
void DrawSetPasswordWindow() {
	//Back button
	Rectangle backButton = { 0, 0, textures[2].width, textures[2].height };
	DrawTexture(textures[2], 0, 0, WHITE);
	Vector2 mousePosition = GetMousePosition();
	if (CheckCollisionPointRec(mousePosition, backButton)) {
		DrawRectangleRec(backButton, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			//Mouse on back buttond
		}
	}
	//Title
	int titleFontSize = 80;
	string title = "FORGOT PASSWORD";
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
			//Mouse on forgot password button
		}
	}
	//Draw status
	DrawText(status.c_str(), (SCREENWIDTH - MeasureText(status.c_str(), fontSize)) / 2, box.y + 150, fontSize, RED);
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
			userInput = CopyOrDelete + ";" + filePath + ";";
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
void DrawQueryNotification() {

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
			if (query == "11") {
				LoadListApp();
				Draw = DrawListAppWindow;
			}
			else if (query == "14") {
				//Change scene
			}
			string y;
			getline(f, y, ';');
			if (y == "Y") {

			}
			else {
				string size;
				getline(f, size, ';');
				int n = stoi(size);
				string clientID;
			}
			f.close();
			isWaiting = false;
		}
	}
	if (isGettingFilePath) {
		mousePosition = { -1, -1 };
	}
	//user button
	Color color = WHITE;
	Rectangle box = { 0, 0, textures[12].width, textures[12].height };
	DrawTexture(textures[12], 0, 0, color);
	if (CheckCollisionPointRec(mousePosition, box)) {
		DrawRectangleRec(box, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			showUserBox = !showUserBox;
		}
	}
	//Client icon
	string label = "Clients";
	box = { 150, 10, 180, 180 }; //x, y, width, height
	DrawRectangle(box.x, box.y, box.width, box.height, color);
	DrawTexture(textures[13], box.x + (box.width - textures[13].width) / 2, box.y + 10, WHITE);
	DrawText(label.c_str(), box.x + (box.width - MeasureText(label.c_str(), queryFontSize)) / 2,
		box.y + (box.height + 10 + textures[13].height - queryFontSize) / 2, queryFontSize, BLACK);
	box = { 340, 10, 1000, 180 };
	DrawRectangleRec(box, color);
	//client list
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
	//User box
	if (showUserBox) {
		box = { 0, 0, (float)textures[12].width, (float)textures[12].height };
		box.width = 300;
		for (int i = 0; i < userQueries.size(); i++) {
			box.y += box.height;
			if (CheckCollisionPointRec(mousePosition, box)) {
				DrawRectangleRec(box, selectedColor);
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

				}
			}
			DrawRectangle(box.x, box.y, box.width, box.height, color);
			DrawRectangleLinesEx(box, 2, BLACK);
			DrawText(userQueries[i].c_str(), box.x + (box.width - MeasureText(userQueries[i].c_str(), queryFontSize)) / 2,
				box.y + (box.height - queryFontSize) / 2, queryFontSize, BLACK);
		}
	}
	//Queries
	int maxBoxInRow = 3;
	int startX = 330;
	box = { 330, 0, 180, 180 };
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
	if (isGettingFilePath) {
		DrawRectangle(0, 0, SCREENWIDTH, SCREENHEIGHT, { 0, 0, 0, 200 });
		DrawGetFilePathWindow();
	}
}
void DrawServerClientWindow() {
	//Back button
	Rectangle backButton = { 0, 0, textures[2].width, textures[2].height };
	DrawTexture(textures[2], 0, 0, WHITE);
	Vector2 mousePosition = GetMousePosition();
	if (CheckCollisionPointRec(mousePosition, backButton)) {
		DrawRectangleRec(backButton, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			//Mouse on back buttond
		}
	}
	//Title
	DrawText("SERVER - CLIENT", (SCREENWIDTH - MeasureText("SERVER - CLIENT", 80)) / 2, 100, 80, BLACK);
	//Text
	string text = "Waiting for request from admin...";
	DrawText(text.c_str(), (SCREENWIDTH - MeasureText(text.c_str(), fontSize)) / 2, 400, fontSize, BLACK);
	//Empty Animation
	timer += GetFrameTime();
	if (timer >= frameTime) {
		timer = 0.0f;
		currentFrame = (currentFrame + 1) % totalFrames;
		sourceRect.x = currentFrame % columns * frameWidth;
		sourceRect.y = currentFrame / columns * frameHeight;
	}
	DrawTextureRec(animations[0], sourceRect, { 600, 500 }, WHITE);
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
			if (query == "12") {

			}
			string y;
			getline(f, y, ';');
			if (y == "Y") {

			}
			else {
				string size;
				getline(f, size, ';');
				int n = stoi(size);
				string clientID;
			}
			f.close();
			isWaiting = false;
		}
	}
	//Back button
	Rectangle backButton = { 0, 0, textures[2].width, textures[2].height };
	DrawTexture(textures[2], 0, 0, WHITE);
	if (CheckCollisionPointRec(mousePosition, backButton)) {
		DrawRectangleRec(backButton, GRAY);
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			//Mouse on back buttond
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
		DrawText(apps[i].c_str(), box.x + 310, appY, fontSize, BLACK);
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
				userInput += apps[i] + ";";
				isUserInputChanged = true;
			}
		}
		//Stop
		stopBox.y += stopBox.height + 10;
		DrawRectangleRec(stopBox, RED);
		DrawText("Stop", stopX, appY, fontSize, BLACK);
		if (CheckCollisionPointRec(mousePosition, stopBox)) {
			DrawRectangleRec(stopBox, GRAY);
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				//Mouse on app
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
}