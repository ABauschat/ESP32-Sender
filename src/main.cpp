#include <Arduino.h>
#include "ConnectWithRemote.h"
#include "HandleEvents.h"
#include "GetMacAddress.h"
#include "DisplayUtils.h"

using namespace NuggetsInc;

ConnectWithRemote connectWithRemote;
DisplayUtils displayUtils;

void setup()
{
    Serial.begin(115200);
    while (!Serial) { ; } 
    Serial.println("Starting system...");

    NuggetsInc::GetMacAddress::begin();
    connectWithRemote.begin();
    HandleEvents::getInstance().setConnector(&connectWithRemote);
}

void loop()
{
    if (Serial.available() > 0)
    {
        NuggetsInc::GetMacAddress::handleSerialCommand();
    }

    if (connectWithRemote.isPeerConnected())
    {
        //Peer is connected
    }
}
