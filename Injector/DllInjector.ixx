module;
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <stdexcept>
#include <iostream>

export module DllInjector;

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
// Exported function that will:
// 1. Create a process suspended,
// 2. Inject the DLL via remote thread calling LoadLibraryW,
// 3. Resume the main thread.
//
// Returns the PID of the newly created process if successful, or throws on failure.
//
export DWORD createSuspendedProcessAndInject(
    const std::wstring& applicationPath, // The EXE to launch
    const std::wstring& commandLineArgs, // Any arguments to pass
    const std::wstring& dllPath          // Full path to the DLL to inject
)
{
    // -------------------------
    // STEP 1: Create process in suspended mode
    // -------------------------
    STARTUPINFOW si{};
    si.cb = sizeof(si);

    PROCESS_INFORMATION pi{};
    std::wstring fullCommandLine = applicationPath + L" " + commandLineArgs;

    // CreateProcessW expects non-const data, so we make a copy buffer:
    std::wstring mutableCommandLine = fullCommandLine;

    // CREATE_SUSPENDED ensures the main thread isn't started yet
    if (!CreateProcessW(
        applicationPath.c_str(),
        mutableCommandLine.data(),
        nullptr,   // default process security attributes
        nullptr,   // default thread security attributes
        FALSE,     // do not inherit handles
        CREATE_SUSPENDED | HIGH_PRIORITY_CLASS, //Create suspended and has high priority (dosn't hurt for extra performance)
        nullptr,   // use parent's environment block
        nullptr,   // use parent's current directory
        &si,
        &pi
    ))
    {
        throw std::runtime_error("Failed to create process in suspended mode.");
    }

    HandleWrapper processHandle(pi.hProcess);
    HandleWrapper threadHandle(pi.hThread);

    // -------------------------
    // STEP 2: Allocate memory in the remote process for the DLL path
    // -------------------------
    const size_t allocSize = (dllPath.size() + 1) * sizeof(wchar_t);

    LPVOID remoteMemory = VirtualAllocEx(
        processHandle.handle,
        nullptr,
        allocSize,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );

    if (!remoteMemory) {
        throw std::runtime_error("Failed to allocate memory in remote process.");
    }

    // -------------------------
    // STEP 3: Write the DLL path into the remote process
    // -------------------------
    SIZE_T bytesWritten = 0;
    if (!WriteProcessMemory(
        processHandle.handle,
        remoteMemory,
        dllPath.c_str(),
        allocSize,
        &bytesWritten
    ) || bytesWritten != allocSize)
    {
        throw std::runtime_error("Failed to write DLL path into the remote process memory.");
    }

    // -------------------------
    // STEP 4: Create remote thread in the suspended process to call LoadLibraryW
    // -------------------------
    // Get the address of LoadLibraryW in kernel32
    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (!hKernel32) {
        throw std::runtime_error("Failed to get handle of kernel32.dll.");
    }

    LPVOID loadLibraryAddr = reinterpret_cast<LPVOID>(GetProcAddress(hKernel32, "LoadLibraryW"));
    if (!loadLibraryAddr) {
        throw std::runtime_error("Failed to find LoadLibraryW address in kernel32.dll.");
    }

    // Create a thread to run LoadLibraryW(remoteMemory)
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
        throw std::runtime_error("Failed to create remote thread in target process.");
    }

    {
        HandleWrapper remoteThreadHandle(remoteThread);
        // Wait for LoadLibraryW to finish
        WaitForSingleObject(remoteThreadHandle.handle, INFINITE);
    }

    // Optionally, you might want to verify the DLL loaded successfully by checking return value
    // or hooking further. This is left out for brevity.

    // -------------------------
    // STEP 5: Resume the main thread so the target program can continue
    // -------------------------
    if (ResumeThread(threadHandle.handle) == (DWORD)-1) {
        throw std::runtime_error("Failed to resume the main thread of the target process.");
    }

    // We intentionally don't close the process handle or thread handle here
    // immediately, so the process can continue running. But the RAII wrapper
    // will close them on function exit if desired.

    // Return the PID to the caller
    return pi.dwProcessId;
}