// HandleEvents.h
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
    SELECT
};

struct Event {
    EventType type;
};

class ConnectWithRemote; // Forward declaration

class HandleEvents
{
public:
    static HandleEvents& getInstance();

    void processCommand(const char* command, const char* data);

    void setConnector(ConnectWithRemote* connector);

    QueueHandle_t getEventQueue();

    void clearEvents();

private:
    HandleEvents();
    ~HandleEvents();
    HandleEvents(const HandleEvents&) = delete;
    HandleEvents& operator=(const HandleEvents&) = delete;

    void executeCommand(const char* command, const char* data);

    ConnectWithRemote* connectWithRemote;
    QueueHandle_t eventQueue;
};

} // namespace NuggetsInc

#endif // HANDLE_EVENTS_H
