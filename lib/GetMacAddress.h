#ifndef GET_MAC_ADDRESS_H
#define GET_MAC_ADDRESS_H

#include <Arduino.h>
#include <WiFi.h>

namespace NuggetsInc {

class GetMacAddress {
public:
    static void begin();
    static String fetch();
    static void handleSerialCommand();
};

} // namespace NuggetsInc

#endif // GET_MAC_ADDRESS_H
