#ifndef CONFIG_H
#define CONFIG_H

#include <cstddef>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

struct Config {
    std::size_t cycleCount;
    int majorCycleMs;
    int flightControlWindowMs;
    int displayWindowMs;
    std::size_t flightControlFailureCycle;
    std::size_t displayFailureCycle;
    bool timestampsEnabled;
    bool emulateTiming;

    Config()
        : cycleCount(5),
          majorCycleMs(100),
          flightControlWindowMs(60),
          displayWindowMs(40),
          flightControlFailureCycle(0),
          displayFailureCycle(3),
          timestampsEnabled(true),
          emulateTiming(true) {}

    static Config fromFile(const std::string& path) {
        Config config;
        std::ifstream input(path.c_str());
        if (!input.is_open()) {
            return config;
        }

        std::string line;
        while (std::getline(input, line)) {
            if (line.empty() || line[0] == '#') {
                continue;
            }

            const std::size_t delimiter = line.find('=');
            if (delimiter == std::string::npos) {
                continue;
            }

            const std::string key = trim(line.substr(0, delimiter));
            const std::string value = trim(line.substr(delimiter + 1));

            if (key == "cycle_count") {
                config.cycleCount = parseSizeT(value, key);
            } else if (key == "major_cycle_ms") {
                config.majorCycleMs = parseInt(value, key);
            } else if (key == "flight_control_window_ms") {
                config.flightControlWindowMs = parseInt(value, key);
            } else if (key == "display_window_ms") {
                config.displayWindowMs = parseInt(value, key);
            } else if (key == "flight_control_failure_cycle") {
                config.flightControlFailureCycle = parseSizeT(value, key);
            } else if (key == "display_failure_cycle") {
                config.displayFailureCycle = parseSizeT(value, key);
            } else if (key == "timestamps_enabled") {
                config.timestampsEnabled = parseBool(value, key);
            } else if (key == "emulate_timing") {
                config.emulateTiming = parseBool(value, key);
            }
        }

        config.validate();
        return config;
    }

    void validate() const {
        if (cycleCount == 0U) {
            throw std::runtime_error("cycleCount must be greater than zero.");
        }

        if (majorCycleMs <= 0 || flightControlWindowMs <= 0 || displayWindowMs <= 0) {
            throw std::runtime_error("Timing values must be positive.");
        }

        if (flightControlWindowMs + displayWindowMs != majorCycleMs) {
            throw std::runtime_error("Flight Control and Display windows must sum to the major cycle time.");
        }

        if (flightControlFailureCycle > cycleCount) {
            throw std::runtime_error("flightControlFailureCycle must be zero or within the configured cycle count.");
        }

        if (displayFailureCycle > cycleCount) {
            throw std::runtime_error("displayFailureCycle must be zero or within the configured cycle count.");
        }
    }

private:
    static std::string trim(const std::string& text) {
        const std::size_t start = text.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) {
            return "";
        }

        const std::size_t end = text.find_last_not_of(" \t\r\n");
        return text.substr(start, end - start + 1U);
    }

    static int parseInt(const std::string& value, const std::string& key) {
        std::istringstream stream(value);
        int parsedValue = 0;
        stream >> parsedValue;
        if (!stream || !stream.eof()) {
            throw std::runtime_error("Invalid integer value for key: " + key);
        }
        return parsedValue;
    }

    static std::size_t parseSizeT(const std::string& value, const std::string& key) {
        std::istringstream stream(value);
        std::size_t parsedValue = 0U;
        stream >> parsedValue;
        if (!stream || !stream.eof()) {
            throw std::runtime_error("Invalid size_t value for key: " + key);
        }
        return parsedValue;
    }

    static bool parseBool(const std::string& value, const std::string& key) {
        if (value == "true" || value == "1") {
            return true;
        }

        if (value == "false" || value == "0") {
            return false;
        }

        throw std::runtime_error("Invalid boolean value for key: " + key);
    }
};

#endif
