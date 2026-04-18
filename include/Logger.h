#ifndef LOGGER_H
#define LOGGER_H

#include <cstddef>
#include <chrono>
#include <string>

class Logger {
public:
    explicit Logger(bool timestampsEnabled);

    void logInfo(const std::string& message, std::chrono::milliseconds simulatedTime) const;
    void logError(const std::string& message, std::chrono::milliseconds simulatedTime) const;
    void logCycleHeader(std::size_t cycleNumber, std::chrono::milliseconds simulatedTime) const;

private:
    bool timestampsEnabled_;

    std::string buildPrefix(std::chrono::milliseconds simulatedTime) const;
    void writeLine(const std::string& level,
                   const std::string& message,
                   std::chrono::milliseconds simulatedTime) const;
};

#endif
