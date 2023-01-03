#include "Injector.h"

#define Inject_button 1
#define AutoInject_checkbox 9
#define Console_checkbox 10
#define Select_button 5
#define Browse_button 8

std::wstring GetHwndText(const HWND& hwnd) {
    const int size = GetWindowTextLength(hwnd);
    std::wstring buffer = L"";
    buffer.reserve(size);
    TCHAR* tempBuffer = new TCHAR[size + 1];
    GetWindowTextW(hwnd, tempBuffer, size + 1);
    for (int i = 0; i < size + 1; i++) {
        buffer += tempBuffer[i];
    }
    delete[] tempBuffer;

    return buffer;
}

const std::wstring ObfuscateWndTitle(const UINT length = 16) {
    std::random_device rnd;
    const std::wstring& acceptedChars = L"abcdefghijklmnopqrstuvwxyz1234567890.-_";
    std::uniform_int_distribution<size_t> range(0, acceptedChars.size() - 1);

    std::wstring newString = L"";
    newString.reserve(length);
    for (UINT i = 0; i < length; i++) {
        newString += acceptedChars[range(rnd)];
    }

    return newString;
}

HWND baseHwnd, injectButton, label1, label2 ,
        radioButton1, inputField, inputField2,
    findButton, groupBox, checkBox1, ofdButton,
    checkBox2 = 0;

bool canInject = false;
bool payloadSelected = false;
bool cbAutoInject, cbBrowse = false;

std::unique_ptr<Injector> injector = std::make_unique<Injector>();



LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


