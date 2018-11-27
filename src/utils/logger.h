#pragma once

#define LOG(SEVERITY, MESSAGE) Logger::Write(SEVERITY, MESSAGE);

#define LOGD(MESSAGE) Logger::Write(Logger::DEBUG, MESSAGE);
#define LOGE(MESSAGE) Logger::Write(Logger::ERR, MESSAGE);
#define LOGI(MESSAGE) Logger::Write(Logger::INFO, MESSAGE);
#define LOGW(MESSAGE) Logger::Write(Logger::WARNING, MESSAGE);
#define LOGC(MESSAGE) Logger::Write(Logger::CONFIG, MESSAGE);

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