module;

#include <mimalloc.h>
#include <mimalloc-override.h>
#include <mimalloc-new-delete.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#define WIN_WIN32_LEAN_AND_MEAN
#include <Windows.h>

export module Configuration;

export class Configuration {
public:
    std::wstring LaunchPath;
    std::vector<std::string> modulesToPatch;

    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Unable to open file: " << filename << "\r\n";
            return false;
        }

        std::string line;

        // First line for LaunchPath
        if (std::getline(file, line)) {
            LaunchPath = utf8_to_wstring(line);
        }
        else {
            return false;
        }

        // Subsequent lines for modulesToPatch
        while (std::getline(file, line)) {
            modulesToPatch.push_back(line);
        }

        file.close();
        return true;
    }

private:
    std::wstring utf8_to_wstring(const std::string& str) {
        if (str.empty()) return std::wstring();

        int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
        std::wstring wstrTo(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
        return wstrTo;
    }
};
