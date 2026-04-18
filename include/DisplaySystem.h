#ifndef DISPLAYSYSTEM_H
#define DISPLAYSYSTEM_H

#include "IPartition.h"

#include <cstddef>
#include <string>

class DisplaySystem : public IPartition {
public:
    explicit DisplaySystem(std::size_t failureCycle);

    std::string getName() const override;
    bool isCritical() const override;
    std::string execute(std::size_t cycleNumber) override;

private:
    std::size_t failureCycle_;
    std::string screenData_;
};

#endif
