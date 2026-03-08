#include "pch.h"
#include "Ghost.h"
#include "Room.h"
#include "ClientPacketHandler.h"
#include "../Common/Packet/PacketProtocol.h"

Ghost::Ghost()
{
	this->type = MONSTER_TYPE_NONE;
	this->maxHp = 20.f;
	this->hp = 20.f;

	detectionRange = 6.0f;
	attackRange = 6.0f;

	chaseSpeed = 2.0f;
	wanderSpeed = 2.0f;
	moveSpeed = 2.0f;

	wanderRadius = 5.0f;
	wanderInterval = 3.0f;

	attackDelay = 3.0f;
	attackCooldown = 3.0f;
	currentCooldown = 0.f;

	state = STATE_IDLE;
}

Ghost::~Ghost()
{
}

void Ghost::OnDamaged(float damage)
{
	if (hp <= 0.0f) return;

	float angle = (rand() % 360) * (3.141592f / 180.0f);
	posX += cos(angle) * 2.0f;
	posZ += sin(angle) * 2.0f;

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