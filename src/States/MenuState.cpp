// MenuState.cpp
#include "MenuState.h"
#include "HandleEvents.h"
#include "Application.h"
#include "StateFactory.h"
#include "Colors.h"
#include "Device.h"
#include <Arduino.h>

namespace NuggetsInc {

MenuState::MenuState()
    : menuIndex(0),
       displayUtils(new DisplayUtils())
{
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
    displayUtils->clearDisplay();
}

void MenuState::update() {
    QueueHandle_t eventQueue = HandleEvents::getInstance().getEventQueue();
    Event event;

    while (xQueueReceive(eventQueue, &event, 0) == pdPASS) {
        switch (event.type) {
            case EventType::MOVE_UP:
                menuIndex = (menuIndex - 1 + menuItems) % menuItems;
                Serial.println("MOVE_UP event processed");
                displayMenu();
                break;
            case EventType::MOVE_DOWN:
                menuIndex = (menuIndex + 1) % menuItems;
                Serial.println("MOVE_DOWN event processed");
                displayMenu();
                break;
            case EventType::SELECT:
                Serial.println("SELECT event processed");
                executeSelection();
                break;
            case EventType::MOVE_LEFT:
                Serial.println("MOVE_LEFT event received but not implemented");
                break;
            case EventType::MOVE_RIGHT:
                Serial.println("MOVE_RIGHT event received but not implemented");
                break;
            default:
                Serial.println("UNKNOWN_EVENT received in MenuState");
                break;
        }
    }
}

void MenuState::displayMenu() {
    displayUtils->fillScreen(COLOR_BLACK);
    displayUtils->setTextColor(COLOR_WHITE);
    displayUtils->setTextSize(2);
    displayUtils->setCursor(0, 0);
    displayUtils->println("Menu");
    displayUtils->setTextSize(1);
    
    for (int i = 0; i < menuItems; i++) {
        if (i == menuIndex) {
            displayUtils->fillRect(0, 20 + (i * 20) - 2, 128, 18, COLOR_WHITE); 
            displayUtils->setTextColor(COLOR_BLACK); 
        }
        else {
            displayUtils->setTextColor(COLOR_WHITE);
        }
        displayUtils->setCursor(0, 20 + (i * 20));
        displayUtils->println(menu[i]);
    }
}

void MenuState::executeSelection() {
    Serial.print("Executing selection: ");
    Serial.println(menu[menuIndex]);

    if (menuIndex == 0) {
        Serial.println("Action: Connect To Device");
    }
    else if (menuIndex == 1) {
        Serial.println("Action: Setup NFC Chip");
    }
    else if (menuIndex == 2) {
        Serial.println("Action: Clone NFC Chip");
    }
    else if (menuIndex == 3) {
        Serial.println("Action: Play Snake Game");
    }
}

} // namespace NuggetsInc
