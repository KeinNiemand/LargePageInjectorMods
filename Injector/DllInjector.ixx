module;
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <stdexcept>
#include <iostream>
#include <sstream>

export module DllInjector;

import Logger;

//
// A small RAII wrapper to close a handle automatically on scope exit.
//
struct HandleWrapper {
    HANDLE handle{ nullptr };

    // Allows implicit construction from HANDLE
    HandleWrapper(HANDLE h) : handle(h) {}

    ~HandleWrapper() {
        if (handle != nullptr && handle != INVALID_HANDLE_VALUE) {
            CloseHandle(handle);
        }
    }

    // Disallow copying
    HandleWrapper(const HandleWrapper&) = delete;
    HandleWrapper& operator=(const HandleWrapper&) = delete;

    // Allow move
    HandleWrapper(HandleWrapper&& other) noexcept {
        handle = other.handle;
        other.handle = nullptr;
    }
    HandleWrapper& operator=(HandleWrapper&& other) noexcept {
        if (this != &other) {
            if (handle && handle != INVALID_HANDLE_VALUE) {
                CloseHandle(handle);
            }
            handle = other.handle;
            other.handle = nullptr;
        }
        return *this;
    }

    // Helper to check validity
    bool valid() const {
        return (handle != nullptr) && (handle != INVALID_HANDLE_VALUE);
    }
};

//
// Utility to format GetLastError() codes into a string
//
static std::string getLastErrorString(DWORD err) {
    std::ostringstream oss;
    oss << " (GLE=0x" << std::hex << err << ")";
    return oss.str();
}

