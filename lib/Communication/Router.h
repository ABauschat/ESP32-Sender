#ifndef ROUTER_H
#define ROUTER_H

#include <Arduino.h>
#include <vector>

namespace NuggetsInc {

// Minimal router - only MAC utility functions for direct communication
class Router {
public:
    Router() {}

    // Helper functions (public for use by other classes)
    static String macToString(const uint8_t mac[6]);
    static bool stringToMac(const String& s, uint8_t out[6]);
};

} // namespace NuggetsInc

#endif // ROUTER_H
