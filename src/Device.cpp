#include "Device.h"

namespace NuggetsInc {

Device& Device::getInstance() {
    static Device instance;
    return instance;
}

Device::Device() {}

void Device::init() {

}

void Device::update() {
   
}

} // namespace NuggetsInc