std::wstring OpenFile(const HWND& hwnd) {
    OPENFILENAMEW ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    TCHAR strBuffer[MAX_PATH] = L"";

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = strBuffer;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"Dynamic Link Library (*.dll)\0*.dll\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    GetOpenFileNameW(&ofn);
    injector->SetDllPath(strBuffer);

    return strBuffer;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    

#pragma region Widgets
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"className";
    RegisterClass(&wc);




    baseHwnd = CreateWindowEx(
        0,
        L"classname",
        ObfuscateWndTitle().c_str(),
        WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        500,
        500,
        0,
        0,
        hInstance,
        nullptr
    );

    baseHwnd ? ShowWindow(baseHwnd, nCmdShow) : 0;





    injectButton = CreateWindowEx(
        0,
        L"BUTTON",
        L"Inject",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | SS_CENTER,
        200,
        300,
        100,
        30,
        baseHwnd,
        (HMENU)1,
        hInstance,
        NULL
    );
    injectButton ? ShowWindow(injectButton, nCmdShow) : 0;




    label1 = CreateWindowEx(
        0,
        L"STATIC",
        L"Target Process:",
        WS_CHILD | WS_VISIBLE | SS_SIMPLE | SS_CENTER,
        160,
        20,
        200,
        100,
        baseHwnd,
        NULL,
        hInstance,
        NULL
    );

    label1 ? ShowWindow(label1, nCmdShow) : 0;


    inputField = CreateWindowEx(
        0,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
        160,
        40,
        150,
        25,
        baseHwnd,
        (HMENU)3,
        hInstance,
        NULL
    );
    inputField ? ShowWindow(inputField, nCmdShow) : 0;


    inputField2 = CreateWindowEx(
        0,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT ,
        160,
        110,
        150,
        25,
        baseHwnd,
        (HMENU)4,
        hInstance,
        NULL
    );
    inputField2 ? ShowWindow(inputField2, nCmdShow) : 0;

    findButton = CreateWindowEx(
        0,
        L"BUTTON",
        L"Select",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | SS_CENTER,
        313,
        40,
        50,
        25,
        baseHwnd,
        (HMENU)5,
        hInstance,
        NULL
    );

    findButton ? ShowWindow(findButton, nCmdShow) : 0;


    groupBox = CreateWindowEx(
        0,
        L"BUTTON",
        L"Settings",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        160,
        190,
        200, //W
        100, // H
        baseHwnd,
        (HMENU)6,
        hInstance,
        NULL
    );
    groupBox ? ShowWindow(groupBox, nCmdShow) : 0;
    
    checkBox1 = CreateWindowEx(
        0,
        L"button",
        L"Auto-Inject",
        WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
        170,
        207,
        115,
        15,
        baseHwnd,
        (HMENU)9,
        hInstance,
        NULL
    );
    checkBox1 ? ShowWindow(checkBox1, nCmdShow) : 0;

    label2 = CreateWindowEx(
        0,
        L"STATIC",
        L"Payload:",
        WS_CHILD | WS_VISIBLE | SS_SIMPLE | SS_CENTER,
        160,
        90,
        200,
        100,
        baseHwnd,
        NULL,
        hInstance,
        NULL
    );

    label2 ? ShowWindow(label2, nCmdShow) : 0;

    ofdButton = CreateWindowEx(
        0,
        L"BUTTON",
        L"Browse...",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | SS_CENTER,
        313,
        110,
        80,
        25,
        baseHwnd,
        (HMENU)8,
        hInstance,
        NULL
    );
    ofdButton ? ShowWindow(ofdButton, nCmdShow) : 0;
    
    checkBox2 = CreateWindowEx(
        0,
        L"button",
        L"Console",
        WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
        170,
        230,
        115,
        15,
        baseHwnd,
        (HMENU)10,
        hInstance,
        NULL
    );
    checkBox2 ? ShowWindow(checkBox2, nCmdShow) : 0;

#pragma endregion

    
    

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg) {

    case (WM_COMMAND): {


        if (LOWORD(wParam) == Select_button && HIWORD(wParam) == BN_CLICKED) { 

            injector->SetTargetProcess(GetHwndText(inputField).c_str());

            if (injector->FindProcess(injector->GetTargetProcess())) {
                injector->Log("Process found.\n");
                
            }

        }

        if (LOWORD(wParam) == Browse_button && HIWORD(wParam) == BN_CLICKED) { 

            if (SetWindowTextW(inputField2, OpenFile(baseHwnd).c_str())) {
                injector->Log("Payload selected.\n");
                payloadSelected = true;
            }
        }

        if (LOWORD(wParam) == Inject_button && HIWORD(wParam) == BN_CLICKED) { 

            if (injector->IsProcessValid() && payloadSelected) {

                if (injector->Inject(LOADLIBRARY)) {

                    injector->Log("Injected payload into target process.\n");
                }
                
            }
            
        }

        if (LOWORD(wParam) == AutoInject_checkbox && HIWORD(wParam) == BN_CLICKED) { 

            SendMessageW(checkBox1, BM_SETCHECK, wParam, NULL);
            cbAutoInject = !cbAutoInject;
            if (SendMessageW(checkBox1, BM_GETSTATE, NULL, NULL) & BST_CHECKED && !cbAutoInject) {
                SendMessageW(checkBox1, BM_SETCHECK, BST_UNCHECKED, NULL);
            }
            

            if (payloadSelected && cbAutoInject) {
                
                do {
                    injector->FindProcess(GetHwndText(inputField).c_str());
                    if (GetAsyncKeyState(VK_ESCAPE)&1) break;
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                } while (!injector->FindProcess(GetHwndText(inputField).c_str()));

                SendMessageW(injectButton, BM_CLICK, wParam, lParam);
                                
            }
        }

        if (LOWORD(wParam) == Console_checkbox && HIWORD(wParam) == BN_CLICKED) {

            SendMessageW(checkBox2, BM_SETCHECK, wParam, NULL);
            cbBrowse = !cbBrowse;
            if (SendMessageW(checkBox2, BM_GETSTATE, NULL, NULL) & BST_CHECKED && !cbBrowse) {
                SendMessageW(checkBox2, BM_SETCHECK, BST_UNCHECKED, NULL);
            }

            if (cbBrowse) {

                if (injector->CreateConsole(hwnd)) {
                    injector->Log("Console created!\n");
                }
            }
            else if (!injector->IsConsoleEnabled() && !cbBrowse) {

                injector->DestroyConsole();
            }
        }
        
        break;
    }

    

    

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        //const HBRUSH hbr = CreateSolidBrush(RGB(15, 15, 15));
        FillRect(hdc, &ps.rcPaint, (HBRUSH)COLOR_BACKGROUND+1);

        EndPaint(hwnd, &ps);
    }
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}



/*
TODO:
Auto Inject multithreading
Thread hijack
manual mapping


*/