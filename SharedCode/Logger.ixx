module;

export module Logger;
export import <string>;

export __declspec(dllexport) class Logger {
public:
    enum class Level : int {
        Error = 1,
        Warning = 2,
        Info = 3,
        Debug = 4,
    };

    static __declspec(dllexport) void SetVerbosity(int verbosity);
    static __declspec(dllexport) int GetVerbosity();
    static __declspec(dllexport) void Log(Level level, const std::string& message);
    static __declspec(dllexport) void Log(Level level, const std::wstring& message);

private:
    static int verbosity_level;
};
