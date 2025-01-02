#pragma once
#ifndef UI_H
#define UI_H

#include "Header.h"
#include "raylib.h"
extern int SCREENWIDTH;
extern int SCREENHEIGHT;

extern bool showCursor;
extern int fontSize;
extern int queryFontSize;
extern float cursorTimer;
extern Color background;
extern Color selectedColor;

extern vector<Texture2D> textures;
//////////////////////////////////////////////////////////////////////// LOG IN
extern string user;
extern string password;
extern string status;
extern bool isTypingUser;
extern bool showUserBox;
extern bool isWaiting;
////////////////////////////////////////////////////////////////////////// SIGN UP
extern string gmail;
extern bool isTypingGmail;
////////////////////////////////////////////////////////////////////////// Confirmation
extern string code;
extern string codeInput;
////////////////////////////////////////////////////////////////////////// ADMIN
extern vector<string> adminMainQueryNames;
extern vector<string> userQueryNames;
extern vector<string> clientList;
extern vector<bool> clientSelected;
extern string CopyOrDelete;
extern string filePath;
extern bool isGettingFilePath;
extern int clientSelectedCount;
extern int clientMagin;
extern int maxClientInRow;
extern int clientPage;
extern int maxClientPage;
////////////////////////////////////////////////////////////////////////// LIST APP
extern vector<pair<string,string>> apps;
extern int maxAppInPage;
extern int appPage; 
extern int maxAppPage;
extern int startAppY;
extern int startAppX;
extern int appMargin; 
//////////////////////////////////////////////////////////////////////// Animation
extern vector<float> frameTimes;
extern vector<int> totalFrames;
extern vector<int> columns;
extern vector<int> currentFrames;
extern vector<float> frameWidths;
extern vector<float> frameHeights;
extern vector<float> timers;
extern vector<Rectangle> sourceRects;
extern vector<Texture2D> animations;
////////////////////////////////////////////////////////////////////////
extern function<void()> Draw;
extern string userInput;
extern bool isUserInputChanged;
extern filesystem::file_time_type modifiedTime;
////////////////////////////////////////////////////////////////////////
void LoadList(const string mode);
void ResetLogin();
void SetClientList(fstream& f);
void DrawCursor(int x, int y);
void DrawStartWindow();
void DrawLoginWindow();
void DrawForgotPasswordWindow();
void DrawSetPasswordWindow();
void DrawSignUpWindow();
void DrawConfirmationWindow();
void DrawAdminWindow();
void DrawServerClientWindow();
void DrawListAppWindow();
void DrawListServiceWindow();

#endif
