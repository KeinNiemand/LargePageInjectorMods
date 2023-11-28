#include <easyhook.h>
#include <string>
#include "framework.h"
#include "Mallocsigmatch.hpp"
#include <mimalloc.h>
#include <mimalloc-new-delete.h>

//DWORD gFreqOffset = 0;
//BOOL WINAPI myBeepHook(DWORD dwFreq, DWORD dwDuration)
//{
//	std::cout << "\n    BeepHook: ****All your beeps belong to us!\n\n";
//	return Beep(dwFreq + gFreqOffset, dwDuration);
//}

// EasyHook will be looking for this export to support DLL injection. If not found then 
// DLL injection will fail.
extern "C" void __declspec(dllexport) __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO * inRemoteInfo);

//Old function using easy hook for the actual hooking dosn't work for free_base
//void PerformHook(void* adress, void* hookFunction)
//{
//	HOOK_TRACE_INFO hHook = { NULL }; // keep track of our hook
//	//std::cout << "\n";
//	//std::cout << "Win32" + moduleHandleName + "found at address : " << GetProcAddress(GetModuleHandle(TEXT("kernel32")), "Beep") << "\n";
//
//	// Install the hook
//	NTSTATUS result = LhInstallHook(
//		adress,
//		hookFunction,
//		NULL,
//		&hHook);
//	if (FAILED(result))
//	{
//		std::wstring s(RtlGetLastErrorString());
//		Beep(2000, 100);
//		printf("ERROR INSTALLING HOOK!");
//		//printToConsole(L"Failed to install hook: ");
//		//printToConsole(s.c_str());
//	}
//	else
//	{
//		//printToConsole(L"Hook 'installed successfully.");
//	}
//
//	// If the threadId in the ACL is set to 0,
//	// then internally EasyHook uses GetCurrentThreadId()
//	ULONG ACLEntries[1] = { 0 };
//
//	// Disable the hook for the provided threadIds, enable for all others
//	LhSetExclusiveACL(ACLEntries, 10, &hHook);
//}

