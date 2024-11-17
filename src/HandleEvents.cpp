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

    void HandleEvents::processCommand(const char* command)
    {
        executeCommand(command);
    }

    void HandleEvents::executeCommand(const char* command)
    {
        struct_message response;
        strcpy(response.messageType, "response");

        if (strcmp(command, "MOVE_UP") == 0)
        {
            Serial.println("Executing MOVE_UP");
            strcpy(response.command, "MOVE_UP_DONE");
        }
        else if (strcmp(command, "MOVE_DOWN") == 0)
        {
            Serial.println("Executing MOVE_DOWN");
            strcpy(response.command, "MOVE_DOWN_DONE");
        }
        else if (strcmp(command, "MOVE_LEFT") == 0)
        {
            Serial.println("Executing MOVE_LEFT");
            strcpy(response.command, "MOVE_LEFT_DONE");
        }
        else if (strcmp(command, "MOVE_RIGHT") == 0)
        {
            Serial.println("Executing MOVE_RIGHT");
            strcpy(response.command, "MOVE_RIGHT_DONE");
        }
        else if (strcmp(command, "SELECT") == 0)
        {
            Serial.println("Executing SELECT");
            strcpy(response.command, "SELECT_DONE");
        }
        else
        {
            Serial.println("Received unknown command");
            strcpy(response.command, "UNKNOWN_COMMAND");
        }

        if (connectWithRemote)
        {
            connectWithRemote->sendData(reinterpret_cast<const uint8_t*>(&response), sizeof(response));
        }
        else
        {
            Serial.println("Connector not set");
        }
    }
}