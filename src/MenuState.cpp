// MenuState.cpp
#include "MenuState.h"
#include "Device.h"
#include "StateFactory.h"
#include "Application.h"
#include "Colors.h"
#include <Arduino.h>
#include "ConnectWithRemote.h"
#include "HandleEvents.h"
#include "GetMacAddress.h"
#include "DisplayUtils.h"

namespace NuggetsInc {

MenuState::MenuState()
    : menuIndex(0),
      displayUtils(nullptr) {
    menu[0] = "Connect To Device";
    menu[1] = "Setup NFC Chip";
    menu[2] = "Clone NFC Chip";
    menu[3] = "Play Snake Game";
}

MenuState::~MenuState() {
    delete displayUtils;
}

void MenuState::onEnter() {
    Serial.begin(115200);
    while (!Serial) { ; } 
    Serial.println("Starting system...");

    NuggetsInc::GetMacAddress::begin();
    connectWithRemote.begin();
    HandleEvents::getInstance().setConnector(&connectWithRemote);
}

void MenuState::onExit() {

}

void MenuState::update() {
    if (Serial.available() > 0)
    {
        NuggetsInc::GetMacAddress::handleSerialCommand();
    }
}

void MenuState::displayMenu() {
   
}

void MenuState::executeSelection() {
   
}

} // namespace NuggetsInc
