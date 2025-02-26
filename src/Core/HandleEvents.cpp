// HandleEvents.cpp
#include "HandleEvents.h"
#include "ConnectWithRemote.h" // Ensure this path is correct
#include <string.h> // For strcmp

namespace NuggetsInc {

HandleEvents& HandleEvents::getInstance()
{
    static HandleEvents instance;
    return instance;
}

HandleEvents::HandleEvents()
    : connectWithRemote(nullptr)
{
    eventQueue = xQueueCreate(10, sizeof(Event));
    if (eventQueue == nullptr)
    {
        Serial.println("Failed to create event queue");
    }
}

HandleEvents::~HandleEvents()
{
    if (eventQueue != nullptr)
    {
        vQueueDelete(eventQueue);
    }
}

void HandleEvents::setConnector(ConnectWithRemote* connector)
{
    connectWithRemote = connector;
}

void HandleEvents::processCommand(const char* command, const char* data)
{
    executeCommand(command, data);
}

void HandleEvents::executeCommand(const char* command, const char* data)
{
    Event event;
    bool validCommand = true;

    if (strcmp(command, "MOVE_UP") == 0)
    {
        event.type = EventType::MOVE_UP;
    }
    else if (strcmp(command, "MOVE_DOWN") == 0)
    {
        event.type = EventType::MOVE_DOWN;
    }
    else if (strcmp(command, "MOVE_LEFT") == 0)
    {
        event.type = EventType::MOVE_LEFT;
    }
    else if (strcmp(command, "MOVE_RIGHT") == 0)
    {
        event.type = EventType::MOVE_RIGHT;
    }
    else if (strcmp(command, "SELECT") == 0)
    {
        event.type = EventType::SELECT;
    }
    else if (strcmp(command, "BACK") == 0)
    {
        event.type = EventType::BACK;
    }
    else
    {
        Serial.println("Unknown command received");
        validCommand = false;
    }

    if (validCommand && eventQueue != nullptr)
    {
        if (xQueueSend(eventQueue, &event, pdMS_TO_TICKS(100)) != pdPASS)
        {
            Serial.println("Failed to enqueue event");
        }
        else
        {
            Serial.print("Enqueued event: ");
            switch (event.type)
            {
                case EventType::MOVE_UP:
                    Serial.println("MOVE_UP");
                    break;
                case EventType::MOVE_DOWN:
                    Serial.println("MOVE_DOWN");
                    break;
                case EventType::MOVE_LEFT:
                    Serial.println("MOVE_LEFT");
                    break;
                case EventType::MOVE_RIGHT:
                    Serial.println("MOVE_RIGHT");
                    break;
                case EventType::SELECT:
                    Serial.println("SELECT");
                    break;
                default:
                    Serial.println("UNKNOWN_EVENT");
            }
        }
    }
}

QueueHandle_t HandleEvents::getEventQueue()
{
    return eventQueue;
}

void HandleEvents::clearEvents()
{
    Event tempEvent;
    while (xQueueReceive(eventQueue, &tempEvent, 0) == pdPASS)
    {
        // Discard the event
    }
}

} // namespace NuggetsInc
