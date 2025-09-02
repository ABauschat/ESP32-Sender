#ifndef HANDLE_EVENTS_H
#define HANDLE_EVENTS_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

namespace NuggetsInc {

enum class EventType {
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    SELECT,
    BACK,
    BOOOP
};

struct Event {
    EventType type;
};

class Node; // Forward declaration

class HandleEvents
{
public:
    static HandleEvents& getInstance();
    QueueHandle_t getEventQueue();
    void executeCommand(uint8_t commandID, const char* data);

private:
    HandleEvents();
    ~HandleEvents();

    QueueHandle_t eventQueue;
    // Simple per-button debounce tracking
    static constexpr uint32_t DEBOUNCE_MS = 120; // ignore repeats within this window
    uint32_t lastEventMs[6] = {0,0,0,0,0,0};
};

} // namespace NuggetsInc

#endif // HANDLE_EVENTS_H
