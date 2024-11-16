#include "GetMacAddress.h"

void setup() {
    NuggetsInc::GetMacAddress::begin();
}

void loop() {
    if (Serial.available() > 0) { 
        NuggetsInc::GetMacAddress::handleSerialCommand();
    }
}
