module;
export module Logger;
export import <string>;
export import <iostream>;

export class Logger {
public:
    enum class Level : int {
        Error = 1,
        Warning = 2,
        Info = 3,
        Debug = 4,
    };

    static void SetVerbosity(int verbosity);
    static int GetVerbosity();
    static void Log(Level level, const std::string& message);
    static void Log(Level level, const std::wstring& message);

private:
    static int verbosity_level;
};
