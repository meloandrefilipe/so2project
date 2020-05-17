#include "Core.h"

#pragma once
class WaitableTimer
{
	LARGE_INTEGER liDueTime;
	HANDLE hTimer;
	HMODULE hDLL;

public: 
	WaitableTimer(LONGLONG time);
	~WaitableTimer();
};

