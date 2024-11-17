#include "HandleEvents.h"

namespace NuggetsInc
{
    // Access the singleton instance
    HandleEvents& HandleEvents::getInstance()
    {
        static HandleEvents instance;
        return instance;
    }

    // Constructor initializes the connector pointer
    HandleEvents::HandleEvents()
        : connectWithRemote(nullptr)
    {
    }

    // Destructor
    HandleEvents::~HandleEvents()
    {
    }

    // Set the connector instance
    void HandleEvents::setConnector(ConnectWithRemote* connector)
    {
        connectWithRemote = connector;
    }

    // Process incoming commands with optional data
    void HandleEvents::processCommand(const char* command, const char* data)
    {
        executeCommand(command, data);
    }

    // Execute the received command and send a response
    void HandleEvents::executeCommand(const char* command, const char* data)
    {
        struct_message response;
        strcpy(response.messageType, "response");

        // Handle different commands and prepare responses
        if (strcmp(command, "MOVE_UP") == 0)
        {
            strcpy(response.command, "MOVE_UP_DONE");
        }
        else if (strcmp(command, "MOVE_DOWN") == 0)
        {
            strcpy(response.command, "MOVE_DOWN_DONE");
        }
        else if (strcmp(command, "MOVE_LEFT") == 0)
        {
            strcpy(response.command, "MOVE_LEFT_DONE");
        }
        else if (strcmp(command, "MOVE_RIGHT") == 0)
        {
            strcpy(response.command, "MOVE_RIGHT_DONE");
        }
        else if (strcmp(command, "SELECT") == 0)
        {
            strcpy(response.command, "SELECT_DONE");
        }
        else if (strcmp(command, "CLEAR_DISPLAY") == 0)
        {
            strcpy(response.command, "CLEAR_DISPLAY_DONE");
        }
        else if (strcmp(command, "DISPLAY_MESSAGE") == 0)
        {
            strcpy(response.command, "DISPLAY_MESSAGE_DONE");
        }
        else if (strcmp(command, "NEW_TERMINAL_DISPLAY") == 0)
        {
            strcpy(response.command, "NEW_TERMINAL_DISPLAY_DONE");
        }
        else if (strcmp(command, "ADD_TO_TERMINAL_DISPLAY") == 0)
        {
            strcpy(response.command, "ADD_TO_TERMINAL_DISPLAY_DONE");
        }
        else if (strcmp(command, "PRINTLN") == 0)
        {
            strcpy(response.command, "PRINTLN_DONE");
        }
        else if (strcmp(command, "PRINT") == 0)
        {
            strcpy(response.command, "PRINT_DONE");
        }
        else if (strcmp(command, "SET_CURSOR") == 0)
        {
            strcpy(response.command, "SET_CURSOR_DONE");
        }
        else if (strcmp(command, "SET_TEXT_SIZE") == 0)
        {
            strcpy(response.command, "SET_TEXT_SIZE_DONE");
        }
        else if (strcmp(command, "SET_TEXT_COLOR") == 0)
        {
            strcpy(response.command, "SET_TEXT_COLOR_DONE");
        }
        else if (strcmp(command, "FILL_SCREEN") == 0)
        {
            strcpy(response.command, "FILL_SCREEN_DONE");
        }
        else if (strcmp(command, "DRAW_RECT") == 0)
        {
            strcpy(response.command, "DRAW_RECT_DONE");
        }
        else if (strcmp(command, "FILL_RECT") == 0)
        {
            strcpy(response.command, "FILL_RECT_DONE");
        }
        else
        {
            strcpy(response.command, "UNKNOWN_COMMAND");
        }

        // If there's data, include it in the response (optional)
        if (data != nullptr)
        {
            strncpy(response.data, data, sizeof(response.data) - 1);
            response.data[sizeof(response.data) - 1] = '\0'; // Ensure null-termination
        }
        else
        {
            response.data[0] = '\0'; // No data
        }

        // Send the response back if the connector is set
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
