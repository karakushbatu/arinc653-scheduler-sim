#ifndef IPARTITION_H
#define IPARTITION_H

#include <cstddef>
#include <string>

class IPartition {
public:
    virtual ~IPartition() {}

    virtual std::string getName() const = 0;
    virtual bool isCritical() const = 0;
    virtual std::string execute(std::size_t cycleNumber) = 0;
};

#endif
