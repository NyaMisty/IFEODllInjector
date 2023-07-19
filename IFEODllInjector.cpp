// IFEODllInjector.cpp : 定义控制台应用程序的入口点。
//
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include <windows.h>
#include <cassert>

void dbg(char *msg, ...) {
	va_list va;
	va_start(va, msg);
	char buf[16384];
	vsprintf(buf, msg, va);
	va_end(va);
	OutputDebugStringA(buf);
}

int wmain(int argc, wchar_t* argv[]) {
	assert(argc > 1);

	wchar_t *pCommandLine = GetCommandLine();
	wchar_t *commandLine = wcschr(wcschr(pCommandLine, L' ') + 1, L' ') + 1;
	
	PROCESS_INFORMATION pi;
	STARTUPINFO si = { sizeof(si) };

	// create the actual process with the debug flag to avoid an infinite loop

	BOOL bCreated = ::CreateProcessW(nullptr, commandLine, nullptr, nullptr, FALSE, CREATE_SUSPENDED | DEBUG_PROCESS, nullptr, nullptr, &si, &pi);
	
	dbg("CreateProcessW bCreated %d, GetLastError %d", bCreated, GetLastError());
	if (bCreated) {
		WCHAR *path = argv[1];
		
		// allocate buffer for the DLL path name

		void* pPathBuffer = ::VirtualAllocEx(pi.hProcess, nullptr, MAX_PATH * sizeof(WCHAR), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		dbg("VirtualAllocEx ret %p, GetLastError %d", pPathBuffer, GetLastError());
		assert(pPathBuffer);

		// write the path

		SIZE_T written;
		BOOL bWrote = ::WriteProcessMemory(pi.hProcess, pPathBuffer, path, MAX_PATH * sizeof(WCHAR), &written);
		dbg("WriteProcessMemory bWrote %d, GetLastError %d", bWrote, GetLastError());

		// create a remote thread to load the DLL

		HANDLE hRemoteThread = ::CreateRemoteThread(pi.hProcess, nullptr, 0,
			(PTHREAD_START_ROUTINE)::GetProcAddress(::GetModuleHandle(L"kernel32"), "LoadLibraryW"),
			pPathBuffer, 0, nullptr);
		dbg("CreateRemoteThread hRemoteThread %d, GetLastError %d", hRemoteThread, GetLastError());

		// allow the process to continue after this one exits

		::DebugSetProcessKillOnExit(FALSE);
		ResumeThread(pi.hThread);

		// close handles (not really needed as we're exiting)
		::CloseHandle(hRemoteThread);
		::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);
	}

	return 0;
}

