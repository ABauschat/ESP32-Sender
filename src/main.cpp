#include <Arduino.h>
#include "ConnectWithRemote.h"
#include "HandleEvents.h"
#include "GetMacAddress.h"

using namespace NuggetsInc;

ConnectWithRemote connectWithRemote;

void setup()
{
    Serial.begin(115200);
    while (!Serial) { ; }

    NuggetsInc::GetMacAddress::begin();
    connectWithRemote.begin();
    HandleEvents::getInstance().setConnector(&connectWithRemote);
}

void loop()
{
    // Check for incoming serial commands to get MAC address
    if (Serial.available() > 0)
    {
        NuggetsInc::GetMacAddress::handleSerialCommand();
    }
}
