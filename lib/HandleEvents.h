#ifndef HANDLE_EVENTS_H
#define HANDLE_EVENTS_H

#include <Arduino.h>
#include "ConnectWithRemote.h"

namespace NuggetsInc
{
    class HandleEvents
    {
    public:
        // Singleton instance access
        static HandleEvents& getInstance();
        
        // Set the connector instance
        void setConnector(ConnectWithRemote* connector);
        
        // Process incoming commands with optional data
        void processCommand(const char* command, const char* data = nullptr);

        // Structure for messages
        struct struct_message
        {
            char messageType[10]; // "command" or "response"
            char command[20];     // Command identifier
            char data[50];        // Additional data (optional)
        };

    private:
        // Private constructor and destructor for singleton pattern
        HandleEvents();
        ~HandleEvents();

        // Delete copy constructor and assignment operator
        HandleEvents(const HandleEvents&) = delete;
        HandleEvents& operator=(const HandleEvents&) = delete;

        // Execute the received command with optional data
        void executeCommand(const char* command, const char* data);

        // Pointer to the connector for sending responses
        ConnectWithRemote* connectWithRemote;
    };
}

#endif // HANDLE_EVENTS_H
