// main.cpp
#include <Arduino.h>
#include "Application.h"
#include "Utils/SerialPort.h"
#include "Communication/Node.h"
#include "Communication/HandleEvents.h"
#include "Utils/DisplayUtils.h"
#include "StateFactory.h"
#include "Applications/MenuState.h"

using namespace NuggetsInc;
Node node;

void setup() {
    // Initialize Serial first with delay for stability
    Serial.begin(115200);
    delay(2000); // Give extra time for serial to stabilize

    Serial.println("=== ESP32 STARTUP DEBUG ===");
    Serial.println("Serial initialized successfully");
    Serial.flush();

    try {
        // Initialize Serial Communication
        Serial.println("Calling SerialPort::begin()...");
        Serial.flush();
        NuggetsInc::SerialPort::begin();
        Serial.println("SerialPort::begin() completed");
        Serial.flush();

        // Initilize Node Networking
        Serial.println("Calling node.begin()...");
        Serial.flush();
        node.begin();
        Serial.println("node.begin() completed");
        Serial.flush();

        // Initialize The Application
        Serial.println("Calling Application::getInstance().init()...");
        Serial.flush();
        Application::getInstance().init();
        Serial.println("Application init completed");
        Serial.flush();

        Serial.println("Creating MENU_STATE...");
        Serial.flush();
        Application::getInstance().changeState(StateFactory::createState(StateType::MENU_STATE));
        Serial.println("=== SETUP COMPLETE ===");
        Serial.flush();
    } catch (...) {
        Serial.println("EXCEPTION CAUGHT IN SETUP!");
        Serial.flush();
    }
}

void loop() {
    static unsigned long lastHeartbeat = 0;
    static bool firstLoop = true;

    if (firstLoop) {
        Serial.println("=== ENTERING MAIN LOOP ===");
        Serial.flush();
        firstLoop = false;
    }

    // Handle incoming serial data
    if (Serial.available() > 0)
    {
        Serial.println("Serial data received");
        NuggetsInc::SerialPort::handleIncommingSerialData();
    }

    // Initial Connection Made, Reset The Menu
    if (node.InitializeMenu) {
        node.InitializeMenu = false;
        Serial.println("Switching to MENU_STATE");

        Application::getInstance().changeState(StateFactory::createState(StateType::MENU_STATE));
    }

    // Handle Application Events
    try {
        Application::getInstance().run();
    } catch (...) {
        Serial.println("Exception in Application::run()!");
    }

    // Yield to allow other tasks to run
    // This is important in FreeRTOS to prevent blocking
    yield();
}
