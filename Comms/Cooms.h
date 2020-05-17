#pragma once
#include <Windows.h>
#include "Core.h"
#include "Car.h"

#define DllImport   __declspec( dllimport )
#define DllExport   __declspec( dllexport )

DllExport DWORD SendCar(Car * car);
DllExport BOOL validatePlate(TCHAR* plate);