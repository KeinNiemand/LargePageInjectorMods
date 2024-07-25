module;
#include "framework.h";
#include <sigmatch.hpp>
export module MallocSigmatch;
export import <unordered_map>;
export import <vector>;
export import <string>;
import <iostream>;

export enum class MiMallocReplacedFunctions
{
	malloc,
	free,
	free_base,
	realloc,
	calloc,
	strdup,
	realpath,
	_expand,
	_msize,
	_recalloc,
	_strdup,
	_strndup,
	_wcsdup,
	_mbsdup,
	_dupenv_s,
	_wdupenv_s,

	reallocf,
	malloc_size,
	malloc_usable_size,
	cfree,
	valloc,
	pvalloc,
	reallocarray,
	reallocarr,
	memalign,
	aligned_alloc,
	posix_memalign,
	_posix_memalign,

	_aligned_malloc,
	_aligned_realloc,
	_aligned_recalloc,
	_aligned_msize,
	_aligned_free,
	_aligned_offset_malloc,
	_aligned_offset_realloc,
	_aligned_offset_recalloc,


	operator_new,
	operator_new_nothrow,
	operator_delete,
	operator_deleteArr
};


//Copy and basted from sigmatch.hpp, Intellisense really dosn't like using namespace inside a module for some reason and will give internal error on importing that
//module despite everything compiling fine
template <sigmatch::details::consteval_str_buffer kSigStrBuf>
[[nodiscard]] constexpr decltype(auto) operator""_sig()
{
	return sigmatch::signature{
		sigmatch::signature::____literal_place_t{},
		sigmatch::impl::parse_sig_str_compile_time<kSigStrBuf>() };
}


export class mallocsigmatch
{
private:
	std::unordered_map<MiMallocReplacedFunctions, sigmatch::signature> functionSignatureMap;
	std::unordered_map<MiMallocReplacedFunctions, std::string> functionNameMap;



public:

