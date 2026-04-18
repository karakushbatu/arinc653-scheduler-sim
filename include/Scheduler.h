#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "Config.h"

#include <chrono>
#include <memory>
#include <vector>

class HealthMonitor;
class IPartition;
class Logger;

class Scheduler {
public:
    Scheduler(const Config& config, Logger& logger, HealthMonitor& healthMonitor);

    void addPartition(std::shared_ptr<IPartition> partition, int windowMs);
    void run();

private:
    struct ScheduledPartition {
        std::shared_ptr<IPartition> partition;
        int windowMs;
    };

    Config config_;
    Logger& logger_;
    HealthMonitor& healthMonitor_;
    std::vector<ScheduledPartition> schedule_;
    std::chrono::milliseconds simulatedTime_;

    void executePartition(const ScheduledPartition& scheduledPartition, std::size_t cycleNumber);
    void waitForWindow(std::chrono::steady_clock::time_point startTime, int windowMs) const;
};

#endif
