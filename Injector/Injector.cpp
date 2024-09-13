#include <vector>
#include <iostream>
#include <easyhook.h>
#include <string>

import Configuration;
import Logger;
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

std::wstring getExecutableFolderPath() {
	// Allocate buffer on the heap

	//NTFS technical max path length, I know this is overkill (normally MAX_PATH 260 is enough)
	//but large path aware applications can have longer paths so this makes it easier to enable large path support if I want to in the future
	constexpr size_t bufferSize = 65535; 
	auto buffer = std::unique_ptr<wchar_t[]>(new wchar_t[](bufferSize));
	DWORD size = GetModuleFileNameW(NULL, buffer.get(), bufferSize);
	if (size == 0) {
		// Handle error, GetLastError() can be used to get error details
		return nullptr;
	}

	// Convert buffer to std::wstring
	std::wstring path(buffer.get(), bufferSize);
	// Find the last backslash in the path to separate the folder path
	size_t pos = path.find_last_of(L"\\/");
	if (pos != std::wstring::npos) {
		return path.substr(0, pos);
	}

	return nullptr;
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

	//Change Current Directory to Executuabel Folder this is probably not the best solution but might work
	//TODO: instead of changing direcotry maybe use absolute paths instead (including a way run the exe from in exectuable folder if a relative path is passed into the config) or rdstore path
	{
		std::wstring executableFolderPath = getExecutableFolderPath();
		_wchdir(executableFolderPath.c_str());
	}
	//Load config
	Configuration config;
	config.loadFromFile(".\\LargePageInjectorMods.toml");

	const WCHAR* exeName = config.LaunchPath.c_str();
	ULONG procIdVar = 0;
	ULONG* procId = &procIdVar;

	//Create Named Pipe so stdout can be passed trough
	HANDLE pipe = nullptr;
	if (config.redirectConsoleOutput) {
		// Create a named pipe
		pipe = createPipe();
	}

	//Setup enviroment variables from config
	for (auto& [key, value] : config.environment) {
		
		//Use windows API to set enviroment variables make sure child procceses inherit
		SetEnvironmentVariableA(key.c_str(), value.c_str());
	}
	
	//Run the process and inject the dll
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
	
	//TODO: Replace all these verbosity checks with a simple logger
	if (nt != 0)
	{
		Logger::Log(Logger::Level::Error, "RhInjectLibrary failed with error code = " + std::to_string(nt));
		PWCHAR err = RtlGetLastErrorString();
		Logger::Log(Logger::Level::Error, std::wstring(err));
	}
	else if(config.verbosity >= 4)
	{
		Logger::Log(Logger::Level::Info, L"Library injected successfully.");
	}


	// Wait for the child process to connect
	if (config.redirectConsoleOutput) {
		BOOL connected = ConnectNamedPipe(pipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
		if (!connected) {
			Logger::Log(Logger::Level::Error, "Failed to connect to pipe");
			CloseHandle(pipe);
			return 1;
		}

		Logger::Log(Logger::Level::Info, L"Child process connected. Reading data...");
		// Read data from the pipe and output the result (output stdout of launched game)
		DWORD bytesRead;
		char buffer[1000];
		while (ReadFile(pipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) != FALSE) {
			buffer[bytesRead] = '\0'; // null-terminate the string
			std::cout << buffer;
		}

		CloseHandle(pipe);
		Logger::Log(Logger::Level::Info, L"Pipe Closed");

	}
	
	
	return 0;
}