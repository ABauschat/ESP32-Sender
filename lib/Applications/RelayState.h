#ifndef RELAYSTATE_H
#define RELAYSTATE_H

#include "State.h"
#include "DisplayUtils.h"
#include "HandleEvents.h"
#include <Arduino.h>
#include <esp_now.h>

namespace NuggetsInc {

class RelayState : public AppState {
public:
    RelayState();
    ~RelayState();

    void onEnter() override;
    void onExit() override;
    void update() override;

    static RelayState* getActiveInstance();
    void handleSyncNodes();

private:
    DisplayUtils* displayUtils;
    static RelayState* activeInstance;
    unsigned long lastSyncTime;
};

} // namespace NuggetsInc

#endif // RELAYSTATE_H
