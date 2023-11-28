#pragma once

#include <unordered_map>
#include <string>
#include <sigmatch.hpp>

using namespace sigmatch_literals;


/// <summary>
/// An Enum filled with all the function that are supposed to be replaced by mimalloc used
/// </summary>
enum class MiMallocReplacedFunctions
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

	//protectedalloc
};


class mallocsigmatch
{
private:
	std::unordered_map<MiMallocReplacedFunctions, sigmatch::signature> functionSignatureMap = {
	{MiMallocReplacedFunctions::malloc,             "40 53 48 83 ec 20 48 8b d9 48 83 f9 ?? 77 3c 48 85 c9"_sig},
	//{MiMallocReplacedFunctions::malloc,             "e9 ?? ?? ?? ?? cc cc cc"_sig},
	{MiMallocReplacedFunctions::free_base,          "48 85 c9 74 ?? 53 48 83 ec 20 4c 8b c1 33 d2 48 8b 0d ?? ?? ?? ??"_sig},
	{MiMallocReplacedFunctions::free,               "c7 44 24 10 00 00 00 00 8b 44 24 10 e9 ?? ?? ?? ??"_sig},
	{MiMallocReplacedFunctions::realloc,            "48 89 5c 24 08 57 48 83 ec 20 48 8b da 48 8b f9 48 85 c9 75"_sig},
	//{MiMallocReplacedFunctions::realloc,            "e9 ?? ?? ?? ?? cc cc cc"_sig},
	{MiMallocReplacedFunctions::calloc,             "40 53 48 83 ec 20 4c 8b c2 48 8b d9 48 85 c9 74 ?? 33 d2"_sig},
	{MiMallocReplacedFunctions::strdup,             "48 89 5c 24 08 48 89 74 24 10 57 48 83 ec 30 48 8b f9 48 85 c9 74 ?? 48 83 cb ff 48 ff c3 80 3c 19 00 75 ??"_sig},
	{MiMallocReplacedFunctions::_strdup,            "48 89 5c 24 08 48 89 74 24 10 57 48 83 ec 30 48 8b f9 48 85 c9 74 ?? 48 83 cb ff 48 ff c3 80 3c 19 00 75 ??"_sig},
	{MiMallocReplacedFunctions::_msize,             "48 83 ec 28 48 85 c9 75 19 E8 ?? ?? ?? ?? C7 00 16 00"_sig},
	{MiMallocReplacedFunctions::_recalloc,          "48 89 5c 24 08 48 89 6c 24 10 48 89 74 24 18 57 48 83 ec 20 49 8b e8 48 8b da 48 8b f1 48 85 d2 74 ?? 33 d2"_sig},
	{MiMallocReplacedFunctions::_wcsdup,            "48 89 5c 24 08 48 89 6c 24 10 48 89 74 ?? 18 57 48 83 ec 30 33 ed 48 8b f9 48 85 c9 74 38 48 83 cb ff 48 ff c3"_sig},
	{MiMallocReplacedFunctions::cfree,              "48 8b ca e9 ?? ?? ?? ?? cc cc cc cc cc cc cc cc"_sig},
	{MiMallocReplacedFunctions::_aligned_malloc,    "48 89 5c 24 08 57 48 83 ec 20 33 db 48 85 d2 74 ?? 48 8d 42 ff 48 85 c2 75 ?? 8d 43 08 48 3b d0 48 0f 47 c2"_sig},
	{MiMallocReplacedFunctions::_aligned_free,      "48 83 ec 28 48 85 c9 74 0d 48 83 e1 f8 48 8b 49 f8"_sig},
	{MiMallocReplacedFunctions::operator_new,       "40 53 48 83 ec 20 48 8b d9 eb 0f 48 8b cb"_sig},
	{MiMallocReplacedFunctions::operator_new_nothrow,"48 83 ec 28 e8 ?? ?? ?? ?? eb 02"_sig}, //Bad Sig not version compatible


		//{MiMallocReplacedFunctions::protectedalloc,  "48 89 5c 24 08 57 48 83 ec 20 48 8b d9 b9 28 00 00 00"_sig},



	};


public:
	std::vector<void*> GetFunctionAdress(std::string moduleName,MiMallocReplacedFunctions function);
};

