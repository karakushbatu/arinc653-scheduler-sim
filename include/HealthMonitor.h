#ifndef HEALTHMONITOR_H
#define HEALTHMONITOR_H

#include <chrono>
#include <cstddef>
#include <string>
#include <vector>

struct FaultRecord {
    std::string partitionName;
    std::size_t cycleNumber;
    std::string message;
    bool critical;
};

class Logger;

class HealthMonitor {
public:
    void recordFailure(const std::string& partitionName,
                       std::size_t cycleNumber,
                       const std::string& message,
                       bool critical);

    bool hasFailures() const;
    bool hasCriticalFailures() const;
    const std::vector<FaultRecord>& getFaultRecords() const;
    void logSummary(const Logger& logger, std::chrono::milliseconds simulatedTime) const;

private:
    std::vector<FaultRecord> faultRecords_;
};

#endif
