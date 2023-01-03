#pragma once
#include "framework.h"

#define msgbox(hwnd, x) MessageBoxEx(hwnd, x, L"!!!", MB_OK, 0);

enum InjectionMethod {
	LOADLIBRARY,
	MANUALMAP
};


class Injector {
public:
	Injector();
	~Injector();
	std::wstring SetTargetProcess(std::wstring processName);
	const std::wstring GetTargetProcess() const;
	bool FindProcess(const std::wstring processName);
	bool IsProcessValid() const;
	bool Inject(const InjectionMethod ijmethod);
	bool CreateConsole(const HWND& hwnd);
	void DestroyConsole();
	bool IsConsoleEnabled() const;
	const std::wstring SetDllPath(const std::wstring path);
	const std::wstring& GetDllPath() const;

	template <typename T>
	std::wostream& Log(const T msg) const;
	
private:
	
	std::wstring _targetProcess;
	std::wstring _dllPath;
	bool _foundProcess;
	bool _enableConsole;
	FILE* _fileOut;
	std::uintptr_t _processID;
	HANDLE _processHandle;
	HANDLE _threadHandle;
	void* _dllData;
};

