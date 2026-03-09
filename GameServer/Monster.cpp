#include "pch.h"
#include "Monster.h"
#include "ClientPacketHandler.h"
#include "RoomManager.h"
#include "../Common/Packet/PacketProtocol.h"
#include "Room.h"
#include "Player.h"

Monster::Monster()
{
}

Monster::~Monster()
{
}

void Monster::Update(float deltaTime)
{
	if (hp <= 0.0f || state == STATE_DEAD) return;

	if (currentCooldown > 0.f)
		currentCooldown -= deltaTime;

	stateTimer += deltaTime;

	switch (state) {
	case STATE_IDLE: ProcessIdle(deltaTime); break;
	case STATE_WANDER: ProcessWander(deltaTime); break;
	case STATE_CHASE: ProcessChase(deltaTime); break;
	case STATE_ATTACK: ProcessAttack(deltaTime); break;
	case STATE_DEAD: ProcessDead(deltaTime); break;
	}
}

void Monster::OnDamaged(float damage)
{
}

void Monster::ProcessIdle(float deltaTime)
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

void Monster::ProcessWander(float deltaTime)
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

void Monster::ProcessChase(float deltaTime)
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
			this->destX = target->posX;
			this->destY = target->posY;
			this->destZ = target->posZ;

			ChangeState(STATE_ATTACK);
		}
		return;
	}

	if (dist > detectionRange * 1.5f)
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

void Monster::ProcessAttack(float deltaTime)
{
	if (stateTimer >= attackDelay)
	{
		currentCooldown = attackCooldown;
		ChangeState(STATE_CHASE);
	}
}

void Monster::ProcessDead(float deltaTime)
{
}

void Monster::ChangeState(MONSTER_STATE newState)
{
	if (state == STATE_DEAD) return;
	if (state == newState) return;

	state = newState;
	stateTimer = 0.0f;

	S_MONSTER_STATE sPkt;
	sPkt.monsterId = this->monsterId;
	sPkt.state = this->state;
	sPkt.targetId = this->targetPlayerId;
	sPkt.destX = this->destX;
	sPkt.destY = this->destY;
	sPkt.destZ = this->destZ;

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(sPkt, PKT_S_MONSTER_STATE);

	RoomRef myRoom = room.lock();
	if (myRoom != nullptr)
	{
		myRoom->Broadcast(sendBuffer);
	}
}