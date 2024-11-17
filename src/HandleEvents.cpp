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
    }
}
