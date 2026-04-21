#include "FlightControl.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>

// Initializes deterministic baseline flight parameters and an optional failure point.
FlightControl::FlightControl(std::size_t failureCycle)
    : failureCycle_(failureCycle),
      altitudeMeters_(1000.0),
      velocityMetersPerSecond_(250.0) {}

// Returns the partition label used by the scheduler.
std::string FlightControl::getName() const {
    return "Flight Control";
}

// Marks this partition as safety-critical.
bool FlightControl::isCritical() const {
    return true;
}

// Advances the private flight state and returns a status line.
std::string FlightControl::execute(std::size_t cycleNumber) {
    if (failureCycle_ != 0U && cycleNumber == failureCycle_) {
        throw std::runtime_error("Flight control computation failure detected.");
    }

    const double altitudeDelta = 12.5;
    const double velocityDelta = 0.8;

    altitudeMeters_ += altitudeDelta;
    velocityMetersPerSecond_ -= velocityDelta;

    std::ostringstream output;
    output << std::fixed << std::setprecision(1);
    output << getName()
           << " running | cycle=" << cycleNumber
           << " | altitude=" << altitudeMeters_ << " m"
           << " | velocity=" << velocityMetersPerSecond_ << " m/s";
    return output.str();
}
