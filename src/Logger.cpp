#include "Logger.h"

#include <iomanip>
#include <iostream>
#include <sstream>

// Configures whether simulated timestamps are printed.
Logger::Logger(bool timestampsEnabled)
    : timestampsEnabled_(timestampsEnabled) {}

// Writes an informational log line.
void Logger::logInfo(const std::string& message, std::chrono::milliseconds simulatedTime) const {
    writeLine("INFO", message, simulatedTime);
}

// Writes an error log line.
void Logger::logError(const std::string& message, std::chrono::milliseconds simulatedTime) const {
    writeLine("ERROR", message, simulatedTime);
}

// Starts a new cycle section in the execution trace.
void Logger::logCycleHeader(std::size_t cycleNumber, std::chrono::milliseconds simulatedTime) const {
    std::ostringstream stream;
    stream << "[Cycle " << cycleNumber << "]";
    writeLine("INFO", stream.str(), simulatedTime);
}

// Builds the deterministic simulated-time prefix for each log line.
std::string Logger::buildPrefix(std::chrono::milliseconds simulatedTime) const {
    if (!timestampsEnabled_) {
        return "";
    }

    std::ostringstream stream;
    stream << "[T+" << std::setw(4) << std::setfill('0') << simulatedTime.count() << " ms] ";
    return stream.str();
}

// Formats and prints one complete log entry.
void Logger::writeLine(const std::string& level,
                       const std::string& message,
                       std::chrono::milliseconds simulatedTime) const {
    std::cout << buildPrefix(simulatedTime)
              << "[" << level << "] "
              << message
              << std::endl;
}
