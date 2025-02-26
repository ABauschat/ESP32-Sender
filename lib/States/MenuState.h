#ifndef MENUSTATE_H
#define MENUSTATE_H

#include "State.h"
#include "DisplayUtils.h"
#include "HandleEvents.h"
#include <Arduino.h>
#include "HX711.h"   // Include the HX711 library

namespace NuggetsInc {

class MenuState : public AppState {
public:
    MenuState();
    ~MenuState();

    void onEnter() override;
    void onExit() override;
    void update() override;

private:
    void executeSelection();
    void redrawMenuChange();
    void redrawLine(int index, bool isSelected);

    // After a selection completes, this resets the scale and redraws the menu.
    void onExitSelection();

    // New functions for Measure Water and Mix Water modes
    void measureWaterMode();
    void mixWaterMode();

    static const int menuItems = 5;  // Five menu items: Tare, Calibrate, Read, Measure Water, Mix Water
    String menu[menuItems];
    int menuIndex;
    int previousMenuIndex;

    DisplayUtils* displayUtils;

    // Persist the calibration factor (0.0 means not calibrated yet)
    float calibrationFactor;
};

} // namespace NuggetsInc

#endif // MENUSTATE_H
