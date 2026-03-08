#include "pch.h"
#include "Golem.h"
#include "Room.h"
#include "Player.h"

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

void Golem::ProcessIdle(float deltaTime)
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

void Golem::ProcessWander(float deltaTime)
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

	float dx = destX - posX;
	float dz = destZ - posZ;
	float dist = std::sqrt(dx * dx + dz * dz);

	if (dist > 0.1f) 
	{
		posX += (dx / dist) * wanderSpeed * deltaTime;
		posZ += (dz / dist) * wanderSpeed * deltaTime;
	}

	if (stateTimer >= wanderInterval)
	{
		ChangeState(STATE_IDLE);
	}
}

void Golem::ProcessChase(float deltaTime)
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
		if (currentCooldown <= 0.f)
		{
			ChangeState(STATE_ATTACK);
		}
		return; 
	}

	if (dist > detectionRange * 1.25f)
	{
		targetPlayerId = -1;
		ChangeState(STATE_IDLE);
		return;
	}

	if (dist > 0.0f)
	{
		posX += (dx / dist) * chaseSpeed * deltaTime;
		posZ += (dz / dist) * chaseSpeed * deltaTime;
	}
}

void Golem::ProcessAttack(float deltaTime)
{
	if (stateTimer >= attackDelay)
	{
		currentCooldown = attackCooldown; 
		ChangeState(STATE_CHASE);
	}
}

void Golem::Update(float deltaTime)
{
	if (currentCooldown > 0.f)
		currentCooldown -= deltaTime;

	Monster::Update(deltaTime);
}
