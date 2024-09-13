module;
#include <iostream>
module Logger;

int Logger::verbosity_level = 0;

void Logger::SetVerbosity(int verbosity) {
    verbosity_level = verbosity;
}

int Logger::GetVerbosity() {
    return verbosity_level;
}

void Logger::Log(Level level, const std::string& message) {
    if (static_cast<int>(level) <= verbosity_level) {
        std::cout << message << std::endl;
    }
}

void Logger::Log(Level level, const std::wstring& message) {
    if (static_cast<int>(level) <= verbosity_level) {
        std::wcout << message << std::endl;
    }
}
