#include <Arduino.h>
#include "ConnectWithRemote.h"
#include "HandleEvents.h"
#include "GetMacAddress.h"

using namespace NuggetsInc;

ConnectWithRemote connectWithRemote;

void setup()
{
    Serial.println("Sender Device Starting...");
    // Initialize serial communication
    NuggetsInc::GetMacAddress::begin();

    // Initialize ESP-NOW
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