#include "RelayState.h"
#include "Communication/HandleEvents.h"
#include "Application.h"
#include "StateFactory.h"
#include "Communication/Node.h"
#include "Communication/NodeService.h"
#include "Communication/MessageTypes.h"
#include "Utils/Device.h"
#include "Communication/MacAddressStorage.h"
#include "Utils/DisplayUtils.h"
#include "Utils/TimeUtils.h"
#include <WiFi.h>
#include <esp_now.h>

namespace NuggetsInc
{

    RelayState *RelayState::activeInstance = nullptr;

    RelayState::RelayState()
        : displayUtils(new DisplayUtils())
    {
    }

    RelayState::~RelayState()
    {
        delete displayUtils;
    }

    void RelayState::onEnter()
    {
        activeInstance = this;
    }

    void RelayState::onExit()
    {
        activeInstance = nullptr;
    }

    void RelayState::update()
    {
        Event event;
        QueueHandle_t eventQueue = HandleEvents::getInstance().getEventQueue();

        if (xQueueReceive(HandleEvents::getInstance().getEventQueue(), &event, 0) == pdPASS)
        {
            switch (event.type)
            {
            case EventType::BOOOP:
                Node *node = Node::getActiveInstance();
                if (!node)
                    return;

                NodeService service(node);
                // Send own mac address
                String ownMac = WiFi.macAddress();
                service.sendSync(ownMac.c_str());

                displayUtils->displayMessage("Sync Completed");
                break;
            }
        }
    }

    void RelayState::handleSyncNodes(const char *macData)
    {
        MacAddressStorage &macStorage = MacAddressStorage::getInstance();
        macStorage.saveMacAddressList(macData);

        Serial.println("Mac Addresses Saved");
        Serial.flush();
    }

    RelayState *RelayState::getActiveInstance()
    {
        return activeInstance;
    }

} // namespace NuggetsInc