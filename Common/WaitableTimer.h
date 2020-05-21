#include "Core.h"
#include "DLLProfessores.h"

static int WT = 0;
#pragma once
class WaitableTimer
{
	LARGE_INTEGER liDueTime;
	HANDLE hTimer;
	DLLProfessores* dll;
	TCHAR* name;

public: 
	WaitableTimer(LONGLONG time);
	~WaitableTimer();
	DWORD wait();
};

