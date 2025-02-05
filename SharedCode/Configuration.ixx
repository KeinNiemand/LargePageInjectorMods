module;

export module Configuration;
export import <vector>;
export import <string>;
export import <map>;


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