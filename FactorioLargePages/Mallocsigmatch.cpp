#include "Mallocsigmatch.hpp"

void* mallocsigmatch::GetFunctionAdress(std::string moduleName ,MiMallocReplacedFunctions function)
{
    

    sigmatch::this_process_target target;
    sigmatch::search_result result = target.in_module(moduleName).search(functionSignatureMap[function]);
    
    if (result.matches().size() == 1);
        //Just return the first match for now
        for (const std::byte* address : result.matches()) {
            return (void*)address;
        }
    
    //Return nullptr if search fails
    return nullptr;
}
