#pragma once
class Monster;

class IState {
public:
    virtual ~IState() {}
    virtual void Enter(Monster* monster) = 0;
    virtual void Execute(Monster* monster, float deltaTime) = 0;
    virtual void Exit(Monster* monster) = 0;
};
