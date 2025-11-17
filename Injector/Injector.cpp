#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <thread>
#include <iostream>
#include <string>
#include <atomic>

import Configuration;
import Logger;
import DllInjector;

// Global (or pass via a struct) to signal the pipe thread to stop if needed
static std::atomic_bool g_shouldStopPipeReading{ false };
static std::jthread pipeReaderJThread;

//
// A simple thread function that:
// 1) blocks on ConnectNamedPipe
// 2) reads from the pipe in a loop
// 3) logs or prints the incoming data
//
void pipeReaderThread(HANDLE pipeHandle)
{
    // Wait for a client to connect (or check if already connected)
    BOOL connected = ConnectNamedPipe(pipeHandle, NULL)
        ? TRUE
        : (GetLastError() == ERROR_PIPE_CONNECTED);
    if (!connected)
    {
        Logger::Log(Logger::Level::Error, "Failed to connect named pipe");
        CloseHandle(pipeHandle);
        return;
    }

    Logger::Log(Logger::Level::Info, L"Child process connected to named pipe. Reading data...");

    DWORD bytesRead = 0;
    char buffer[1000]{};

    // Read loop
    while (!g_shouldStopPipeReading.load())
    {
        BOOL success = ReadFile(
            pipeHandle,
            buffer,
            sizeof(buffer) - 1,
            &bytesRead,
            NULL
        );
        if (!success || bytesRead == 0)
        {
            // Either an error or the client closed the pipe
            break;
        }
        // Null-terminate and write to stdout
        buffer[bytesRead] = '\0';
        std::cout << buffer;
    }

    Logger::Log(Logger::Level::Info, L"Pipe closed or reading stopped");
    CloseHandle(pipeHandle);
}

// Erstellt beide Enden der Named Pipe und gibt das Client-Handle zurück
// (Das Server-Handle wird für den Reader-Thread verwendet)
HandleWrapper createPipeForProcess() {
    // Erst die Named Pipe für das Lesen erstellen (Server-Ende)
    HANDLE serverPipe = CreateNamedPipeW(
        L"\\\\.\\pipe\\myoutputpipe",      // pipe name
        PIPE_ACCESS_INBOUND,               // read-only
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS,
        1,                                 // eine Instanz
        0,                                 // Output-Puffer-Größe
        0,                                 // Input-Puffer-Größe
        0,                                 // Default-Timeout
        NULL                               // Default-Security
    );

    if (serverPipe == INVALID_HANDLE_VALUE) {
        Logger::Log(Logger::Level::Error, "Failed to create server pipe");
        return nullptr;
    }

    // Dann das Client-Ende erzeugen, das an den untergeordneten Prozess übergeben wird
    SECURITY_ATTRIBUTES saAttr{};
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;          // Wichtig: Dieses Handle MUSS vererbbar sein
    saAttr.lpSecurityDescriptor = NULL;

    HANDLE clientPipe = CreateFileW(
        L"\\\\.\\pipe\\myoutputpipe",
        GENERIC_WRITE,
        0,                                 // Kein Sharing
        &saAttr,                           // Sicherheitsattribute mit Vererbung
        OPEN_EXISTING,
        0,                                 // Standard-Attribute
        NULL                               // Keine Vorlage
    );

    if (clientPipe == INVALID_HANDLE_VALUE) {
        CloseHandle(serverPipe);
        Logger::Log(Logger::Level::Error, "Failed to create client pipe");
        return nullptr;
    }

    // Server-Handle nicht vererben (wir wollen nicht, dass der Kindprozess es sieht)
    SetHandleInformation(serverPipe, HANDLE_FLAG_INHERIT, 0);

    // Starte den Lesethread für das Server-Ende
    pipeReaderJThread = std::jthread(pipeReaderThread, serverPipe);

    // Gib das Client-Ende zurück, das an den untergeordneten Prozess übergeben wird
    return clientPipe;
}

std::wstring getExecutableFolderPath() {
    constexpr size_t bufferSize = 65535;
    auto buffer = std::make_unique<wchar_t[]>(bufferSize);
    DWORD size = GetModuleFileNameW(NULL, buffer.get(), bufferSize);
    if (size == 0) {
        Logger::Log(Logger::Level::Error, "Failed to GetModuleFileNameW in getExecutableFolderPath");
        return {};
    }

    std::wstring path(buffer.get());
    size_t pos = path.find_last_of(L"\\/");
    if (pos != std::wstring::npos)
        return path.substr(0, pos + 1);

    Logger::Log(Logger::Level::Error, "Error when trying to get executable folder path");
    return {};
}

int wmain(int argc, wchar_t* argv[])
{
    using namespace std::string_literals;

    std::wstring passTroughArgument;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            passTroughArgument += LR"(")"s + argv[i] + LR"(")"s;
            if (i + 1 < argc) {
                passTroughArgument += L" ";
            }
        }
    }

    std::wstring executableFolderPath = getExecutableFolderPath();
    _wchdir(executableFolderPath.c_str());

    // Load config
    Configuration config;
    config.loadFromFile(".\\LargePageInjectorMods.toml");

    Logger::Log(Logger::Level::Debug, L"exe path = " + config.LaunchPath);

    // Possibly set environment variables from config
    for (auto& [key, value] : config.environment) {
        SetEnvironmentVariableW(key.c_str(), value.c_str());
    }


    // Create named pipe + reading thread FIRST if user wants console redirection
    HandleWrapper clientPipeHandle = nullptr;
    if (config.redirectConsoleOutput) {
        clientPipeHandle = createPipeForProcess();
        if (!clientPipeHandle.valid()) {
            Logger::Log(Logger::Level::Error, "Could not create pipe no io redireciton possible; continuing anyway...");
        }
    }

    // Now do the injection
    Logger::Log(Logger::Level::Debug, "Starting DLL injection...");
    DWORD pid = createSuspendedProcessAndInject(
        config.LaunchPath,    // EXE
        passTroughArgument,   // Args
        L"MiMallocReplacer.DLL"s, // The DLL (relative or absolute)
        clientPipeHandle.handle          //Client handle for the pipe for IO redirection
    );

    if (pid == 0) {
        Logger::Log(Logger::Level::Error, "Injection failed (returned 0). Check logs for details.");
        // If you want to stop the pipe reading thread:
        g_shouldStopPipeReading = true;
        // Possibly Sleep() or do some other mechanism to ensure the thread wakes up
        return 1;
    }

    Logger::Log(Logger::Level::Info, L"Library injected successfully into PID=" + std::to_wstring(pid));

    // The main thread could do more, or just wait until the child process exits, etc.
    // The separate pipeReaderThread will keep reading logs as they come in.

    // Example: Wait for the child to close by hooking WaitForSingleObject on pid, or do nothing.
    // You might do something like:
    if (TRUE) {
        HANDLE hProc = OpenProcess(SYNCHRONIZE, FALSE, pid);
        WaitForSingleObject(hProc, INFINITE);
        CloseHandle(hProc);
    }

    // When done, tell the pipe thread to stop reading
    g_shouldStopPipeReading = true;
    Logger::Log(Logger::Level::Info, "Main injector done.");
    return 0;
}
