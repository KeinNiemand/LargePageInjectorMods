// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.
#include <mimalloc.h>
#include "framework.h"
#include <easyhook.h>
#include <string>
#include <iostream>
#include <fcntl.h>
#include <io.h>

void RedirectIO(FILE* hFrom, HANDLE hTo)
{
	int fd = _open_osfhandle((intptr_t)hTo, _O_WRONLY | _O_TEXT);
	_dup2(fd, _fileno(hFrom));
	setvbuf(hFrom, NULL, _IONBF, 0); //Disable buffering.
}

void RedirectOutputToInjectorPipe() {
	// Connect to the named pipe
	HANDLE pipe = CreateFile(
		L"\\\\.\\pipe\\myoutputpipe",
		GENERIC_WRITE,
		0, // no sharing
		NULL, // default security attributes
		OPEN_EXISTING,
		0, // default attributes
		NULL); // no template file

	if (pipe == INVALID_HANDLE_VALUE) {
		std::cout << "MiMallocReplacer.dll: Failed to connect to pipe" << std::endl;
	}

	FreeConsole();
	AttachConsole(ATTACH_PARENT_PROCESS);


	FILE* fpio = new FILE();
	if (GetStdHandle(STD_OUTPUT_HANDLE) != INVALID_HANDLE_VALUE) {
		if (freopen_s(&fpio, "CONOUT$", "w", stdout) != NULL) {
			fflush(stdout);
		}
	}

	//RedirectIO(fpio, pipe);

	FILE* fperr = new FILE();
	if (GetStdHandle(STD_ERROR_HANDLE) != INVALID_HANDLE_VALUE) {
		if (freopen_s(&fperr, "CONOUT$", "w", stderr) != NULL) {
			fflush(stdout);
		}
	}

	SetStdHandle(STD_OUTPUT_HANDLE, pipe);
	SetStdHandle(STD_ERROR_HANDLE, pipe);
	RedirectIO(fpio, pipe);
	RedirectIO(fperr, pipe);
	RedirectIO(stdout, pipe);
	RedirectIO(stderr, pipe);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		mi_version();
		RedirectOutputToInjectorPipe();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

