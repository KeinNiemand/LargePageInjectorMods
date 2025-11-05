module;

export module Configuration;
import <vector>;
import <string>;
import <map>;

#pragma warning(push)
#pragma warning(disable: 4251)

export class __declspec(dllexport) Configuration {
public:
    std::wstring LaunchPath;
    std::vector<std::string> modulesToPatch;
    int verbosity = 0; // Default verbosity level
    bool redirectConsoleOutput = false; // Default to false
    bool enableBeep = false; // Default to false
    std::map<std::wstring, std::wstring> environment;

    bool loadFromFile(const std::string& filename);

private:
    std::wstring utf8_to_wstring(const std::string& str);
};

#pragma warning(pop)