void* AllocatePageNearAddress(void* targetAddr)
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	const uint64_t PAGE_SIZE = sysInfo.dwPageSize;

	uint64_t startAddr = (uint64_t(targetAddr) & ~(PAGE_SIZE - 1)); //round down to nearest page boundary
	uint64_t minAddr = min(startAddr - 0x7FFFFF00, (uint64_t)sysInfo.lpMinimumApplicationAddress);
	uint64_t maxAddr = max(startAddr + 0x7FFFFF00, (uint64_t)sysInfo.lpMaximumApplicationAddress);

	uint64_t startPage = (startAddr - (startAddr % PAGE_SIZE));

	uint64_t pageOffset = 1;
	while (1)
	{
		uint64_t byteOffset = pageOffset * PAGE_SIZE;
		uint64_t highAddr = startPage + byteOffset;
		uint64_t lowAddr = (startPage > byteOffset) ? startPage - byteOffset : 0;

		bool needsExit = highAddr > maxAddr && lowAddr < minAddr;

		if (highAddr < maxAddr)
		{
			void* outAddr = VirtualAlloc((void*)highAddr, PAGE_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			if (outAddr)
				return outAddr;
		}

		if (lowAddr > minAddr)
		{
			void* outAddr = VirtualAlloc((void*)lowAddr, PAGE_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			if (outAddr != nullptr)
				return outAddr;
		}

		pageOffset++;

		if (needsExit)
		{
			break;
		}
	}

	return nullptr;
}

void WriteAbsoluteJump64(void* absJumpMemory, void* addrToJumpTo)
{
	uint8_t absJumpInstructions[] =
	{
	  0x49, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //mov r10, addr
	  0x41, 0xFF, 0xE2 //jmp r10
	};

	uint64_t addrToJumpTo64 = (uint64_t)addrToJumpTo;
	memcpy(&absJumpInstructions[2], &addrToJumpTo64, sizeof(addrToJumpTo64));
	memcpy(absJumpMemory, absJumpInstructions, sizeof(absJumpInstructions));
}

void InstallHook(void* func2hook, void* payloadFunction)
{
	void* relayFuncMemory = AllocatePageNearAddress(func2hook);
	WriteAbsoluteJump64(relayFuncMemory, payloadFunction); //write relay func instructions

	//now that the relay function is built, we need to install the E9 jump into the target func,
	//this will jump to the relay function
	DWORD oldProtect;
	VirtualProtect(func2hook, 1024, PAGE_EXECUTE_READWRITE, &oldProtect);

	//32 bit relative jump opcode is E9, takes 1 32 bit operand for jump offset
	uint8_t jmpInstruction[5] = { 0xE9, 0x0, 0x0, 0x0, 0x0 };

	//to fill out the last 4 bytes of jmpInstruction, we need the offset between 
	//the relay function and the instruction immediately AFTER the jmp instruction
	const uint64_t relAddr = (uint64_t)relayFuncMemory - ((uint64_t)func2hook + sizeof(jmpInstruction));
	memcpy(jmpInstruction + 1, &relAddr, 4);

	//install the hook
	memcpy(func2hook, jmpInstruction, sizeof(jmpInstruction));
}

void HookIfSigFound(std::string moduleName, MiMallocReplacedFunctions function, void* replacementFunctionPointer) {
	mallocsigmatch sigmatcher;
	std::vector<void*> functionAdresses = sigmatcher.GetFunctionAdress("stellaris.exe", function);

	for (void* functionAdress : functionAdresses) {
		if (functionAdress) {
			InstallHook(functionAdress, replacementFunctionPointer);
			//Beep(1000, 100);
		}
		else {
			Beep(250, 100);
		}
	}

}

void HookAllMallocFunctions(const std::string& ModuleName) {
	HookIfSigFound(ModuleName, MiMallocReplacedFunctions::malloc, mi_malloc);
	HookIfSigFound(ModuleName, MiMallocReplacedFunctions::free, mi_free);
	HookIfSigFound(ModuleName, MiMallocReplacedFunctions::free_base, mi_free);
	HookIfSigFound(ModuleName, MiMallocReplacedFunctions::realloc, mi_realloc);
	HookIfSigFound(ModuleName, MiMallocReplacedFunctions::calloc, mi_calloc);
	HookIfSigFound(ModuleName, MiMallocReplacedFunctions::_strdup, mi_strdup);
	HookIfSigFound(ModuleName, MiMallocReplacedFunctions::_msize, mi_usable_size);
	HookIfSigFound(ModuleName, MiMallocReplacedFunctions::_recalloc, mi_recalloc);
	HookIfSigFound(ModuleName, MiMallocReplacedFunctions::_wcsdup, mi_wcsdup);
	HookIfSigFound(ModuleName, MiMallocReplacedFunctions::_aligned_malloc, mi_malloc_aligned);
	HookIfSigFound(ModuleName, MiMallocReplacedFunctions::_aligned_free, mi_free);
	HookIfSigFound(ModuleName, MiMallocReplacedFunctions::operator_new, mi_new);
}

extern "C" void __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* inRemoteInfo)
{
	//Load Mimalloc redirect and ovveride, this automatically replaces malloc used here with mimalloc
	//LoadLibrary(L"mimalloc-redirect.dll");
	//LoadLibrary(L"mimalloc-override.dll");

	//std::wstring	printToConsole(L"Injected by process Id: "inRemoteInfo->HostPID)  L"\n");
	//std::cout << "Passed in data size: " << inRemoteInfo->UserDataSize << "\n";
	//if (inRemoteInfo->UserDataSize == sizeof(DWORD))
	//{
	//	gFreqOffset = *reinterpret_cast<DWORD*>(inRemoteInfo->UserData);
	//	std::cout << "Adjusting Beep frequency by: " << gFreqOffset << "\n";
	//}

	//mi_version();
	// Perform hooking
	mi_version();
	HookAllMallocFunctions("stellaris.exe");

	//Most if not all of these are probably pointless since it's unlikely that diffrent dll have their own version of malloc
	//It definitly works without this
	//BUt just in case I'm trieing to overide these bunchh of random dll anyways
	HookAllMallocFunctions("pops_api.dll");
	HookAllMallocFunctions("pdx_red_king.dll");
	HookAllMallocFunctions("d3d11.dll");
	HookAllMallocFunctions("d3d9.dll");
	HookAllMallocFunctions("D3DCompiler_47.dll");
	HookAllMallocFunctions("D3X9_43.dll");
	HookAllMallocFunctions("D3DCompiler_47.dll");
	HookAllMallocFunctions("msvcpwin.dll");
	HookAllMallocFunctions("msvcp110_win.dll");
	HookAllMallocFunctions("msvcp140.dll");
	HookAllMallocFunctions("msvcrt.dll");
	HookAllMallocFunctions("ucrtbase.dll");
	HookAllMallocFunctions("vcruntime_140.dll");
	HookAllMallocFunctions("vcruntime_140_1.dll");
	//HookIfSigFound("stellaris.exe", MiMallocReplacedFunctions::operator_new_nothrow, mi_new_nothrow); //Disabled Bad Signature



	
	//Let the game run
	Beep(1000, 100);
    RhWakeUpProcess();


	return;
}