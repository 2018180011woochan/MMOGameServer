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
	_attackTimer = 0.0f;
	_wanderTimer = 0.0f;
}

Ghost::~Ghost()
{
}

void Ghost::Update(float deltaTime)
{
	if (hp <= 0.0f) return;

	RoomRef roomRef = room.lock();
	if (roomRef == nullptr) return;

	if (_attackTimer > 0.0f) _attackTimer -= deltaTime;
	if (_wanderTimer > 0.0f) _wanderTimer -= deltaTime;

	PlayerRef target = roomRef->FindNearestPlayer(posX, posY, posZ, 12.0f);

	if (target != nullptr)
	{
		if (_attackTimer <= 0.0f)
		{
			_attackTimer = 3.0f;

			S_MONSTER_STATE statePkt;
			statePkt.monsterId = monsterId;
			statePkt.state = MONSTER_STATE::STATE_ATTACK;
			statePkt.targetId = target->playerId;
			statePkt.destX = target->posX;
			statePkt.destY = target->posY;
			statePkt.destZ = target->posZ;

			auto sendBuffer = ClientPacketHandler::MakeSendBuffer(statePkt, PKT_S_MONSTER_STATE);
			roomRef->Broadcast(sendBuffer);
		}
	}
	else
	{
		_attackTimer = 0.0f;

		if (_wanderTimer <= 0.0f)
		{
			_wanderTimer = 3.0f;

			float angle = (rand() % 360) * (3.141592f / 180.0f);
			posX += cos(angle) * 5.0f;
			posZ += sin(angle) * 5.0f;

			S_MONSTER_STATE statePkt;
			statePkt.monsterId = monsterId;
			statePkt.state = MONSTER_STATE::STATE_WANDER;
			statePkt.targetId = -1;
			statePkt.destX = posX;
			statePkt.destY = posY;
			statePkt.destZ = posZ;

			auto sendBuffer = ClientPacketHandler::MakeSendBuffer(statePkt, PKT_S_MONSTER_STATE);
			roomRef->Broadcast(sendBuffer);
		}
	}
}

void Ghost::OnDamaged(float damage)
{
	float angle = (rand() % 360) * (3.141592f / 180.0f);
	posX += cos(angle) * 4.0f;
	posZ += sin(angle) * 4.0f;

	RoomRef roomRef = room.lock();
	if (roomRef != nullptr)
	{
		S_MONSTER_STATE statePkt;
		statePkt.monsterId = monsterId;
		statePkt.state = MONSTER_STATE::STATE_TELEPORT;
		statePkt.targetId = -1;
		statePkt.destX = posX;
		statePkt.destY = posY;
		statePkt.destZ = posZ;

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(statePkt, PKT_S_MONSTER_STATE);
		roomRef->Broadcast(sendBuffer);
	}
}