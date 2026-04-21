#include "Config.h"
#include "DisplaySystem.h"
#include "FlightControl.h"
#include "HealthMonitor.h"
#include "Logger.h"
#include "Scheduler.h"

#include <exception>
#include <iostream>
#include <memory>

// Builds the system components, installs the static schedule, and starts the simulation.
int main() {
    try {
        const Config config = Config::fromFile("config.txt");

        Logger logger(config.timestampsEnabled);
        HealthMonitor healthMonitor;
        Scheduler scheduler(config, logger, healthMonitor);

        std::shared_ptr<IPartition> flightControl(new FlightControl(config.flightControlFailureCycle));
        std::shared_ptr<IPartition> displaySystem(new DisplaySystem(config.displayFailureCycle));

        scheduler.addPartition(flightControl, config.flightControlWindowMs);
        scheduler.addPartition(displaySystem, config.displayWindowMs);
        scheduler.run();
    } catch (const std::exception& exception) {
        std::cerr << "Fatal error: " << exception.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Fatal error: unknown exception." << std::endl;
        return 1;
    }

    return 0;
}
