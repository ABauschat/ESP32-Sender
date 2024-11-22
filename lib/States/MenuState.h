// MenuState.h
#ifndef MENUSTATE_H
#define MENUSTATE_H

#include "State.h"
#include "DisplayUtils.h"
#include "HandleEvents.h"
#include <Arduino.h>

namespace NuggetsInc {

class MenuState : public AppState {
public:
    MenuState();
    ~MenuState();

    void onEnter() override;
    void onExit() override;
    void update() override;

private:
    // Optionally keep this if you plan to use full screen redraws in certain scenarios
    // void displayMenu();

    void executeSelection();
    void redrawMenuChange();
    void redrawLine(int index, bool isSelected); // New function to redraw a specific line

    static const int menuItems = 4;
    String menu[menuItems];
    int menuIndex;
    int previousMenuIndex; // New member to track the previously selected menu index

    DisplayUtils* displayUtils;
};

} // namespace NuggetsInc

#endif // MENUSTATE_H
