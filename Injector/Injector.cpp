#include <vector>
#include <iostream>
#include <easyhook.h>
#include <string>

import Configuration;

HANDLE createPipe() {
	// Create a named pipe
	HANDLE pipe = CreateNamedPipe(
		L"\\\\.\\pipe\\myoutputpipe", // name of the pipe
		PIPE_ACCESS_INBOUND, // only need read access
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, // byte-type pipe
		1, // number of instances
		0, // output buffer size
		0, // input buffer size
		0, // client time-out
		NULL); // default security attribute

	if (pipe == INVALID_HANDLE_VALUE) {
		std::cerr << "Failed to create pipe" << std::endl;
		return nullptr;
	}

	return pipe;
}

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

	//Create Named Pipe so stdout can be passed trough
	HANDLE pipe = createPipe();

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

	

	// Wait for the child process to connect
	BOOL connected = ConnectNamedPipe(pipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

	if (!connected) {
		std::cerr << "Failed to connect to pipe" << std::endl;
		CloseHandle(pipe);
		return 1;
	}

	std::cout << "Child process connected. Reading data..." << std::endl;

	// Read data from the pipe and output the result (output stdout of launched game)
	DWORD bytesRead;
	char buffer[1000];
	while (ReadFile(pipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) != FALSE) {
		buffer[bytesRead] = '\0'; // null-terminate the string
		std::cout << buffer;
		//std::wcout << L"Hello Hello";
	}

	
	CloseHandle(pipe);
	std::wcout << L"Pipe Closed";
	
	
	return 0;
}