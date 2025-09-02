#include "HandleEvents.h"
#include "Node.h"
#include <string.h>
#include "MessageTypes.h"  // for command ID definitions

namespace NuggetsInc {

HandleEvents& HandleEvents::getInstance()
{
    static HandleEvents instance;
    return instance;
}

HandleEvents::HandleEvents()
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

void HandleEvents::executeCommand(uint8_t commandID, const char* data)
{
    Event event;
    bool validCommand = true;

    switch (commandID)
    {
        case CMD_MOVE_UP:
            event.type = EventType::MOVE_UP;
            break;
        case CMD_MOVE_DOWN:
            event.type = EventType::MOVE_DOWN;
            break;
        case CMD_MOVE_LEFT:
            event.type = EventType::MOVE_LEFT;
            break;
        case CMD_MOVE_RIGHT:
            event.type = EventType::MOVE_RIGHT;
            break;
        case CMD_SELECT:
            event.type = EventType::SELECT;
            break;
        case CMD_BACK:
            event.type = EventType::BACK;
            break;
        case CMD_BOOOP:
            event.type = EventType::BOOOP;
            break;
        default:
            Serial.print("Unknown command ID received: ");
            Serial.println(commandID, HEX);
            validCommand = false;
            break;
    }

    if (validCommand && eventQueue != nullptr)
    {
        if (xQueueSend(eventQueue, &event, pdMS_TO_TICKS(50)) != pdPASS)
        {
            Serial.println("Failed to enqueue event");
        }
    }
}

QueueHandle_t HandleEvents::getEventQueue()
{
    return eventQueue;
}

} // namespace NuggetsInc
