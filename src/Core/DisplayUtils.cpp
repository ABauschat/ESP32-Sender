// DisplayUtils.cpp
#include "DisplayUtils.h"
#include "ConnectWithRemote.h"
#include "HandleEvents.h"
#include "MessageTypes.h"

namespace NuggetsInc {

DisplayUtils::DisplayUtils() {}
DisplayUtils::~DisplayUtils() {}

void DisplayUtils::sendCommand(const char* command, const char* data) { 
    struct_message message;
    strcpy(message.messageType, "command");
    strncpy(message.command, command, sizeof(message.command) - 1);
    message.command[sizeof(message.command) - 1] = '\0'; 

    if (data != nullptr)
    {
        strncpy(message.data, data, sizeof(message.data) - 1);
        message.data[sizeof(message.data) - 1] = '\0'; 
    }
    else
    {
        message.data[0] = '\0'; 
    }

    ConnectWithRemote* connector = ConnectWithRemote::getActiveInstance();
    if (connector && connector->isPeerConnected()) {
        if (connector->sendDataBlocking(message))
        {
            Serial.print("Sent command: ");
            Serial.print(command);
            if (data != nullptr)
            {
                Serial.print(" with data: ");
                Serial.println(data);
            }
            else
            {
                Serial.println();
            }
        }
        else
        {
            Serial.println("Failed to send command after retries");
        }
    }
    else {
        Serial.println("Cannot send command: Connector not active or peer not connected");
    }
}

void DisplayUtils::clearDisplay() {
    sendCommand("CLEAR_DISPLAY");
}

void DisplayUtils::displayMessage(const String& message) {
    sendCommand("DISPLAY_MESSAGE", message.c_str());
}

void DisplayUtils::newTerminalDisplay(const String& message) {
    sendCommand("NEW_TERMINAL_DISPLAY", message.c_str());
}

void DisplayUtils::addToTerminalDisplay(const String& message) {
    sendCommand("ADD_TO_TERMINAL_DISPLAY", message.c_str());
}

void DisplayUtils::println(const String& message) {
    sendCommand("PRINTLN", message.c_str());
}

void DisplayUtils::print(const String& message) {
    sendCommand("PRINT", message.c_str());
}

void DisplayUtils::setCursor(int16_t x, int16_t y) {
    char dataBuffer[20];
    snprintf(dataBuffer, sizeof(dataBuffer), "%d,%d", x, y);
    sendCommand("SET_CURSOR", dataBuffer);
}

void DisplayUtils::setTextSize(uint8_t size) {
    char dataBuffer[10];
    snprintf(dataBuffer, sizeof(dataBuffer), "%d", size);
    sendCommand("SET_TEXT_SIZE", dataBuffer);
}

void DisplayUtils::setTextColor(uint16_t color) {
    char dataBuffer[10];
    snprintf(dataBuffer, sizeof(dataBuffer), "%d", color);
    sendCommand("SET_TEXT_COLOR", dataBuffer);
}

void DisplayUtils::fillScreen(uint16_t color) {
    char dataBuffer[10];
    snprintf(dataBuffer, sizeof(dataBuffer), "%d", color);
    sendCommand("FILL_SCREEN", dataBuffer);
}

void DisplayUtils::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    char dataBuffer[30];
    snprintf(dataBuffer, sizeof(dataBuffer), "%d,%d,%d,%d,%d", x, y, w, h, color);
    sendCommand("DRAW_RECT", dataBuffer);
}

void DisplayUtils::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    char dataBuffer[30];
    snprintf(dataBuffer, sizeof(dataBuffer), "%d,%d,%d,%d,%d", x, y, w, h, color);
    sendCommand("FILL_RECT", dataBuffer);
}

} // namespace NuggetsInc
