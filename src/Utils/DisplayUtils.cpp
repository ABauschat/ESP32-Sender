#include "DisplayUtils.h"
#include "Communication/Node.h"
#include "Communication/NodeService.h"
#include "Communication/HandleEvents.h"
#include "Communication/MessageTypes.h"

namespace NuggetsInc {

DisplayUtils::DisplayUtils() {}

DisplayUtils::~DisplayUtils() {}

void DisplayUtils::sendCommand(::DisplayCommandID cmdID, const char* data, bool nonBlocking, int maxRetries) {
    Node* connector = Node::getActiveInstance();
    if (!connector || !connector->isPeerIntialized()) {
        Serial.println("Cannot send command: Connector not active or peer not initialized");
        return;
    }

    NodeService service(connector);
    const uint32_t displayAckTimeoutMs = 120; 

    bool ok = false;
    if (nonBlocking) {
        ok = service.sendCommandNonBlocking(static_cast<uint8_t>(cmdID), data);
    } else {
        ok = service.sendCommandBlocking(static_cast<uint8_t>(cmdID), data, displayAckTimeoutMs, maxRetries);
    }
}

void DisplayUtils::clearDisplay(bool nonBlocking, int maxRetries) {
    sendCommand(::CMD_CLEAR_DISPLAY, nullptr, nonBlocking, maxRetries);
}

void DisplayUtils::displayMessage(const String& message, bool nonBlocking, int maxRetries) {
    sendCommand(CMD_DISPLAY_MESSAGE, message.c_str(), nonBlocking, maxRetries);
}

void DisplayUtils::newTerminalDisplay(const String& message, bool nonBlocking, int maxRetries) {
    sendCommand(CMD_NEW_TERMINAL_DISPLAY, message.c_str(), nonBlocking, maxRetries);
}

void DisplayUtils::addToTerminalDisplay(const String& message, bool nonBlocking, int maxRetries) {
    sendCommand(CMD_ADD_TO_TERMINAL, message.c_str(), nonBlocking, maxRetries);
}

void DisplayUtils::println(const String& message, bool nonBlocking, int maxRetries) {
    sendCommand(CMD_PRINTLN, message.c_str(), nonBlocking, maxRetries);
}

void DisplayUtils::print(const String& message, bool nonBlocking, int maxRetries) {
    sendCommand(CMD_PRINT, message.c_str(), nonBlocking, maxRetries);
}

void DisplayUtils::setCursor(int16_t x, int16_t y, bool nonBlocking, int maxRetries) {
    char dataBuffer[20];
    snprintf(dataBuffer, sizeof(dataBuffer), "%d,%d", x, y);
    sendCommand(CMD_SET_CURSOR, dataBuffer, nonBlocking, maxRetries);
}

void DisplayUtils::setTextSize(uint8_t size, bool nonBlocking, int maxRetries) {
    char dataBuffer[10];
    snprintf(dataBuffer, sizeof(dataBuffer), "%d", size);
    sendCommand(CMD_SET_TEXT_SIZE, dataBuffer, nonBlocking, maxRetries);
}

void DisplayUtils::setTextColor(uint16_t color, bool nonBlocking, int maxRetries) {
    char dataBuffer[10];
    snprintf(dataBuffer, sizeof(dataBuffer), "%d", color);
    sendCommand(CMD_SET_TEXT_COLOR, dataBuffer, nonBlocking, maxRetries);
}

void DisplayUtils::fillScreen(uint16_t color, bool nonBlocking, int maxRetries) {
    char dataBuffer[10];
    snprintf(dataBuffer, sizeof(dataBuffer), "%d", color);
    sendCommand(CMD_FILL_SCREEN, dataBuffer, nonBlocking, maxRetries);
}

void DisplayUtils::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, bool nonBlocking, int maxRetries) {
    char dataBuffer[30];
    snprintf(dataBuffer, sizeof(dataBuffer), "%d,%d,%d,%d,%d", x, y, w, h, color);
    sendCommand(CMD_DRAW_RECT, dataBuffer, nonBlocking, maxRetries);
}

void DisplayUtils::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, bool nonBlocking, int maxRetries) {
    char dataBuffer[30];
    snprintf(dataBuffer, sizeof(dataBuffer), "%d,%d,%d,%d,%d", x, y, w, h, color);
    sendCommand(CMD_FILL_RECT, dataBuffer, nonBlocking, maxRetries);
}

void DisplayUtils::beginPlot(const String& xTitle, const String& yTitle, int minX, int maxX, int minY, int maxY, bool nonBlocking, int maxRetries) {
    char dataBuffer[50];
    snprintf(dataBuffer, sizeof(dataBuffer), "%d,%d,%d,%d,%s,%s", minX, maxX, minY, maxY, xTitle.c_str(), yTitle.c_str());
    sendCommand(CMD_BEGIN_PLOT, dataBuffer, nonBlocking, maxRetries);
}

void DisplayUtils::plotPoint(int xValue, int yValue, uint16_t color, bool nonBlocking, int maxRetries) {
    char dataBuffer[20];
    snprintf(dataBuffer, sizeof(dataBuffer), "%d,%d,%d", xValue, yValue, color);
    sendCommand(CMD_PLOT_POINT, dataBuffer, nonBlocking, maxRetries);
}

} // namespace NuggetsInc
