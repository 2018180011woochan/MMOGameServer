#include "pch.h"
#include "IdleState.h"
#include "Monster.h"

void IdleState::Enter(Monster* monster)
{
	monster->stateTimer = 0.0f;
}

void IdleState::Execute(Monster* monster, float deltaTime)
{
}

void IdleState::Exit(Monster* monster)
{
}
