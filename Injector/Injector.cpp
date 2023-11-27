#include <iostream>
#include <string>
#include <cstring>

#include <easyhook.h>

int main(int argc, char* argv[])
{
	//DWORD processId;
	//std::wcout << "Enter the target process Id: ";
	//std::cin >> processId;

	//DWORD freqOffset = 0;
	//std::cout << "Enter a frequency offset in hertz (e.g. 800): ";
	//std::cin >> freqOffset;

	const WCHAR* dllToInject = L"MiMallocReplacer.dll";
	//wprintf(L"Attempting to inject: %s\n\n", dllToInject);

	// Inject dllToInject into the target process Id, passing 
	// freqOffset as the pass through data.

	//NTSTATUS nt = RhInjectLibrary(
	//	processId,   // The process to inject into
	//	0,           // ThreadId to wake up upon injection
	//	EASYHOOK_INJECT_DEFAULT,
	//	dllToInject, // 32-bit
	//	NULL,		 // 64-bit not provided
	//	&freqOffset, // data to send to injected DLL entry point
	//	sizeof(DWORD)// size of data to send
	//);

	const WCHAR* exeName = L".\\stellaris.exe";
	ULONG procIdVar = 0;
	ULONG* procId = &procIdVar;

	NTSTATUS nt = RhCreateAndInject(
		(WCHAR*)exeName,
		(WCHAR*)L"-gdpr-compliant",
		HIGH_PRIORITY_CLASS, //Process Creation Options
		EASYHOOK_INJECT_DEFAULT,
		nullptr, //No 32bit dll
		(WCHAR*)dllToInject,
		nullptr, //No pass trough buffer
		0, //0 Pass trough size
		procId //Store Created Proc Id
	);

	if (nt != 0)
	{
		printf("RhInjectLibrary failed with error code = %d\n", nt);
		PWCHAR err = RtlGetLastErrorString();
		std::wcout << err << "\n";
	}
	else
	{
		std::wcout << L"Library injected successfully.\n";
	}

	std::wcout << "Press Enter to exit";
	std::wstring input;
	std::getline(std::wcin, input);
	std::getline(std::wcin, input);
	return 0;
}