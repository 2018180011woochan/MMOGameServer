#include "pch.h"
#include "Golem.h"

Golem::Golem()
{
	this->type = MONSTER_TYPE_GOLEM;
	this->maxHp = 100.f;
	this->hp = 100.f;

	detectionRange = 5.0f;
	attackRange = 3.5f;

	chaseSpeed = 3.5f;
	wanderSpeed = 2.0f;

	wanderRadius = 5.0f;
	wanderInterval = 3.0f;

	attackDelay = 2.0f;
	attackCooldown = 1.0f;
	currentCooldown = 0.f;

	state = STATE_IDLE;
}

Golem::~Golem()
{
}