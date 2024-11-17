#include "Application.h"
#include "StateFactory.h"
#include "Device.h"

namespace NuggetsInc {

Application& Application::getInstance() {
    static Application instance;
    return instance;
}

Application::Application() : currentState(nullptr) {}

void Application::init() {
    Device::getInstance().init();
}

void Application::run() {
    Device::getInstance().update(); 
    if (currentState) {
        currentState->update();
    }
}

void Application::changeState(AppState* newState) {
    if (currentState) {
        currentState->onExit();
        delete currentState;
    }
    currentState = newState;
    if (currentState) {
        currentState->onEnter();
    }
}

} // namespace NuggetsInc
