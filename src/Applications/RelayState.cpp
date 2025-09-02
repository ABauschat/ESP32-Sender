#include "RelayState.h"
#include "Communication/HandleEvents.h"
#include "Application.h"
#include "StateFactory.h"
#include "Communication/Node.h"
#include "Communication/NodeService.h"
#include "Communication/MessageTypes.h"
#include "Utils/Device.h"
#include "Communication/MacAddressStorage.h"
#include <WiFi.h>
#include <esp_now.h>

namespace NuggetsInc {

RelayState* RelayState::activeInstance = nullptr;

RelayState::RelayState()
    : displayUtils(new DisplayUtils()), lastSyncTime(0)
{
}

RelayState::~RelayState() {
    delete displayUtils;
}

void RelayState::onEnter() {
    activeInstance = this;
}

void RelayState::onExit() {
    activeInstance = nullptr;
}

void RelayState::update() {
}

void RelayState::handleSyncNodes() {
    Node* node = Node::getActiveInstance();
    if (!node) return;

    NodeService service(node);
    // Send own mac address
    String ownMac = WiFi.macAddress();
    service.sendSync(ownMac.c_str());

    // NodeService.sendSync() already handles the sending
    Serial.println("Sync nodes command processed");
    delay(100);
}

RelayState* RelayState::getActiveInstance() {
    return activeInstance;
}

} // namespace NuggetsInc
