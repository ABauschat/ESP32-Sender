#include <Arduino.h>
#include "Application.h"
#include "GetMacAddress.h"
#include "ConnectWithRemote.h"
#include "HandleEvents.h"
#include "GetMacAddress.h"
#include "DisplayUtils.h"

using namespace NuggetsInc;

DisplayUtils* displayUtils;
ConnectWithRemote connectWithRemote;

void setup() {
    Serial.begin(115200);
    while (!Serial) { ; } 
    Serial.println("Starting system...");
    NuggetsInc::GetMacAddress::begin();
    connectWithRemote.begin();
    HandleEvents::getInstance().setConnector(&connectWithRemote);
    Application::getInstance().init();
}

void loop() {
    if (Serial.available() > 0)
    {
        NuggetsInc::GetMacAddress::handleSerialCommand();
    }

    Application::getInstance().run();
}
