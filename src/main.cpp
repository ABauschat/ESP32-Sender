#include <Arduino.h>
#include "ConnectWithRemote.h"
#include "HandleEvents.h"
#include "GetMacAddress.h"

using namespace NuggetsInc;

// Create instances of ConnectWithRemote and HandleEvents
ConnectWithRemote connectWithRemote;

void setup()
{
    NuggetsInc::GetMacAddress::begin(); 

    Serial.println("Device 2 (Actuator) Starting...");

    // Initialize ESP-NOW communication
    connectWithRemote.begin();

    // Set the connector for HandleEvents
    HandleEvents::getInstance().setConnector(&connectWithRemote);

    Serial.println("Device 2 Initialized and Ready");
}

void loop()
{   
  if (Serial.available() > 0) { 
        NuggetsInc::GetMacAddress::handleSerialCommand();
    }
}