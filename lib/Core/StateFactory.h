#ifndef STATEFACTORY_H
#define STATEFACTORY_H

#include "State.h"
#include "Utils/DisplayUtils.h"

namespace NuggetsInc {

enum StateType {
    MENU_STATE,
    RELAY_STATE
};

class StateFactory {
public:
    static AppState* createState(StateType type);

private:
    DisplayUtils* displayUtils;
};

} // namespace NuggetsInc

#endif // STATEFACTORY_H
