// MapInfoGraphics.cpp : Defines the entry point for the application.
//

#include "MapInfoGraphics.h"


Cidade* cidade;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{


#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);
#endif

    DWORD idGetMapInfoThread, idCloseThread;
    HANDLE getMapInfoThread, closeThread, hEventCanBoot;

    cidade = new Cidade();

    hEventCanBoot = CreateEvent(NULL, TRUE, FALSE, EVENT_BOOT_ALL);
    if (hEventCanBoot == NULL) {
        cidade->dll->log((TCHAR*)TEXT("Não foi possivel criar o evento para encerrar!"), TYPE::ERRO);
        return EXIT_FAILURE;
    }


    WaitForSingleObject(hEventCanBoot, INFINITE);
    CloseHandle(hEventCanBoot);



    getMapInfoThread = CreateThread(NULL, 0, GetMapThread, cidade, 0, &idGetMapInfoThread);
    closeThread = CreateThread(NULL, 0, CloseThread, cidade, 0, &idCloseThread);


    if (getMapInfoThread == NULL) {
        cidade->dll->log((TCHAR*)TEXT("Não foi possivel criar a Thread!"), TYPE::ERRO);
        return EXIT_FAILURE;
    }

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


    WaitForSingleObject(cidade->getHMutexMap(), INFINITE);
    CloseHandle(cidade->getHMutexMap());

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MAPINFOGRAPHICS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MAPINFOGRAPHICS));

    

    // Main message loop:
    while (GetMessage(&cidade->msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(cidade->msg.hwnd, hAccelTable, &cidade->msg))
        {
            TranslateMessage(&cidade->msg);
            DispatchMessage(&cidade->msg);
        }
    }

    WaitForSingleObject(getMapInfoThread, INFINITE);
    WaitForSingleObject(closeThread, INFINITE);

    int ret = (int)cidade->msg.wParam;
    CloseHandle(getMapInfoThread);
    CloseHandle(hEventCanBoot);
    CloseHandle(closeThread);
    delete cidade;

    return ret;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAPINFOGRAPHICS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = cidade->color_building;
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MAPINFOGRAPHICS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    PAINTSTRUCT ps;
    HDC hdc;
    switch (message){
    case WM_COMMAND:{
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_LCARS:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_LCARS), hWnd, ListCars);
                break;
            case IDM_LCLIENTS:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_LCLIENTS), hWnd, ListClients);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            hdc = BeginPaint(hWnd, &ps);
            CityPaint(hWnd, hdc);
            DeleteDC(hdc);
            EndPaint(hWnd, &ps);
        }
        break;
    case  WM_LBUTTONDOWN:
        hdc = GetDC(hWnd);

        MouseClick(hWnd, hdc, lParam);

        DeleteDC(hdc);
        break;
    case WM_MOUSEMOVE:
            TRACKMOUSEEVENT tracking;
            tracking.cbSize = sizeof(TRACKMOUSEEVENT);
            tracking.dwFlags = TME_HOVER | TME_LEAVE;
            tracking.hwndTrack = hWnd;
            tracking.dwHoverTime = 1;
            TrackMouseEvent(&tracking);
        break;
    case WM_MOUSEHOVER:
        hdc = GetDC(hWnd);

        MouseHover(hWnd, hdc, lParam);

        DeleteDC(hdc);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
LRESULT CALLBACK ListCars(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
    int i;
    HWND hwndList;
    switch (messg) {
    case WM_INITDIALOG:

        hwndList = GetDlgItem(hWnd, IDC_LCARS);
        SendMessage(hwndList, LB_RESETCONTENT, 0, 0);

        for (i = 0; i < cidade->getCars().size(); i++) {
            SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)cidade->getCars()[i]->getPlate());
        }
        break;

    case WM_COMMAND:
        //if (LOWORD(wParam) == IDC_LCARS) {
        //    switch (HIWORD(wParam)) {
        //    case LBN_DBLCLK:
        //        hwndList = GetDlgItem(hWnd, IDC_LCARS);
        //        i = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0);
        //        MessageBox(hWnd, cidade->getCars()[i]->getPlate(), _T("Valor selecionado"), MB_OK | MB_ICONINFORMATION);
        //        break;
        //    }
        //}
        break;
    case WM_CLOSE:
        EndDialog(hWnd, 0);
        return TRUE;
    }
    return FALSE;
}

