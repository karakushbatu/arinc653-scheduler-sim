#include "DisplaySystem.h"

#include <sstream>
#include <stdexcept>

// Initializes the display state and the configured failure point.
DisplaySystem::DisplaySystem(std::size_t failureCycle)
    : failureCycle_(failureCycle),
      screenData_("Display initialized") {}

// Returns the partition label used by the scheduler.
std::string DisplaySystem::getName() const {
    return "Display System";
}

// Marks this partition as non-critical.
bool DisplaySystem::isCritical() const {
    return false;
}

// Updates display data and injects a controlled fault on the selected cycle.
std::string DisplaySystem::execute(std::size_t cycleNumber) {
    if (cycleNumber == failureCycle_) {
        throw std::runtime_error("Display refresh failure detected.");
    }

    std::ostringstream builder;
    builder << "Primary Flight Display frame updated for cycle " << cycleNumber;
    screenData_ = builder.str();

    std::ostringstream output;
    output << getName() << " running | cycle=" << cycleNumber
           << " | screenData=\"" << screenData_ << "\"";
    return output.str();
}
