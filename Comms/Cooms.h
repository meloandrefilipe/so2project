#pragma once
#include <Windows.h>
#include "Taxista.h"
#include "DLLProfessores.h"

#define DllImport   __declspec( dllimport )
#define DllExport   __declspec( dllexport )

DllExport DWORD SendCar(Taxista* taxista);
DllExport BOOL validatePlate(TCHAR* plate);
DllExport DWORD getMap(Taxista* taxista);