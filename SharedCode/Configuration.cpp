module;
#define WIN_WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <toml.hpp>;

module Configuration;
import <vector>;
import <string>;
import <iostream>;
import <fstream>;

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

