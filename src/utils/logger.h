#pragma once

#define DEBUG_LOG(PRIORITY, MESSAGE) Logger::Write(PRIORITY, MESSAGE);

#include <string>

namespace dw {

class Logger {
public:
    enum Severity {
        DEBUG,
        CONFIG,
        INFO,
        WARNING,
        ERR
    };

    Logger() = delete;
    Logger(const Logger& logger) = delete;
    Logger& operator= (const Logger& logger) = delete;

    static void Init(const Severity minSeverity, const std::string&& logFileName);
    static void Write(const Severity severity, const std::string& message);
    static void Destroy();

private:
	static Severity m_minSeverity;
};

} // namespace dw