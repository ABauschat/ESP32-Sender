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
      previousMenuIndex(-1), 
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
    displayUtils->fillScreen(COLOR_BLACK);
    displayUtils->setTextColor(COLOR_WHITE);
    displayUtils->setTextSize(2);
    displayUtils->setCursor(0, 0);
    displayUtils->println("Menu");
    displayUtils->setTextSize(1);

    for (int i = 0; i < menuItems; i++) {
        redrawLine(i, i == menuIndex);
    }
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
                if (menuIndex > 0) {
                    previousMenuIndex = menuIndex;
                    menuIndex = (menuIndex - 1 + menuItems) % menuItems;
                    redrawMenuChange();
                }
                break;
            case EventType::MOVE_DOWN:
                previousMenuIndex = menuIndex;
                menuIndex = (menuIndex + 1) % menuItems;
                redrawMenuChange();
                break;
            case EventType::SELECT:
                executeSelection();
                break;
            case EventType::MOVE_LEFT:
                break;
            case EventType::MOVE_RIGHT:
                break;
            default:
                break;
        }
    }
}

void MenuState::redrawMenuChange() {
    if (previousMenuIndex != -1) {
        redrawLine(previousMenuIndex, false);
    }
    redrawLine(menuIndex, true);
}

void MenuState::redrawLine(int index, bool isSelected) {
    int y = 20 + (index * 20);

    if (isSelected) {
        displayUtils->fillRect(0, y - 2, displayUtils->ScreenWidth, 18, COLOR_WHITE);
        displayUtils->setTextColor(COLOR_BLACK);
        displayUtils->setTextSize(2);
    }
    else {
        displayUtils->fillRect(0, y - 2, displayUtils->ScreenWidth, 18, COLOR_BLACK);
        displayUtils->setTextColor(COLOR_WHITE);
        displayUtils->setTextSize(1);
    }

    displayUtils->setCursor(0, y);
    displayUtils->println(menu[index]);
    displayUtils->setTextColor(COLOR_WHITE);
}


void MenuState::executeSelection() {
    if (menuIndex == 0) {
    }
    else if (menuIndex == 1) {
    }
    else if (menuIndex == 2) {
    }
    else if (menuIndex == 3) {
    }
}

} // namespace NuggetsInc
