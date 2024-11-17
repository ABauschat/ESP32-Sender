#include "StateFactory.h"
#include "MenuState.h"
#include "ClearState.h"

namespace NuggetsInc {

AppState* StateFactory::createState(StateType type) {
    return new ClearState(type);
}

AppState* StateFactory::createActualState(StateType type) {
    switch (type) {
        case MENU_STATE:
            return new MenuState();
            return nullptr;
        default:
            return nullptr;
    }
}

} // namespace NuggetsInc
