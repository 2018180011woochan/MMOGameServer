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
	//int32 nearestPlayerId = 1; 
	//float distance = 4.0f;     

	//if (distance <= detectionRange)
	//{
	//	targetPlayerId = nearestPlayerId;
	//	ChangeState(STATE_CHASE);
	//}

	if (stateTimer < 3.0f) return;

	ChangeState(STATE_CHASE);
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
	//float distance = 3.0f;

	//if (distance > detectionRange * 1.2f)
	//{
	//	targetPlayerId = -1;
	//	hasConfrontedOnce = false; 
	//	ChangeState(STATE_WANDER);
	//	return;
	//}

	//if (distance <= attackRange)
	//{
	//	if (!hasConfrontedOnce)
	//	{
	//		ChangeState((MONSTER_STATE)5);
	//	}
	//	else
	//	{
	//		ChangeState(STATE_ATTACK);
	//	}
	//}

	if (stateTimer < 3.0f) return;

	ChangeState(STATE_ATTACK);
}
