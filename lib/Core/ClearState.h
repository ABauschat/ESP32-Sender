#ifndef CLEAR_STATE_H
#define CLEAR_STATE_H

#include "State.h"
#include "StateFactory.h"
#include "DisplayUtils.h"

namespace NuggetsInc {

    class ClearState : public AppState {
    public:
        explicit ClearState(StateType nextState);

        void onEnter() override;
        void update() override;
        void onExit() override;

    private:
        StateType nextState; 
        DisplayUtils* displayUtils;
    };
}

#endif // CLEAR_STATE_H
