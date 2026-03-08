#include "pch.h"
#include "Skeleton.h"

Skeleton::Skeleton()
{
	this->type = MONSTER_TYPE_SKELETON;
	this->maxHp = 30.f;
	this->hp = 30.f;

	detectionRange = 10.0f;
	attackRange = 1.8f;

	chaseSpeed = 1.0f;
	wanderSpeed = 1.0f;
	moveSpeed = 1.0f;

	wanderRadius = 5.0f;
	wanderInterval = 3.0f;

	attackDelay = 1.0f;
	attackCooldown = 0.0f;
	currentCooldown = 0.f;

	state = STATE_IDLE;
}

Skeleton::~Skeleton()
{
}