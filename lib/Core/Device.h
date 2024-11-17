#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>

namespace NuggetsInc {

class Device {
public:
    static Device& getInstance();

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    void init();

    void update();

private:
    Device(); 
};

} // namespace NuggetsInc

#endif // DEVICE_H
