#include "Core.h"

enum class TYPE { ERRO, WARNING, NOTIFICATION };

class DLLProfessores
{
	HMODULE hDLL;
	FuncRegister funcRegister;
	FuncLog funcLog;

public:
	DLLProfessores();
	~DLLProfessores();
	DWORD regist(TCHAR* message, int code);
	DWORD log(TCHAR* message, TYPE type);
};

