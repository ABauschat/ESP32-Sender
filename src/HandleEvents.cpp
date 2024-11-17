//Device Sender 
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
        if (strcmp(command, "MOVE_UP") == 0)
        {
            Serial.println("Executing MOVE_UP");

            struct_message response;
            strcpy(response.messageType, "response");
            strcpy(response.command, "MOVE_UP_DONE");
            if (connectWithRemote)
            {
                connectWithRemote->sendData((uint8_t*)&response, sizeof(response));
            }
        }
        else if (strcmp(command, "MOVE_DOWN") == 0)
        {
            Serial.println("Executing MOVE_DOWN");

            struct_message response;
            strcpy(response.messageType, "response");
            strcpy(response.command, "MOVE_DOWN_DONE");
            if (connectWithRemote)
            {
                connectWithRemote->sendData((uint8_t*)&response, sizeof(response));
            }
        }
        else if (strcmp(command, "MOVE_LEFT") == 0)
        {
            Serial.println("Executing MOVE_LEFT");

            struct_message response;
            strcpy(response.messageType, "response");
            strcpy(response.command, "MOVE_LEFT_DONE");
            if (connectWithRemote)
            {
                connectWithRemote->sendData((uint8_t*)&response, sizeof(response));
            }
        }
        else if (strcmp(command, "MOVE_RIGHT") == 0)
        {
            Serial.println("Executing MOVE_RIGHT");

            struct_message response;
            strcpy(response.messageType, "response");
            strcpy(response.command, "MOVE_RIGHT_DONE");
            if (connectWithRemote)
            {
                connectWithRemote->sendData((uint8_t*)&response, sizeof(response));
            }
        }
        else if (strcmp(command, "SELECT") == 0)
        {
            Serial.println("Executing SELECT");

            struct_message response;
            strcpy(response.messageType, "response");
            strcpy(response.command, "SELECT_DONE");
            if (connectWithRemote)
            {
                connectWithRemote->sendData((uint8_t*)&response, sizeof(response));
            }
        }
        else
        {
            Serial.println("Received unknown command");
            struct_message response;
            strcpy(response.messageType, "response");
            strcpy(response.command, "UNKNOWN_COMMAND");
            if (connectWithRemote)
            {
                connectWithRemote->sendData((uint8_t*)&response, sizeof(response));
            }
        }
    }
}