//
// Exported function that will:
// 1. Create a process suspended,
// 2. Inject the DLL via remote thread calling LoadLibraryW,
// 3. Resume the main thread.
//
// Returns the PID of the newly created process if successful.
// Returns 0 on failure.
//
export DWORD createSuspendedProcessAndInject(
    const std::wstring& applicationPath, // The EXE to launch
    const std::wstring& commandLineArgs, // Any arguments to pass
    const std::wstring& dllPath          // Possibly relative path to the DLL
)
{
    Logger::Log(Logger::Level::Debug, L"Beginning injection procedure...");

    // -------------------------
    // STEP 1: Create process in suspended mode
    // -------------------------
    STARTUPINFOW si{};
    si.cb = sizeof(si);

    PROCESS_INFORMATION pi{};
    std::wstring fullCommandLine = applicationPath + L" " + commandLineArgs;

    // CreateProcessW expects non-const data, so we make a copy buffer:
    std::wstring mutableCommandLine = fullCommandLine;

    Logger::Log(Logger::Level::Debug, L"Creating process in suspended mode...");

    BOOL createOk = CreateProcessW(
        applicationPath.c_str(),
        mutableCommandLine.data(),
        nullptr,   // default process security attributes
        nullptr,   // default thread security attributes
        FALSE,     // do not inherit handles
        CREATE_SUSPENDED | HIGH_PRIORITY_CLASS, // suspended + high priority
        nullptr,   // use parent's environment block
        nullptr,   // use parent's current directory
        &si,
        &pi
    );

    if (!createOk) {
        DWORD gle = GetLastError();
        std::wostringstream oss;
        oss << L"Failed to create process in suspended mode."
            << getLastErrorString(gle).c_str();
        Logger::Log(Logger::Level::Error, oss.str());
        return 0;
    }

    Logger::Log(Logger::Level::Debug, L"Process created successfully in suspended mode.");

    HandleWrapper processHandle(pi.hProcess);
    HandleWrapper threadHandle(pi.hThread);

    // -------------------------
    // STEP 2: Allocate memory in the remote process for the DLL path
    // -------------------------
    const size_t allocSize = (dllPath.size() + 1) * sizeof(wchar_t);

    Logger::Log(Logger::Level::Debug, L"Allocating memory in remote process for DLL path...");
    LPVOID remoteMemory = VirtualAllocEx(
        processHandle.handle,
        nullptr,
        allocSize,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );

    if (!remoteMemory) {
        DWORD gle = GetLastError();
        std::wostringstream oss;
        oss << L"Failed to allocate memory in remote process."
            << getLastErrorString(gle).c_str();
        Logger::Log(Logger::Level::Error, oss.str());
        return 0;
    }

    Logger::Log(Logger::Level::Debug, L"Memory allocated in remote process.");

    // -------------------------
    // STEP 3: Write the DLL path into the remote process
    // -------------------------
    SIZE_T bytesWritten = 0;
    BOOL writeOk = WriteProcessMemory(
        processHandle.handle,
        remoteMemory,
        dllPath.c_str(),
        allocSize,
        &bytesWritten
    );

    if (!writeOk || bytesWritten != allocSize) {
        DWORD gle = GetLastError();
        std::wostringstream oss;
        oss << L"Failed to write DLL path into the remote process memory."
            << getLastErrorString(gle).c_str();
        Logger::Log(Logger::Level::Error, oss.str());
        return 0;
    }

    Logger::Log(Logger::Level::Debug, L"DLL path successfully written to remote memory.");

    // -------------------------
    // STEP 4: Create remote thread in the suspended process to call LoadLibraryW
    // -------------------------
    Logger::Log(Logger::Level::Debug, L"Retrieving LoadLibraryW address from kernel32.dll...");
    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!hKernel32) {
        DWORD gle = GetLastError();
        std::wostringstream oss;
        oss << L"Failed to get handle of kernel32.dll."
            << getLastErrorString(gle).c_str();
        Logger::Log(Logger::Level::Error, oss.str());
        return 0;
    }

    LPVOID loadLibraryAddr = reinterpret_cast<LPVOID>(GetProcAddress(hKernel32, "LoadLibraryW"));
    if (!loadLibraryAddr) {
        DWORD gle = GetLastError();
        std::wostringstream oss;
        oss << L"Failed to find LoadLibraryW address in kernel32.dll."
            << getLastErrorString(gle).c_str();
        Logger::Log(Logger::Level::Error, oss.str());
        return 0;
    }

    Logger::Log(Logger::Level::Debug, L"Creating remote thread to call LoadLibraryW...");
    HANDLE remoteThread = CreateRemoteThread(
        processHandle.handle,
        nullptr,
        0,
        reinterpret_cast<LPTHREAD_START_ROUTINE>(loadLibraryAddr),
        remoteMemory,
        0,
        nullptr
    );

    if (!remoteThread) {
        DWORD gle = GetLastError();
        std::wostringstream oss;
        oss << L"Failed to create remote thread in target process."
            << getLastErrorString(gle).c_str();
        Logger::Log(Logger::Level::Error, oss.str());
        return 0;
    }

    {
        HandleWrapper remoteThreadHandle(remoteThread);
        Logger::Log(Logger::Level::Debug, L"Waiting for remote thread to finish (LoadLibraryW)...");
        DWORD waitRes = WaitForSingleObject(remoteThreadHandle.handle, INFINITE);
        if (waitRes != WAIT_OBJECT_0) {
            DWORD gle = GetLastError();
            std::wostringstream oss;
            oss << L"WaitForSingleObject on remote thread failed."
                << getLastErrorString(gle).c_str();
            Logger::Log(Logger::Level::Error, oss.str());
            return 0;
        }

        // Check the exit code of the thread (return value of LoadLibraryW)
        DWORD loadLibraryRet = 0;
        if (GetExitCodeThread(remoteThreadHandle.handle, &loadLibraryRet)) {
            if (loadLibraryRet == 0) {
                Logger::Log(Logger::Level::Warning,
                    L"Remote LoadLibraryW returned NULL. The DLL may not have loaded. "
                    L"Check the DLL path or bitness of the process/DLL.");
            }
            else {
                std::wostringstream oss2;
                oss2 << L"Remote thread returned HMODULE=0x" << std::hex << loadLibraryRet
                    << L" (LoadLibraryW success).";
                Logger::Log(Logger::Level::Debug, oss2.str());
            }
        }
        else {
            DWORD gle = GetLastError();
            std::wostringstream oss2;
            oss2 << L"Could not retrieve remote thread exit code."
                << getLastErrorString(gle).c_str();
            Logger::Log(Logger::Level::Warning, oss2.str());
        }
    }

    Logger::Log(Logger::Level::Debug, L"LoadLibraryW call complete in remote process.");

    // -------------------------
    // STEP 5: Resume the main thread so the target program can continue
    // -------------------------
    Logger::Log(Logger::Level::Debug, L"Resuming main thread of the process...");
    DWORD resumeCount = ResumeThread(threadHandle.handle);
    if (resumeCount == (DWORD)-1) {
        DWORD gle = GetLastError();
        std::wostringstream oss;
        oss << L"Failed to resume the main thread of the target process."
            << getLastErrorString(gle).c_str();
        Logger::Log(Logger::Level::Error, oss.str());
        return 0;
    }

    std::wostringstream oss;
    oss << L"DLL injection procedure completed. Target PID=" << pi.dwProcessId;
    Logger::Log(Logger::Level::Info, oss.str());

    // Return the PID to the caller
    return pi.dwProcessId;
}