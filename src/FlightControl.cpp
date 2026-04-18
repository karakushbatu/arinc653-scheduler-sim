#include "FlightControl.h"

#include <iomanip>
#include <sstream>

// Initializes deterministic baseline flight parameters.
FlightControl::FlightControl()
    : altitudeMeters_(1000.0),
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