LRESULT CALLBACK ListClients(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
    HWND hwndList;
    switch (messg) {
    case WM_INITDIALOG:

        hwndList = GetDlgItem(hWnd, IDC_LCLIENTS);
        SendMessage(hwndList, LB_RESETCONTENT, 0, 0);

        for (int i = 0; i < cidade->getClients().size(); i++) {
            SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)cidade->getClients()[i]->getId());
        }
        break;

    case WM_COMMAND:
        //if (LOWORD(wParam) == IDC_LCLIENTS) {
        //    switch (HIWORD(wParam)) {
        //    case LBN_DBLCLK:
        //        hwndList = GetDlgItem(hWnd, IDC_LCLIENTS);
        //        selected = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0);
        //        MessageBox(hWnd, cidade->getClients()[selected]->getId(), _T("Valor selecionado"), MB_OK | MB_ICONINFORMATION);
        //        break;
        //    }
        //}
        break;
    case WM_CLOSE:
        EndDialog(hWnd, 0);
        return TRUE;
    }
    return FALSE;
}
// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void MouseClick(HWND hWnd, HDC hdc, LPARAM lParam) {
    int mouse_x = GET_X_LPARAM(lParam);
    int mouse_y = GET_Y_LPARAM(lParam);
    int col = 0;
    int row = 0;

    LPRECT lpRect = new RECT;
    GetClientRect(hWnd, lpRect);

    int rows = cidade->getRows();
    int cols = cidade->getCols();
    int vsize = lpRect->bottom / rows;
    int hsize = lpRect->right / cols;

    vector< Node*> nodes = cidade->town->getNodes();


    for (int i = 0; i < nodes.size(); i++){
        if (mouse_x >= nodes[i]->getCol() * hsize
            && mouse_x <= hsize + nodes[i]->getCol() * hsize
            && mouse_y >= nodes[i]->getRow() * vsize
            && mouse_y <= nodes[i]->getRow() * vsize + vsize) {
            col = nodes[i]->getCol();
            row = nodes[i]->getRow();
            break;
        }
    }
    Passageiro* passageiro = cidade->getPassageiroAt(row, col);
    if (passageiro != nullptr) {
        tstringstream msg;
        msg << "Passageiro: " << passageiro->getId() << endl;
        msg << "Posição atual: "<< passageiro->getRow() << ", " << passageiro->getCol() << endl;
        if (passageiro->getDestRow() >= 0 && passageiro->getDestCol() >= 0) {
            msg << "Posição destino: " << passageiro->getDestRow() << ", " << passageiro->getDestCol() << endl;
        }
        if (_tcscmp(passageiro->getPlate(), TEXT("")) != 0) {
            msg << "Taxi destinado: " << passageiro->getPlate() << endl;
        }
        MessageBox(hWnd, msg.str().c_str(), TEXT("PASSAGEIRO"), MB_OK);
    }
 
}
void MouseHover(HWND hWnd, HDC hdc, LPARAM lParam) {
    int mouse_x = GET_X_LPARAM(lParam);
    int mouse_y = GET_Y_LPARAM(lParam);
    int col = 0;
    int row = 0;

    LPRECT lpRect = new RECT;
    GetClientRect(hWnd, lpRect);

    int rows = cidade->getRows();
    int cols = cidade->getCols();
    int vsize = lpRect->bottom / rows;
    int hsize = lpRect->right / cols;

    vector< Node*> nodes = cidade->town->getNodes();


    for (int i = 0; i < nodes.size(); i++) {
        if (mouse_x >= nodes[i]->getCol() * hsize
            && mouse_x <= hsize + nodes[i]->getCol() * hsize
            && mouse_y >= nodes[i]->getRow() * vsize
            && mouse_y <= nodes[i]->getRow() * vsize + vsize) {
            col = nodes[i]->getCol();
            row = nodes[i]->getRow();
            break;
        }
    }
    Car* car = cidade->getCarAt(row, col);
    if (car != nullptr) {
        tstringstream msg;
        msg << "Matricula: "<< car->getPlate() << endl;
        msg << "Posição atual: " << car->getRow() << ", " << car->getCol() << endl;
        if (_tcscmp(car->getClient(), TEXT("")) != 0) {
            msg << "Passageiro destinado: " << car->getClient() << endl;
        }
        MessageBox(hWnd, msg.str().c_str(), TEXT("TÁXI"), MB_OK);
    }

}
void CityPaint(HWND hWnd,HDC hdc) {
    HDC buffer;
    HBITMAP Membitmap;
    LPRECT lpRect = new RECT;
    GetClientRect(hWnd, lpRect);

    int win_width = lpRect->right - lpRect->left ;
    int win_height = lpRect->bottom - lpRect->top;

    buffer = CreateCompatibleDC(hdc);
    Membitmap = CreateCompatibleBitmap(hdc, win_width, win_height);
    SelectObject(buffer, Membitmap);

    RECT rect;
    int rows = cidade->getRows();
    int cols = cidade->getCols();
    int vsize = lpRect->bottom / rows;
    int hsize = lpRect->right / cols;


    vector< Node*> nodes = cidade->town->getNodes();

    for (int i = 0; i < nodes.size(); i++)
    {
        SetRect(&rect, nodes[i]->getCol() * hsize, nodes[i]->getRow() * vsize, hsize + nodes[i]->getCol() * hsize, nodes[i]->getRow() * vsize + vsize);
        if (nodes[i]->isRoad()) {
            FillRect(buffer, &rect, cidade->color_street);
        }else{
            FillRect(buffer, &rect, cidade->color_building);
        }
    }
    for (int i = 0; i < cidade->getClients().size(); i++){
        int c = cidade->getClients()[i]->getCol();
        int r = cidade->getClients()[i]->getRow();
        SetRect(&rect, c * hsize, r * vsize, hsize + c * hsize, r * vsize + vsize);
        FillRect(buffer, &rect, cidade->color_person);
        if (_tcscmp(cidade->getClients()[i]->getPlate(), TEXT("")) != 0) {
            FrameRect(buffer, &rect, cidade->border_client_car);
        }
    }

    for (int i = 0; i < cidade->getCars().size(); i++) {
        int c = cidade->getCars()[i]->getCol();
        int r = cidade->getCars()[i]->getRow();
        SetRect(&rect, c * hsize, r * vsize, hsize + c * hsize, r * vsize + vsize);

        FillRect(buffer, &rect, CreateSolidBrush(RGB(cidade->getCars()[i]->getColor().red, cidade->getCars()[i]->getColor().green, cidade->getCars()[i]->getColor().blue)));
        if (_tcscmp(cidade->getCars()[i]->getClient(), TEXT("")) != 0) {
            FrameRect(buffer, &rect, cidade->border_car_ocup);
        }
        else {
            FrameRect(buffer, &rect, cidade->border_car_free);
        }
    }
    BitBlt(hdc, 0, 0, win_width, win_height, buffer, 0, 0, SRCCOPY);
    
    DeleteObject(Membitmap);
    DeleteDC(buffer);
}

