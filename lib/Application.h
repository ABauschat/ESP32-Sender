#ifndef APPLICATION_H
#define APPLICATION_H

#include "State.h"

namespace NuggetsInc {

class Application {
public:
    static Application& getInstance();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void init();
    void run();
    void changeState(AppState* newState);

private:
    Application(); 
    AppState* currentState;
};

} 

#endif // APPLICATION_H
