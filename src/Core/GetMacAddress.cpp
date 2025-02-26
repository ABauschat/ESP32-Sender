#include "GetMacAddress.h"

namespace NuggetsInc {

void GetMacAddress::begin() {
    Serial.begin(115200);
    Serial.println("ESP32 Ready");
}

String GetMacAddress::fetch() {
    return WiFi.macAddress();
}

void GetMacAddress::handleSerialCommand() {
    if (Serial.available() > 0) { 
        String command = Serial.readStringUntil('\n'); 
        command.trim();
        if (command.equalsIgnoreCase("GET_MAC")) {
            Serial.println(fetch()); 
        } else {
            Serial.println("UNKNOWN_COMMAND"); 
        }
    }
}

} // namespace NuggetsInc
