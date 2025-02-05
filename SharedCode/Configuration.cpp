module;
#define WIN_WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <toml.hpp>;
#include <filesystem>;

module Configuration;
import <vector>;
import <string>;
import <iostream>;
import <fstream>;
import Logger; // Import Logger


bool Configuration::loadFromFile(const std::string& filename)
{
    try {
        auto data = toml::parse(filename);

        // Parse LaunchPath
        auto launchPathStr = toml::find<std::string>(data, "LaunchPath");
        LaunchPath = utf8_to_wstring(launchPathStr);

        // Parse ModulesToPatch
        auto modules = toml::find<std::vector<std::string>>(data, "ModulesToPatch");
        modulesToPatch = modules;

        // Parse Verbosity
        verbosity = toml::find_or<int>(data, "Verbosity", 0);

        // Set verbosity in Logger
        Logger::SetVerbosity(verbosity);

        // Parse RedirectConsoleOutput
        redirectConsoleOutput = toml::find_or<bool>(data, "RedirectConsoleOutput", false);

        // Parse EnableBeep
        enableBeep = toml::find_or<bool>(data, "EnableBeep", false);

        // Parse [Environment] table
        if (data.contains("Environment")) {
            auto env_table = toml::find<toml::table>(data, "Environment");
            for (const auto& [key, value] : env_table) {
                environment[utf8_to_wstring(key)] = utf8_to_wstring(toml::get<std::string>(value));
            }
        }

        return true;
    }
    catch (const toml::syntax_error& err) {
        std::cerr << "Failed to parse TOML file: " << err.what() << "\r\n";
        return false;
    }
    catch (const std::out_of_range& err) {
        std::cerr << "Missing required key in TOML file: " << err.what() << "\r\n";
        return false;
    }
}

std::wstring Configuration::utf8_to_wstring(const std::string& str) {
	if (str.empty()) return std::wstring();

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

