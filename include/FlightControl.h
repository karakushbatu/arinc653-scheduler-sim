#ifndef FLIGHTCONTROL_H
#define FLIGHTCONTROL_H

#include "IPartition.h"

class FlightControl : public IPartition {
public:
    explicit FlightControl(std::size_t failureCycle);

    std::string getName() const override;
    bool isCritical() const override;
    std::string execute(std::size_t cycleNumber) override;

private:
    std::size_t failureCycle_;
    double altitudeMeters_;
    double velocityMetersPerSecond_;
};

#endif
