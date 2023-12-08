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

    if (result.matches().size() > 1)
     std::cout << "MiMallocReplacer.dll: WARNING: function in module " << moduleName << " Found more then once." << std::endl;

    if (result.matches().size() > 0) {
        //Just return the first match for now
        for (const std::byte* address : result.matches()) {
            foundAdresses.push_back((void*)address);
        }
    }
    else {
        std::cout << "MiMallocReplacer.dll: WARNING: No Matches Found for function " << (int)function << " In Module :" << moduleName << std::endl;
    }
    
    
    
    //Return nullptr if search fails
    return foundAdresses;
}
