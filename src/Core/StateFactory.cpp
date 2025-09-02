#include "StateFactory.h"
#include "Applications/MenuState.h"
#include "Applications/RelayState.h"
#include "Utils/Device.h"

namespace NuggetsInc {

AppState* StateFactory::createState(StateType type) {

    // Create a DisplayUtils instance
    DisplayUtils* displayUtils = new DisplayUtils();

    // Clear The Screen
    displayUtils->fillScreen(COLOR_BLACK);

    //Wait for a short duration to ensure the display is ready
    delay(100);  

    //Remove the displayUtils instance
    delete displayUtils; 

    // Create the appropriate state based on the type
    switch (type) {
        case MENU_STATE:
            return new MenuState();
        case RELAY_STATE:
            return new RelayState();
        default:
            return nullptr;
    }
}

} // namespace NuggetsInc
