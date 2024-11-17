#include <Arduino.h>
#include "ConnectWithRemote.h"
#include "HandleEvents.h"
#include "GetMacAddress.h"

using namespace NuggetsInc;

ConnectWithRemote connectWithRemote;

void setup()
{
    NuggetsInc::GetMacAddress::begin(); 

    Serial.println("Device 2 (Actuator) Starting...");

    connectWithRemote.begin();
    HandleEvents::getInstance().setConnector(&connectWithRemote);

    Serial.println("Device 2 Initialized and Ready");
}

void loop()
{   
  if (Serial.available() > 0) { 
        NuggetsInc::GetMacAddress::handleSerialCommand();
    }
}