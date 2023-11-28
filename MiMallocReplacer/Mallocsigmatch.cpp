#include "Mallocsigmatch.hpp"
#include <vector>
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>


std::vector<void*> mallocsigmatch::GetFunctionAdress(std::string moduleName ,MiMallocReplacedFunctions function)
{
    //DWORD processID = GetCurrentProcessId(); // Get the current process ID
    //HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    //MODULEENTRY32 me32;
    std::vector<void*> foundAdresses;
    // Take a snapshot of all modules in the current process.
    //hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processID);
    //if (hModuleSnap == INVALID_HANDLE_VALUE) {
    //    std::cerr << "CreateToolhelp32Snapshot failed." << std::endl;
    //    return foundAdresses;
    //}

    // Set the size of the structure before using it.
    //me32.dwSize = sizeof(MODULEENTRY32);

    // Retrieve information about the first module,
    // and exit if unsuccessful
    //if (!Module32First(hModuleSnap, &me32)) {
    //    std::cerr << "Module32First failed." << std::endl;  // Show cause of failure
    //    CloseHandle(hModuleSnap);           // Clean the snapshot object
    //    return foundAdresses;
    //}

    // Now walk the module list of the process,


    // Now walk the module list of the process,
    // and display information about each module
    //do {
    //    sigmatch::this_process_target target;
    //    sigmatch::search_result result = target.in_module(me32.szModule).search(functionSignatureMap[function]);
    //    if (result.matches().size() > 0);
    //    //Just return the first match for now
    //    for (const std::byte* address : result.matches()) {
    //        foundAdresses.push_back((void*)address);
    //    }
    //} while (Module32Next(hModuleSnap, &me32));

    //CloseHandle(hModuleSnap);

    sigmatch::this_process_target target;
    sigmatch::search_result result = target.in_module(moduleName).search(functionSignatureMap[function]);
    if (result.matches().size() > 0);
    //Just return the first match for now
    for (const std::byte* address : result.matches()) {
        foundAdresses.push_back((void*)address);
    }
    
    
    //Return nullptr if search fails
    return foundAdresses;
}