	mallocsigmatch() :
		functionSignatureMap{
				{ MiMallocReplacedFunctions::malloc,             "40 53 48 83 ec 20 48 8b d9 48 83 f9 ?? 77 3c 48 85 c9"_sig },
			{ MiMallocReplacedFunctions::free_base,          "48 85 c9 74 ?? 53 48 83 ec 20 4c 8b c1 33 d2 48 8b 0d ?? ?? ?? ??"_sig },
			{ MiMallocReplacedFunctions::free,               "c7 44 24 10 00 00 00 00 8b 44 24 10 e9 ?? ?? ?? ??"_sig },
			{ MiMallocReplacedFunctions::realloc,            "48 89 5c 24 08 57 48 83 ec 20 48 8b da 48 8b f9 48 85 c9 75"_sig },
			{ MiMallocReplacedFunctions::calloc,             "40 53 48 83 ec 20 4c 8b c2 48 8b d9 48 85 c9 74 ?? 33 d2"_sig },
			{ MiMallocReplacedFunctions::strdup,             "48 89 5c 24 08 48 89 74 24 10 57 48 83 ec 30 48 8b f9 48 85 c9 74 ?? 48 83 cb ff 48 ff c3 80 3c 19 00 75 ??"_sig },
			{ MiMallocReplacedFunctions::_strdup,            "48 89 5c 24 08 48 89 74 24 10 57 48 83 ec 30 48 8b f9 48 85 c9 74 ?? 48 83 cb ff 48 ff c3 80 3c 19 00 75 ??"_sig },
			{ MiMallocReplacedFunctions::_msize,             "48 83 ec 28 48 85 c9 75 19 E8 ?? ?? ?? ?? C7 00 16 00"_sig },
			{ MiMallocReplacedFunctions::_recalloc,          "48 89 5c 24 08 48 89 6c 24 10 48 89 74 24 18 57 48 83 ec 20 49 8b e8 48 8b da 48 8b f1 48 85 d2 74 ?? 33 d2"_sig },
			{ MiMallocReplacedFunctions::_wcsdup,            "48 89 5c 24 08 48 89 6c 24 10 48 89 74 ?? 18 57 48 83 ec 30 33 ed 48 8b f9 48 85 c9 74 38 48 83 cb ff 48 ff c3"_sig },
			{ MiMallocReplacedFunctions::_aligned_malloc,    "48 89 5c 24 08 57 48 83 ec 20 33 db 48 85 d2 74 ?? 48 8d 42 ff 48 85 c2 75 ?? 8d 43 08 48 3b d0 48 0f 47 c2"_sig },
			{ MiMallocReplacedFunctions::_aligned_free,      "48 83 ec 28 48 85 c9 74 0d 48 83 e1 f8 48 8b 49 f8"_sig },
			{ MiMallocReplacedFunctions::operator_new,       "40 53 48 83 ec 20 48 8b d9 eb 0f 48 8b cb"_sig },
			},
		functionNameMap {
				{MiMallocReplacedFunctions::malloc, "malloc"},
				{MiMallocReplacedFunctions::free, "free"},
				{MiMallocReplacedFunctions::free_base, "free_base"}, // Placeholder, actual name might differ
				{MiMallocReplacedFunctions::realloc, "realloc"},
				{MiMallocReplacedFunctions::calloc, "calloc"},
				{MiMallocReplacedFunctions::strdup, "strdup"},
				{MiMallocReplacedFunctions::realpath, "realpath"}, // Placeholder, actual name might differ
				{MiMallocReplacedFunctions::_expand, "_expand"},
				{MiMallocReplacedFunctions::_msize, "_msize"},
				{MiMallocReplacedFunctions::_recalloc, "_recalloc"},
				{MiMallocReplacedFunctions::_strdup, "_strdup"},
				{MiMallocReplacedFunctions::_strndup, "_strndup"}, // Placeholder, actual name might differ
				{MiMallocReplacedFunctions::_wcsdup, "_wcsdup"},
				{MiMallocReplacedFunctions::_mbsdup, "_mbsdup"}, // Placeholder, actual name might differ
				{MiMallocReplacedFunctions::_dupenv_s, "_dupenv_s"},
				{MiMallocReplacedFunctions::_wdupenv_s, "_wdupenv_s"},

				{MiMallocReplacedFunctions::reallocf, "reallocf"}, // Placeholder, actual name might differ
				{MiMallocReplacedFunctions::malloc_size, "malloc_size"}, // Placeholder, actual name might differ
				{MiMallocReplacedFunctions::malloc_usable_size, "malloc_usable_size"}, // Placeholder, actual name might differ
				{MiMallocReplacedFunctions::cfree, "cfree"},
				{MiMallocReplacedFunctions::valloc, "valloc"},
				{MiMallocReplacedFunctions::pvalloc, "pvalloc"}, // Placeholder, actual name might differ
				{MiMallocReplacedFunctions::reallocarray, "reallocarray"}, // Placeholder, actual name might differ
				{MiMallocReplacedFunctions::reallocarr, "reallocarr"}, // Placeholder, actual name might differ
				{MiMallocReplacedFunctions::memalign, "memalign"},
				{MiMallocReplacedFunctions::aligned_alloc, "aligned_alloc"},
				{MiMallocReplacedFunctions::posix_memalign, "posix_memalign"},
				{MiMallocReplacedFunctions::_posix_memalign, "_posix_memalign"}, // Placeholder, actual name might differ

				{MiMallocReplacedFunctions::_aligned_malloc, "_aligned_malloc"},
				{MiMallocReplacedFunctions::_aligned_realloc, "_aligned_realloc"},
				{MiMallocReplacedFunctions::_aligned_recalloc, "_aligned_recalloc"},
				{MiMallocReplacedFunctions::_aligned_msize, "_aligned_msize"},
				{MiMallocReplacedFunctions::_aligned_free, "_aligned_free"},
				{MiMallocReplacedFunctions::_aligned_offset_malloc, "_aligned_offset_malloc"}, // Placeholder, actual name might differ
				{MiMallocReplacedFunctions::_aligned_offset_realloc, "_aligned_offset_realloc"}, // Placeholder, actual name might differ
				{MiMallocReplacedFunctions::_aligned_offset_recalloc, "_aligned_offset_recalloc"}, // Placeholder, actual name might differ

				{MiMallocReplacedFunctions::operator_new, "operator new"},
				{MiMallocReplacedFunctions::operator_new_nothrow, "operator new nothrow"}, // Placeholder, actual name might differ
				{MiMallocReplacedFunctions::operator_delete, "operator delete"},
				{MiMallocReplacedFunctions::operator_deleteArr, "operator delete[]"} // Placeholder, actual name might differ
			}
	{}

	std::vector<void*> GetFunctionAdress(std::string moduleName, MiMallocReplacedFunctions function) {
		std::vector<void*> foundAdresses;

		//First check with GetProcAdress if function can be found in the Export table of the module
		void* procAddress = GetProcAddress(GetModuleHandleA(moduleName.c_str()), functionNameMap[function].c_str());
		if (procAddress != nullptr) {
			// If GetProcAddress returns a valid address, use it
			foundAdresses.push_back(procAddress);
			return foundAdresses;
		}

		//Function has not been found in the export table so use 
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
};