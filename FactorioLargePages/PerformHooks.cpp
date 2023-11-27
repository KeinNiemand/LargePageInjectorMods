#include <easyhook.h>
#include <string>
#include "framework.h"
#include "Mallocsigmatch.hpp"

//DWORD gFreqOffset = 0;
//BOOL WINAPI myBeepHook(DWORD dwFreq, DWORD dwDuration)
//{
//	std::cout << "\n    BeepHook: ****All your beeps belong to us!\n\n";
//	return Beep(dwFreq + gFreqOffset, dwDuration);
//}

// EasyHook will be looking for this export to support DLL injection. If not found then 
// DLL injection will fail.
extern "C" void __declspec(dllexport) __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO * inRemoteInfo);

void PerformHook(void* adress, void* hookFunction)
{
	HOOK_TRACE_INFO hHook = { NULL }; // keep track of our hook
	//std::cout << "\n";
	//std::cout << "Win32" + moduleHandleName + "found at address : " << GetProcAddress(GetModuleHandle(TEXT("kernel32")), "Beep") << "\n";

	// Install the hook
	NTSTATUS result = LhInstallHook(
		adress,
		hookFunction,
		NULL,
		&hHook);
	if (FAILED(result))
	{
		std::wstring s(RtlGetLastErrorString());
		Beep(100, 100);
		Sleep(200);
		//printToConsole(L"Failed to install hook: ");
		//printToConsole(s.c_str());
	}
	else
	{
		//printToConsole(L"Hook 'installed successfully.");
	}

	// If the threadId in the ACL is set to 0,
	// then internally EasyHook uses GetCurrentThreadId()
	ULONG ACLEntries[1] = { 0 };

	// Disable the hook for the provided threadIds, enable for all others
	LhSetExclusiveACL(ACLEntries, 1, &hHook);
}

void HookIfSigFound(std::string moduleName, MiMallocReplacedFunctions function, void* replacementFunctionPointer) {
	mallocsigmatch sigmatcher;
	void* functionAdress = sigmatcher.GetFunctionAdress("stellaris.exe", function);
	if (functionAdress) {
		PerformHook(functionAdress, replacementFunctionPointer);
		//Beep(1000, 100);
	}
	else {
		//Beep(250, 100);
	}
}

extern "C" void __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* inRemoteInfo)
{
	//Load Mimalloc redirect and ovveride, this automatically replaces malloc used here with mimalloc
	//LoadLibrary(L"mimalloc-redirect.dll");
	LoadLibrary(L"mimalloc-override.dll");

	//std::wstring	printToConsole(L"Injected by process Id: "inRemoteInfo->HostPID)  L"\n");
	//std::cout << "Passed in data size: " << inRemoteInfo->UserDataSize << "\n";
	//if (inRemoteInfo->UserDataSize == sizeof(DWORD))
	//{
	//	gFreqOffset = *reinterpret_cast<DWORD*>(inRemoteInfo->UserData);
	//	std::cout << "Adjusting Beep frequency by: " << gFreqOffset << "\n";
	//}

	void* a = malloc(199);
	free(a);

	// Perform hooking
	HookIfSigFound("stellaris.exe", MiMallocReplacedFunctions::malloc, malloc); //Replacing with regular malloc here actually uses mimalloc becouse of the ovveride dll
	HookIfSigFound("stellaris.exe", MiMallocReplacedFunctions::free, free);
	HookIfSigFound("stellaris.exe", MiMallocReplacedFunctions::realloc, realloc);
	HookIfSigFound("stellaris.exe", MiMallocReplacedFunctions::calloc, calloc);
	HookIfSigFound("stellaris.exe", MiMallocReplacedFunctions::_strdup, _strdup);
	HookIfSigFound("stellaris.exe", MiMallocReplacedFunctions::_msize, _msize);
	HookIfSigFound("stellaris.exe", MiMallocReplacedFunctions::_recalloc, _recalloc);
	HookIfSigFound("stellaris.exe", MiMallocReplacedFunctions::_wcsdup, _wcsdup);
	HookIfSigFound("stellaris.exe", MiMallocReplacedFunctions::_aligned_malloc, _aligned_malloc);
	HookIfSigFound("stellaris.exe", MiMallocReplacedFunctions::_aligned_free, _aligned_free);
	//HookIfSigFound("stellaris.exe", MiMallocReplacedFunctions::realloc, realloc);



	
	//Let the game run
	RhWakeUpProcess();
	Beep(1000, 100);

	return;
}