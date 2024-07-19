module;
#define WIN_WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <toml.hpp>;

export module Configuration;
export import <vector>;
export import <string>;
export import <iostream>;
export import <fstream>;


export class __declspec(dllexport) Configuration {
public:
    std::wstring LaunchPath;
    std::vector<std::string> modulesToPatch;

    bool loadFromFile(const std::string& filename);

private:
    std::wstring utf8_to_wstring(const std::string& str);
};
