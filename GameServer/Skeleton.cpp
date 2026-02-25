#include "pch.h"
#include "Skeleton.h"
#include "Room.h"
#include "Player.h"

Skeleton::Skeleton()
{
	detectionRange = 10.0f;
	attackRange = 1.8f; 
	moveSpeed = 1.0f;

	wanderRadius = 5.0f;
	wanderInterval = 3.0f;
	attackDelay = 1.0f; 

	state = STATE_IDLE;
	this->type = MONSTER_TYPE_SKELETON;
	this->maxHp = 30.f;
	this->hp = 30.f;
}

Skeleton::~Skeleton()
{
}

void Skeleton::ProcessIdle(float deltaTime)
{
	if (RoomRef myRoom = room.lock())
	{
		PlayerRef target = myRoom->FindNearestPlayer(posX, posY, posZ, detectionRange);
		if (target != nullptr)
		{
			this->targetPlayerId = target->playerId;
			ChangeState(STATE_CHASE);
			return;
		}
	}

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

void Skeleton::ProcessWander(float deltaTime)
{
	if (RoomRef myRoom = room.lock())
	{
		PlayerRef target = myRoom->FindNearestPlayer(posX, posY, posZ, detectionRange);
		if (target != nullptr)
		{
			this->targetPlayerId = target->playerId;
			ChangeState(STATE_CHASE);
			return;
		}
	}

	if (stateTimer >= wanderInterval)
	{
		ChangeState(STATE_IDLE);
	}
}

void Skeleton::ProcessChase(float deltaTime)
{
	RoomRef myRoom = room.lock();
	if (myRoom == nullptr) return;

	PlayerRef target = myRoom->GetPlayer(targetPlayerId);

	if (target == nullptr)
	{
		targetPlayerId = -1;
		ChangeState(STATE_IDLE);
		return;
	}

	float dx = target->posX - posX;
	float dy = target->posY - posY;
	float dz = target->posZ - posZ;
	float dist = std::sqrt(dx * dx + dy * dy + dz * dz);

	if (dist <= attackRange)
	{
		ChangeState(STATE_ATTACK);
		return;
	}
	else if (dist > detectionRange * 1.5f)
	{
		targetPlayerId = -1;
		ChangeState(STATE_IDLE);
		return;
	}

	if (dist > 0.0f)
	{
		posX += (dx / dist) * moveSpeed * deltaTime;
		posZ += (dz / dist) * moveSpeed * deltaTime;
	}
}

void Skeleton::ProcessAttack(float deltaTime)
{
	if (stateTimer >= attackDelay)
	{
		ChangeState(STATE_CHASE);
	}
}