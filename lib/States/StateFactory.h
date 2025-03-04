#ifndef STATEFACTORY_H
#define STATEFACTORY_H

#include "State.h"

namespace NuggetsInc {

enum StateType {
    MENU_STATE
};

class StateFactory {
public:
    static AppState* createState(StateType type);
    static AppState* createActualState(StateType type);
};

} // namespace NuggetsInc

#endif // STATEFACTORY_H
