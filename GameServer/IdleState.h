#pragma once
#include "IState.h"

class IdleState : public IState
{
public:
    static IdleState* GetInstance() {
        static IdleState instance;
        return &instance;
    }
    virtual void Enter(Monster* monster) override;
    virtual void Execute(Monster* monster, float deltaTime) override;
    virtual void Exit(Monster* monster) override;
};


