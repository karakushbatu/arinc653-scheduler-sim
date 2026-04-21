#include "Scheduler.h"

#include "HealthMonitor.h"
#include "IPartition.h"
#include "Logger.h"

#include <exception>
#include <sstream>
#include <stdexcept>
#include <thread>

// Validates configuration and prepares an empty static schedule.
Scheduler::Scheduler(const Config& config, Logger& logger, HealthMonitor& healthMonitor)
    : config_(config),
      logger_(logger),
      healthMonitor_(healthMonitor),
      simulatedTime_(0) {
    config_.validate();
}

// Adds one partition window to the repeating major-cycle schedule.
void Scheduler::addPartition(std::shared_ptr<IPartition> partition, int windowMs) {
    if (!partition) {
        throw std::runtime_error("Cannot schedule a null partition.");
    }

    if (windowMs <= 0) {
        throw std::runtime_error("Window duration must be positive.");
    }

    schedule_.push_back(ScheduledPartition{partition, windowMs});
}

// Executes the configured major cycle for the requested number of iterations.
void Scheduler::run() {
    if (schedule_.empty()) {
        throw std::runtime_error("Scheduler cannot run without partitions.");
    }

    logger_.logInfo("Scheduler started.", simulatedTime_);

    for (std::size_t cycle = 1; cycle <= config_.cycleCount; ++cycle) {
        logger_.logCycleHeader(cycle, simulatedTime_);

        for (std::size_t index = 0; index < schedule_.size(); ++index) {
            executePartition(schedule_[index], cycle);
            simulatedTime_ += std::chrono::milliseconds(schedule_[index].windowMs);

            if (healthMonitor_.hasCriticalFailures()) {
                logger_.logError("Critical failure detected. Scheduler stopped.", simulatedTime_);
                healthMonitor_.logSummary(logger_, simulatedTime_);
                logger_.logInfo("Scheduler finished.", simulatedTime_);
                return;
            }
        }
    }

    healthMonitor_.logSummary(logger_, simulatedTime_);
    logger_.logInfo("Scheduler finished.", simulatedTime_);
}

// Runs one partition window and isolates any partition-level failure.
void Scheduler::executePartition(const ScheduledPartition& scheduledPartition, std::size_t cycleNumber) {
    std::ostringstream startMessage;
    startMessage << "Active partition: " << scheduledPartition.partition->getName()
                 << " | window=" << scheduledPartition.windowMs << " ms";
    logger_.logInfo(startMessage.str(), simulatedTime_);

    const std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

    try {
        const std::string result = scheduledPartition.partition->execute(cycleNumber);
        logger_.logInfo(result, simulatedTime_);
    } catch (const std::exception& exception) {
        std::ostringstream errorMessage;
        errorMessage << scheduledPartition.partition->getName()
                     << " ERROR: " << exception.what();
        logger_.logError(errorMessage.str(), simulatedTime_);

        healthMonitor_.recordFailure(scheduledPartition.partition->getName(),
                                     cycleNumber,
                                     exception.what(),
                                     scheduledPartition.partition->isCritical());
    } catch (...) {
        const std::string unknownError = "Unknown failure.";
        std::ostringstream errorMessage;
        errorMessage << scheduledPartition.partition->getName()
                     << " ERROR: " << unknownError;
        logger_.logError(errorMessage.str(), simulatedTime_);

        healthMonitor_.recordFailure(scheduledPartition.partition->getName(),
                                     cycleNumber,
                                     unknownError,
                                     scheduledPartition.partition->isCritical());
    }

    waitForWindow(startTime, scheduledPartition.windowMs);
}

// Sleeps only for the remaining time in the current static window.
void Scheduler::waitForWindow(std::chrono::steady_clock::time_point startTime, int windowMs) const {
    if (!config_.emulateTiming) {
        return;
    }

    const std::chrono::milliseconds windowDuration(windowMs);
    const std::chrono::steady_clock::duration elapsed = std::chrono::steady_clock::now() - startTime;
    const std::chrono::steady_clock::duration target = std::chrono::duration_cast<std::chrono::steady_clock::duration>(windowDuration);

    if (elapsed < target) {
        std::this_thread::sleep_for(target - elapsed);
    }
}
