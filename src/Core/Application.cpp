#include "Application.h"
#include "StateFactory.h"
namespace NuggetsInc {

Application& Application::getInstance() {
    static Application instance;
    return instance;
}

Application::Application() : currentState(nullptr) {}

void Application::init() {
    // Initialize 
}

void Application::run() {
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
