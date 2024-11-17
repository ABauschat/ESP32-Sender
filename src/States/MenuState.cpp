// MenuState.cpp
#include "MenuState.h"
#include "Device.h"
#include "StateFactory.h"
#include "Application.h"
#include "Colors.h"
#include <Arduino.h>

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
    displayMenu();
}

void MenuState::onExit() {
}

void MenuState::update() {
}

void MenuState::displayMenu() {
    displayUtils->fillScreen(COLOR_BLACK);
    displayUtils->setTextColor(0xFFFF);
    displayUtils->setTextSize(2);
    displayUtils->setCursor(0, 0);
    displayUtils->println("Menu");
    displayUtils->setTextSize(1);
    for (int i = 0; i < menuItems; i++) {
        displayUtils->setCursor(0, 20 + (i * 10));
        displayUtils->println(menu[i]);
    }
}

void MenuState::executeSelection() {
}

} // namespace NuggetsInc
