#include "pch.h"
#include "Skeleton.h"

Skeleton::Skeleton()
{
	detectionRange = 5.0f;
	attackRange = 6.0f;
	moveSpeed = 1.0f;

	wanderRadius = 5.0f;
	wanderInterval = 3.0f;
	confrontDuration = 2.0f;
	retreatDuration = 1.5f;

	state = STATE_IDLE;
}

Skeleton::~Skeleton()
{
}

void Skeleton::ProcessWander(float deltaTime)
{
	if (stateTimer >= 3.0f)
	{
		ChangeState(STATE_IDLE);
	}
}

void Skeleton::ProcessConfront(float deltaTime)
{
	float distance = 3.0f;

	if (distance > attackRange * 1.5f)
	{
		ChangeState(STATE_CHASE);
		return;
	}

	if (stateTimer >= confrontDuration)
	{
		hasConfrontedOnce = true;
		ChangeState(STATE_ATTACK);
	}
}

void Skeleton::ProcessRetreat(float deltaTime)
{
	float distance = 7.0f;

	if (stateTimer >= retreatDuration || distance > attackRange * 1.5f)
	{
		if (distance <= attackRange)
		{
			ChangeState(STATE_ATTACK);
		}
		else
		{
			ChangeState(STATE_CHASE);
		}
	}
}

void Skeleton::ProcessChase(float deltaTime)
{
	if (stateTimer < 3.0f) return;

	ChangeState(STATE_ATTACK);
}

void Skeleton::ProcessIdle(float deltaTime)
{
	if (stateTimer >= 2.0f)
	{
		float angle = (rand() % 360) * 3.14159f / 180.f;
		float r = (rand() % 100 / 100.f) * wanderRadius;

		this->destX = this->posX + r * cos(angle);
		this->destY = this->posY; 
		this->destZ = this->posZ + r * sin(angle);
		this->targetPlayerId = -1;

		ChangeState(STATE_WANDER);
	}
}
