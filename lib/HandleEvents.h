#ifndef HANDLE_EVENTS_H
#define HANDLE_EVENTS_H

#include <Arduino.h>
#include "ConnectWithRemote.h"

namespace NuggetsInc
{
    class HandleEvents
    {
    public:
        /**
         * @brief Gets the singleton instance of HandleEvents.
         * 
         * @return HandleEvents& Reference to the singleton instance.
         */
        static HandleEvents& getInstance();

        /**
         * @brief Sets the ConnectWithRemote instance to send responses.
         * 
         * @param connector Pointer to the ConnectWithRemote instance.
         */
        void setConnector(ConnectWithRemote* connector);

        /**
         * @brief Processes an incoming command.
         * 
         * @param command The command string to process.
         */
        void processCommand(const char* command);

        /**
         * @brief Structure to define the message format.
         */
        struct struct_message
        {
            char messageType[10];
            char command[20]; // Increased size for longer responses if needed
        };

    private:
        HandleEvents();
        ~HandleEvents();

        // Non-copyable
        HandleEvents(const HandleEvents&) = delete;
        HandleEvents& operator=(const HandleEvents&) = delete;

        /**
         * @brief Executes the given command.
         * 
         * @param command The command string to execute.
         */
        void executeCommand(const char* command);

        ConnectWithRemote* connectWithRemote; // Pointer to send responses
    };
}

#endif // HANDLE_EVENTS_H
