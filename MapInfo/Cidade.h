#pragma once
#include <iostream>
#include <windows.h>
#include <string.h>
#include <tchar.h>
#include <strsafe.h>
#include <fcntl.h>
#include <vector>
#include <io.h>
#include "WaitableTimer.h"

class Cidade
{
	BOOL exit;

public:
	DLLProfessores* dll;
	Cidade();
	~Cidade();
	BOOL isExit() const;
	void setExit(BOOL val);
};

