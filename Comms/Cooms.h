#pragma once
#include <Windows.h>
#include "Taxista.h"

#define DllImport   __declspec( dllimport )
#define DllExport   __declspec( dllexport )

DllExport DWORD SendCar(Taxista* taxista);
DllExport BOOL validatePlate(TCHAR* plate);
DllExport DWORD getMap(Taxista* taxista);
DllExport DWORD bufferCircular(Taxista* taxista);