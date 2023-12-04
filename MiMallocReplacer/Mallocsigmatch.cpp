#include "Mallocsigmatch.hpp"
#include <vector>
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>


std::vector<void*> mallocsigmatch::GetFunctionAdress(std::string moduleName ,MiMallocReplacedFunctions function)
{
    std::vector<void*> foundAdresses;

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
