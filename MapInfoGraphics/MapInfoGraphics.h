#pragma once

#include "resource.h"
#include "Core.h"
#include <windowsx.h>
#include "Cidade.h"
#include <iostream>
#include <functional>
#include <string>



#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


// funcs
void CityPaint(HWND hWnd,HDC hdc);
void MouseClick(HWND hWnd, HDC hdc, LPARAM lParam);
void MouseHover(HWND hWnd, HDC hdc, LPARAM lParam);
LRESULT CALLBACK ListCars(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ListClients(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);

//Threads
DWORD WINAPI GetMapThread(LPVOID lpParam);
DWORD WINAPI CloseThread(LPVOID lpParam);