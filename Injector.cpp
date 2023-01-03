#include "Injector.h"

Injector::Injector() {
    this->_targetProcess = L"";
    this->_dllPath = L"";
    this->_foundProcess = false;
    this->_enableConsole = false;
    this->_fileOut = nullptr;
    this->_processID = NULL;
    this->_processHandle = nullptr;
    this->_threadHandle = nullptr;
}

Injector::~Injector() {
    if (this->_enableConsole) {
        FreeConsole();
        std::fflush(this->_fileOut);
        fclose(this->_fileOut);
        delete this->_fileOut;
    }

    CloseHandle(this->_processHandle);
    CloseHandle(this->_threadHandle);
    
}

std::wstring Injector::SetTargetProcess(std::wstring processName) {
    return (this->_targetProcess = processName);
}

const std::wstring Injector::GetTargetProcess() const {
    return this->_targetProcess;
}

bool Injector::FindProcess(const std::wstring processName) {

    PROCESSENTRY32W procEntry;
    procEntry.dwSize = sizeof(procEntry);

    HANDLE procHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (procHandle == INVALID_HANDLE_VALUE) {
        this->Log("Failed to snap processes.\n");
        this->_foundProcess = false;
        return false;
    }

    if (Process32FirstW(procHandle, &procEntry)) {

        if (processName.compare(procEntry.szExeFile) != 0) {

            do {
                if (processName.compare(procEntry.szExeFile) == 0) {
                    this->_processID = procEntry.th32ProcessID;
                    this->_foundProcess = true;
                    return true;
                }
            } while (Process32NextW(procHandle, &procEntry));
        }
        else {
            this->_processID = procEntry.th32ProcessID;
            this->_foundProcess = true;
            return true;
        }
    }
    this->Log("Failed to find target process.\n");
    this->_foundProcess = false;
    return false;
}

bool Injector::IsProcessValid() const {
    return this->_foundProcess;
}

bool Injector::Inject(const InjectionMethod ijmethod) {

    switch (ijmethod) {

    case LOADLIBRARY: {

        this->_processHandle = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, NULL, this->_processID);
        
        if (this->_processHandle == INVALID_HANDLE_VALUE) {

            this->Log("Failed to open process handle.\n");
            return false;
        }

    
        void* memAlloc = VirtualAllocEx(this->_processHandle, NULL, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        
        if (!memAlloc) {
            this->Log("Failed to allocate memory in target process.\n");
            return false;
        }

        if (WriteProcessMemory(this->_processHandle, memAlloc, this->_dllPath.c_str(), MAX_PATH, NULL)) {
            this->_threadHandle = CreateRemoteThread(this->_processHandle, nullptr, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryW, memAlloc, NULL, NULL);
            if (!this->_threadHandle) {
                this->Log("Failed to spawn thread in target process.\n");
                return false;
            }
            CloseHandle(this->_threadHandle);
           
            return true;
        }

        break;
    }

    case MANUALMAP: {

        this->_processHandle = OpenProcess(
            PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_READ |PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION,
            NULL, 
            this->_processID
        );

        if (this->_processHandle == INVALID_HANDLE_VALUE) {
            this->Log("Failed to create handle to process.\n");
            return false;
        }

        DWORD bytesRead = 0;
        if (ReadFile(this->_processHandle, this->_dllData, sizeof(this->_dllData), &bytesRead, nullptr)) {
            
            auto FnRtlUserThreadStart = GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlUserThreadStart");
            if (bytesRead && FnRtlUserThreadStart) {
                
            }
        }

        break;
    }
    }
    return false;
}

bool Injector::CreateConsole(const HWND& hwnd) {
    if (!AllocConsole()) {
        MessageBoxW(hwnd, L"Failed to allocate console.", L"Error", NULL);
        return false;
    }

    
    if (freopen_s(&this->_fileOut, "CONOUT$", "w", stdout) != 0) {
        MessageBoxW(hwnd, L"freopen_s was nullptr.", L"Error", NULL);
        return false;
    }

    return true;
}

void Injector::DestroyConsole() {

    FreeConsole();
    std::fflush(this->_fileOut);
    fclose(this->_fileOut);
}

bool Injector::IsConsoleEnabled() const {
    return this->_enableConsole;
}

const std::wstring Injector::SetDllPath(const std::wstring path) {
    return (this->_dllPath = path);
}

const std::wstring& Injector::GetDllPath() const {
    return this->_dllPath;
}

template <typename T>
std::wostream& Injector::Log(const T msg) const {

    auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char buffer[26] = "";
    ctime_s(buffer, sizeof(buffer), &time);
    std::string tempBuffer;

    for (BYTE i = 0; i < sizeof(buffer); i++) {

        tempBuffer += buffer[i];
        if (tempBuffer[i] == '\n') {
            buffer[i] = ' ';
        }
    }
    
    
    return std::wcout << L"[LOG ENTRY] - " << buffer << "- " << msg;
}
