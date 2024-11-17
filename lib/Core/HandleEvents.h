#ifndef HANDLE_EVENTS_H
#define HANDLE_EVENTS_H

#include <Arduino.h>
#include "ConnectWithRemote.h"

namespace NuggetsInc
{
    class HandleEvents
    {
    public:
        static HandleEvents& getInstance();
        void setConnector(ConnectWithRemote* connector);
        void processCommand(const char* command, const char* data = nullptr);

        struct struct_message
        {
            char messageType[10]; 
            char command[20];    
            char data[50];      
        };

    private:
        HandleEvents();
        ~HandleEvents();

        HandleEvents(const HandleEvents&) = delete;
        HandleEvents& operator=(const HandleEvents&) = delete;
        void executeCommand(const char* command, const char* data);
        ConnectWithRemote* connectWithRemote;
    };
}

#endif // HANDLE_EVENTS_H
