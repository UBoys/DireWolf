#include "logger.h"

#include <fstream>
#include <iostream>
#include <cassert>

namespace {
    const std::string SEVERITY_MAP[] = {
        "DEBUG",
        "CONFIG",
        "INFO",
        "WARNING",
        "ERROR"
    };

	std::ofstream s_fileStream;
	dw::Logger::Severity s_minSeverity;
	bool s_isInitialized = false;
}

namespace dw {

void Logger::Init(const Severity minSeverity, const std::string&& logFileName) {
    assert(!s_isInitialized && "You should only initialize this class once\n");

    s_isInitialized = true;
	s_minSeverity = minSeverity;

	if (!logFileName.empty()) {
		s_fileStream.open(logFileName.c_str());
	}
}

// TODO: Make use of streams instead of expensive concatenation of strings
void Logger::Write(const Severity severity, const std::string& message) {
	assert(s_isInitialized && "You must initialize this class before using this function\n");

	if (severity >= s_minSeverity) {
		std::ostream& stream = s_fileStream.is_open() ? s_fileStream : std::cout;
		stream << "DW " << SEVERITY_MAP[severity] << ": " << message << std::endl;
	}
}

void Logger::Destroy() {
	assert(s_isInitialized && "This class has not been initialized\n");

	s_isInitialized = false;

	if (s_fileStream.is_open()) {
		s_fileStream.close();
	}
}

} // namespace dw