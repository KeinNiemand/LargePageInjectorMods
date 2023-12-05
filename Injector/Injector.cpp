import Configuration;
#include <vector>
#include <iostream>
#include <easyhook.h>
#include <string>

int wmain(int argc, wchar_t* argv[])
{
	using namespace std::string_literals; // enables s-suffix for std::string literals

	const WCHAR* dllToInject = L"MiMallocReplacer.dll";
	std::wstring passTroughArgument = L" ";

	if (argc > 1) {
		passTroughArgument = L"";
		for (int i = 1; i < argc; i++) {
			passTroughArgument += LR"(")"s + argv[i] + LR"(")"s;
			if (i + 1 < argc) {
				passTroughArgument += L" ";
			}
		}
	}

	//Load config
	Configuration config;
	config.loadFromFile(".\\LargePageInjectorMods.config");

	const WCHAR* exeName = config.LaunchPath.c_str();
	ULONG procIdVar = 0;
	ULONG* procId = &procIdVar;

	NTSTATUS nt = RhCreateAndInject(
		(WCHAR*)exeName,
		(WCHAR*)passTroughArgument.c_str(),
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