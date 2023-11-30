#include <iostream>
#include <string>
#include <cstring>
#include <locale>
#include <codecvt>
#include <vector>

#include <easyhook.h>

int wmain(int argc, wchar_t* argv[])
{
	const WCHAR* dllToInject = L"MiMallocReplacer.dll";

	//// Calculate the total length required for the wchar_t array
	//int totalLength = 0;
	//for (int i = 1; i < argc; ++i) {
	//	totalLength += MultiByteToWideChar(CP_UTF8, 0, argv[i], -1, NULL, 0);
	//}
	//totalLength += argc - 2; // For spaces and final null terminator

	//// Allocate memory for the wchar_t array
	//wchar_t* combinedArgs = new wchar_t[totalLength];
	//wchar_t* currentPos = combinedArgs;

	//for (int i = 1; i < argc; ++i) {
	//	// Convert and concatenate each argument
	//	int converted = MultiByteToWideChar(CP_UTF8, 0, argv[i], -1, currentPos, totalLength - (currentPos - combinedArgs));
	//	currentPos += converted - 1; // -1 to overwrite the null terminator

	//	// Add a space between arguments, but not after the last one
	//	if (i < argc - 1) {
	//		*currentPos = L' ';
	//		currentPos++;
	//	}
	//}

	//// Null-terminate the string
	//*currentPos = L'\0';
	

	const WCHAR* exeName = L".\\stellaris.exe";
	ULONG procIdVar = 0;
	ULONG* procId = &procIdVar;

	NTSTATUS nt = RhCreateAndInject(
		(WCHAR*)exeName,
		*argv,
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
	return 0;
}