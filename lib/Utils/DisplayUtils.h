#ifndef DISPLAYUTILS_H
#define DISPLAYUTILS_H

#include <Arduino.h>
#include <vector>
#include "HandleEvents.h"
#include "Communication/MessageTypes.h"

namespace NuggetsInc
{

    class DisplayUtils
    {
    public:
        DisplayUtils();
        ~DisplayUtils();

        // Each call supports:
        // - nonBlocking: false for blocking (default), true for non-blocking
        // - maxRetries: -1 for infinite retries (default), or a finite retry count
        void clearDisplay(bool nonBlocking = false, int maxRetries = -1);
        void displayMessage(const String &message, bool nonBlocking = false, int maxRetries = -1);
        void newTerminalDisplay(const String &message, bool nonBlocking = false, int maxRetries = -1);
        void addToTerminalDisplay(const String &message, bool nonBlocking = false, int maxRetries = -1);

        void println(const String &message, bool nonBlocking = false, int maxRetries = -1);
        void print(const String &message, bool nonBlocking = false, int maxRetries = -1);
        void setCursor(int16_t x, int16_t y, bool nonBlocking = false, int maxRetries = -1);
        void setTextSize(uint8_t size, bool nonBlocking = false, int maxRetries = -1);
        void setTextColor(uint16_t color, bool nonBlocking = false, int maxRetries = -1);
        void fillScreen(uint16_t color, bool nonBlocking = false, int maxRetries = -1);
        void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, bool nonBlocking = false, int maxRetries = -1);
        void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, bool nonBlocking = false, int maxRetries = -1);

        void beginPlot(const String &xTitle, const String &yTitle, int minX, int maxX, int minY, int maxY, bool nonBlocking = false, int maxRetries = -1);
        void plotPoint(int xValue, int yValue, uint16_t color, bool nonBlocking = false, int maxRetries = -1);

    private:
        // Now using binary command identifiers.
        void sendCommand(::DisplayCommandID cmdID, const char *data = nullptr, bool nonBlocking = false, int maxRetries = -1);

    };
}

#endif // DISPLAYUTILS_H
