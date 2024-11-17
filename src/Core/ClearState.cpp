// ClearState.cpp
#include "ClearState.h"
#include "Device.h"
#include "Application.h"
#include "StateFactory.h"
#include "Colors.h"

namespace NuggetsInc {

ClearState::ClearState(StateType nextState) : nextState(nextState) {}

void ClearState::onEnter() {
    displayUtils = new DisplayUtils();
    displayUtils->fillScreen(COLOR_BLACK);
    delay(100);  
}

void ClearState::update() {
    Application::getInstance().changeState(StateFactory::createActualState(nextState));
}

void ClearState::onExit() {
}

} // namespace NuggetsInc
