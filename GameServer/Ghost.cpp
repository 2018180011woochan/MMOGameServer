#include "pch.h"
#include "Ghost.h"
#include "Room.h"
#include "Player.h"
#include "ClientPacketHandler.h"
#include "../Common/Packet/PacketProtocol.h"

Ghost::Ghost()
{
	hp = 100.f;
	maxHp = 100.f;
	moveSpeed = 2.0f;
}

Ghost::~Ghost()
{
}

void Ghost::Update(float deltaTime)
{
	if (hp <= 0.0f) return;

	RoomRef roomRef = room.lock();
	if (roomRef == nullptr) return;

	if (_teleportCooldownTimer > 0.0f)
	{
		_teleportCooldownTimer -= deltaTime;
	}

	if (_isTeleportPending)
	{
		_teleportDelayTimer -= deltaTime;
		if (_teleportDelayTimer <= 0.0f)
		{
			_isTeleportPending = false;
			_teleportCooldownTimer = 1.5f;

			PlayerRef target = roomRef->FindNearestPlayer(posX, posY, posZ, 12.0f);
			if (target != nullptr)
			{
				float angle = ((float)rand() / RAND_MAX) * 3.141592f * 2.0f;
				posX = target->posX + cos(angle) * 6.0f;
				posZ = target->posZ + sin(angle) * 6.0f;
			}
			else
			{
				float angle = ((float)rand() / RAND_MAX) * 3.141592f * 2.0f;
				posX += cos(angle) * 6.0f;
				posZ += sin(angle) * 6.0f;
			}

			_attackTimer = 0.0f;

			S_MONSTER_STATE statePkt;
			statePkt.monsterId = monsterId;
			statePkt.state = (MONSTER_STATE)4;
			statePkt.targetId = target != nullptr ? target->playerId : -1;
			statePkt.destX = posX;
			statePkt.destY = posY;
			statePkt.destZ = posZ;

			auto sendBuffer = ClientPacketHandler::MakeSendBuffer(statePkt, PKT_S_MONSTER_STATE);
			roomRef->Broadcast(sendBuffer);
		}

		return;
	}

	PlayerRef target = roomRef->FindNearestPlayer(posX, posY, posZ, 12.0f);

	if (target != nullptr)
	{
		float dirX = target->posX - posX;
		float dirZ = target->posZ - posZ;
		float dist = sqrt(dirX * dirX + dirZ * dirZ);

		if (dist <= 6.0f)
		{
			_attackTimer += deltaTime;
			if (_attackTimer >= 3.0f)
			{
				_attackTimer = 0.0f;

				S_MONSTER_STATE statePkt;
				statePkt.monsterId = monsterId;
				statePkt.state = MONSTER_STATE::STATE_ATTACK;
				statePkt.targetId = target->playerId;

				statePkt.destX = target->posX;
				statePkt.destY = target->posY;
				statePkt.destZ = target->posZ;

				auto sendBuffer = ClientPacketHandler::MakeSendBuffer(statePkt, PKT_S_MONSTER_STATE);
				roomRef->Broadcast(sendBuffer);
				return;
			}
		}
		else
		{
			if (_attackTimer > 0.0f)
			{
				_attackTimer = 0.0f;
				_wanderTimer = 0.0f;
			}

			posX += (dirX / dist) * moveSpeed * deltaTime;
			posZ += (dirZ / dist) * moveSpeed * deltaTime;
		}
	}
	else
	{
		if (_attackTimer > 0.0f)
		{
			_attackTimer = 0.0f;
			_wanderTimer = 0.0f;
		}
	}

	_wanderTimer -= deltaTime;
	if (_wanderTimer > 0.0f) return;
	_wanderTimer = 2.0f;

	S_MONSTER_STATE statePkt;
	statePkt.monsterId = monsterId;

	if (target != nullptr)
	{
		statePkt.state = MONSTER_STATE::STATE_CHASE;
		statePkt.targetId = target->playerId;
		statePkt.destX = target->posX;
		statePkt.destY = target->posY;
		statePkt.destZ = target->posZ;
	}
	else
	{
		float randomX = posX + (((float)rand() / RAND_MAX) * 10.0f - 5.0f);
		float randomZ = posZ + (((float)rand() / RAND_MAX) * 10.0f - 5.0f);

		statePkt.state = MONSTER_STATE::STATE_WANDER;
		statePkt.targetId = -1;
		statePkt.destX = randomX;
		statePkt.destY = posY;
		statePkt.destZ = randomZ;
		posX = randomX;
		posZ = randomZ;
	}

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(statePkt, PKT_S_MONSTER_STATE);
	roomRef->Broadcast(sendBuffer);
}

void Ghost::OnDamaged(float damage)
{
	if (_teleportCooldownTimer <= 0.0f && _isTeleportPending == false)
	{
		_isTeleportPending = true;
		_teleportDelayTimer = 1.0f;
	}
}