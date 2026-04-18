#include "HealthMonitor.h"

#include "Logger.h"

#include <sstream>

// Stores a partition fault for later reporting.
void HealthMonitor::recordFailure(const std::string& partitionName,
                                  std::size_t cycleNumber,
                                  const std::string& message,
                                  bool critical) {
    faultRecords_.push_back(FaultRecord{partitionName, cycleNumber, message, critical});
}

// Reports whether any partition fault has been recorded.
bool HealthMonitor::hasFailures() const {
    return !faultRecords_.empty();
}

// Reports whether a recorded fault belongs to a critical partition.
bool HealthMonitor::hasCriticalFailures() const {
    for (std::size_t index = 0; index < faultRecords_.size(); ++index) {
        if (faultRecords_[index].critical) {
            return true;
        }
    }

    return false;
}

// Exposes recorded faults for inspection without copying them.
const std::vector<FaultRecord>& HealthMonitor::getFaultRecords() const {
    return faultRecords_;
}

// Writes a compact execution summary for all recorded faults.
void HealthMonitor::logSummary(const Logger& logger, std::chrono::milliseconds simulatedTime) const {
    if (!hasFailures()) {
        logger.logInfo("Health Monitor Summary: no partition failures recorded.", simulatedTime);
        return;
    }

    std::ostringstream summary;
    summary << "Health Monitor Summary: " << faultRecords_.size() << " failure(s) recorded.";
    logger.logInfo(summary.str(), simulatedTime);

    for (std::size_t index = 0; index < faultRecords_.size(); ++index) {
        std::ostringstream line;
        line << "Fault #" << (index + 1U)
             << " | partition=" << faultRecords_[index].partitionName
             << " | cycle=" << faultRecords_[index].cycleNumber
             << " | critical=" << (faultRecords_[index].critical ? "yes" : "no")
             << " | message=" << faultRecords_[index].message;
        logger.logError(line.str(), simulatedTime);
    }
}
