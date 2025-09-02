#include "MenuState.h"
#include "Communication/HandleEvents.h"
#include "Application.h"
#include "StateFactory.h"
#include "Utils/Device.h"
#include "Utils/MenuService.h"
#include <Arduino.h>
#include "Communication/Node.h"
#include <Wire.h>

// Declare a static global instance of HX711 within the NuggetsInc namespace.
namespace NuggetsInc
{
}

namespace NuggetsInc
{

MenuState* MenuState::activeInstance = nullptr;

    MenuState::MenuState()
        : menuService(new MenuService())
    {
        setupMenu();
    }

    MenuState::~MenuState()
    {
        delete menuService;
    }

    void MenuState::setupMenu()
    {
        menuService->createMenu(DeviceName);

        menuService->addMenuItem("Auto Test", [this]() {
            Node* connector = Node::getActiveInstance();
            if (connector) {
                connector->setRouteMode(Node::RouteMode::AUTO);
            }
            menuService->drawMenu("Auto Mode");
        });

        menuService->addMenuItem("Direct Test", [this]() {
            Node* connector = Node::getActiveInstance();
            if (connector) {
                connector->setRouteMode(Node::RouteMode::DIRECT);
            }
            menuService->drawMenu("Direct Mode");
        });

        menuService->addMenuItem("Routed Test", [this]() {
            Node* connector = Node::getActiveInstance();
            if (connector) {
                connector->setRouteMode(Node::RouteMode::ROUTED);
            }
            menuService->drawMenu("Routed Mode");
        });
    }

    void MenuState::onEnter()
    {
        activeInstance = this;
        menuService->drawMenu();
    }

    void MenuState::onExit()
    {
        activeInstance = nullptr;
        menuService->clearScreen();
    }

    void MenuState::update()
    {
        Event event;
        QueueHandle_t eventQueue = HandleEvents::getInstance().getEventQueue();

        if (xQueueReceive(HandleEvents::getInstance().getEventQueue(), &event, 0) == pdPASS)
        {
            uint32_t nowMs = millis();
            if ((uint32_t)(nowMs - lastHandledMs) < UI_COOLDOWN_MS) {
                return;
            }
            lastHandledMs = nowMs;

            switch (event.type)
            {
            case EventType::MOVE_UP:
                menuService->moveUp();
                break;
            case EventType::MOVE_DOWN:
                menuService->moveDown();
                break;
            case EventType::SELECT:
                executeSelection();
                break;
            case EventType::BACK:
                onExitSelection();
                break;
            case EventType::BOOOP:
                onEnter();
                break;
            }
        }
    }

    void MenuState::executeSelection()
    {
        menuService->selectCurrent();
    }

    void MenuState::onExitSelection()
    {
        menuService->drawMenu();
    }

    MenuState* MenuState::getActiveInstance() {
        return activeInstance;
    }

} // namespace NuggetsInc
