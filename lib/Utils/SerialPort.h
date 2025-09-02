#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <Arduino.h>
#include <WiFi.h>

namespace NuggetsInc {

class SerialPort {
public:
    static void begin();
    static String fetchMacAdress();
    static void handleIncommingSerialData();
    static void help();
    static void version();
};

} // namespace NuggetsInc

#endif // SERIAL_PORT_H
