// main.cpp
#include <Arduino.h>
#include "Application.h"
#include "GetMacAddress.h"
#include "ConnectWithRemote.h"
#include "HandleEvents.h"
#include "DisplayUtils.h"
#include "StateFactory.h"

using namespace NuggetsInc;

ConnectWithRemote connectWithRemote;

void setup() {
    Serial.begin(115200);
    //tunr on led
    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);
    
    Serial.println("Starting up...");
    NuggetsInc::GetMacAddress::begin();
    connectWithRemote.begin();
    NuggetsInc::HandleEvents::getInstance().setConnector(&connectWithRemote);
    Application::getInstance().init();
    Application::getInstance().changeState(StateFactory::createState(StateType::MENU_STATE));
}

void loop() {
    if (Serial.available() > 0)
    {
        NuggetsInc::GetMacAddress::handleSerialCommand();
    }

    if (connectWithRemote.shouldStartMainMenu) {
        connectWithRemote.shouldStartMainMenu = false; 
        Application::getInstance().changeState(StateFactory::createState(StateType::MENU_STATE));
    }

    Application::getInstance().run();

    yield();
}
