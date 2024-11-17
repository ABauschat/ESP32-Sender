#include "HandleEvents.h"

namespace NuggetsInc
{
    HandleEvents& HandleEvents::getInstance()
    {
        static HandleEvents instance;
        return instance;
    }

    HandleEvents::HandleEvents()
        : connectWithRemote(nullptr)
    {
    }

    HandleEvents::~HandleEvents()
    {
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
        struct_message response;
        strcpy(response.messageType, "response");

        if (strcmp(command, "MOVE_UP") == 0)
        {

        }
        else if (strcmp(command, "MOVE_DOWN") == 0)
        {

        }
        else if (strcmp(command, "MOVE_LEFT") == 0)
        {

        }
        else if (strcmp(command, "MOVE_RIGHT") == 0)
        {
            
        }
        else if (strcmp(command, "SELECT") == 0)
        {

        } 
        else
        {
            strcpy(response.command, "UNKNOWN_COMMAND");
        }

        if (data != nullptr)
        {
            strncpy(response.data, data, sizeof(response.data) - 1);
            response.data[sizeof(response.data) - 1] = '\0'; 
        }
        else
        {
            response.data[0] = '\0'; 
        }

        if (connectWithRemote)
        {
            connectWithRemote->sendData(reinterpret_cast<const uint8_t*>(&response), sizeof(response));
            Serial.print("Sent response: ");
            Serial.println(response.command);
        }
        else
        {
            Serial.println("Connector not set");
        }
    }
}
