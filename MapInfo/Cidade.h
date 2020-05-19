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
#include "DLLProfessores.h"

class Cidade
{
	BOOL exit;


public:
	Cidade();
	~Cidade();
	BOOL isExit() const;
	void setExit(BOOL val);
};