DWORD WINAPI GetMapThread(LPVOID lpParam) {
    HANDLE hFileMapping, hFileMappingMap, sCanRead, sCanSize, sCanWrite;
    MAPINFODATA* pBuf;
    LPCTSTR pMap;
    Cidade* cidade = (Cidade*)lpParam;

    sCanRead = CreateSemaphore(NULL, 0, 1, SEMAPHORE_MAPINFO_READ);
    sCanWrite = CreateSemaphore(NULL, 0, 1, SEMAPHORE_MAPINFO_WRITE);
    sCanSize = CreateSemaphore(NULL, 0, 1, SEMAPHORE_MAPINFO_SIZE);

    if (sCanSize == NULL || sCanRead == NULL || sCanWrite == NULL) {
        cidade->dll->log((TCHAR*)TEXT("Não foi possivel criar o semafro sCanWrite ou sCanRead!"), TYPE::ERRO);
        CloseHandle(sCanRead);
        CloseHandle(sCanSize);
        CloseHandle(sCanWrite);
        return EXIT_FAILURE;
    }
    cidade->dll->regist((TCHAR*)SEMAPHORE_MAPINFO_WRITE, 3);
    cidade->dll->regist((TCHAR*)SEMAPHORE_MAPINFO_READ, 3);

    hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHAREDMEMORY_ZONE_MAPSIZE);
    hFileMappingMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHAREDMEMORY_ZONE_MAPINFO);

    if (hFileMapping == NULL || hFileMappingMap == NULL) {
        cidade->dll->log((TCHAR*)TEXT("Não foi possivel criar o file mapping SHAREDMEMORY_ZONE_MAPINFO!"), TYPE::ERRO);
        CloseHandle(sCanRead);
        CloseHandle(sCanSize);
        CloseHandle(sCanWrite);
        CloseHandle(hFileMapping);
        CloseHandle(hFileMappingMap);
        return EXIT_FAILURE;
    }

    pBuf = (MAPINFODATA*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(MAPINFODATA));
    if (pBuf == NULL) {
        cidade->dll->log((TCHAR*)TEXT("Não foi possivel mapear o ficheiro!"), TYPE::ERRO);
        CloseHandle(sCanRead);
        CloseHandle(sCanSize);
        CloseHandle(sCanWrite);
        CloseHandle(hFileMapping);
        CloseHandle(hFileMappingMap);
        return EXIT_FAILURE;
    }


    WaitForSingleObject(sCanSize, INFINITE);
    cidade->update(pBuf);
    pMap = (LPTSTR)MapViewOfFile(hFileMappingMap, FILE_MAP_ALL_ACCESS, 0, 0, cidade->getMapSize());
    if (pMap == NULL) {
        cidade->dll->log((TCHAR*)TEXT("Não foi possivel mapear o ficheiro!"), TYPE::ERRO);
        CloseHandle(sCanRead);
        CloseHandle(sCanSize);
        CloseHandle(sCanWrite);
        CloseHandle(hFileMapping);
        CloseHandle(hFileMappingMap);
        return EXIT_FAILURE;
    }
    cidade->dll->regist((TCHAR*)SHAREDMEMORY_ZONE_MAPINFO, 7);

    cidade->setMap((TCHAR*)pMap);
    SetEvent(cidade->getHMutexMap());
    while (!cidade->isExit()) {
        WaitForSingleObject(sCanRead, INFINITE);
        cidade->update(pBuf);
        RedrawWindow(cidade->msg.hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
        ReleaseSemaphore(sCanWrite, 1, NULL);
    }
    CloseHandle(sCanRead);
    CloseHandle(sCanSize);
    CloseHandle(sCanWrite);
    UnmapViewOfFile(pBuf);
    UnmapViewOfFile(pMap);
    CloseHandle(hFileMapping);
    CloseHandle(hFileMappingMap);
    return EXIT_SUCCESS;
}

DWORD WINAPI CloseThread(LPVOID lpParam) {

    HANDLE hEventClose;
    Cidade* cidade = (Cidade*)lpParam;
    WaitableTimer* wt = new WaitableTimer(WAIT_ONE_SECOND * 5);
    hEventClose = CreateEvent(NULL, TRUE, FALSE, EVENT_CLOSE_ALL);
    if (hEventClose == NULL) {
        cidade->dll->log((TCHAR*)TEXT("Não foi possivel criar o evento!"), TYPE::ERRO);
    }
    WaitForSingleObject(hEventClose, INFINITE);

    cidade->dll->regist((TCHAR*)EVENT_CLOSE_ALL, 4);

    _tprintf(TEXT("\n[WARNING] A Central fechou!\n"));
    cidade->setExit(true);
    _tprintf(TEXT("[SHUTDOWN] A Sair...\n"));
    CloseHandle(hEventClose);
    wt->wait();
    delete wt;
    delete cidade;
    exit(EXIT_SUCCESS);
}
