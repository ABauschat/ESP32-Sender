#include <Arduino.h>
#include "ConnectWithRemote.h"
#include "HandleEvents.h"
#include "GetMacAddress.h"
#include "DisplayUtils.h" // Include DisplayUtils

using namespace NuggetsInc;

// Instantiate the communication and utility classes
ConnectWithRemote connectWithRemote;
DisplayUtils displayUtils;

void setup()
{
    // Initialize Serial communication
    Serial.begin(115200);
    while (!Serial) { ; } // Wait for Serial to be ready

    Serial.println("Starting system...");

    // Initialize MAC address retrieval (assuming implementation exists)
    NuggetsInc::GetMacAddress::begin();

    // Begin ESP-NOW communication
    connectWithRemote.begin();

    // Set the connector for HandleEvents
    HandleEvents::getInstance().setConnector(&connectWithRemote);
}

void loop()
{
    // Check for incoming serial commands to get MAC address (assuming implementation exists)
    if (Serial.available() > 0)
    {
        NuggetsInc::GetMacAddress::handleSerialCommand();
    }

}
