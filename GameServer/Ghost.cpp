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

	_wanderTimer -= deltaTime;
	if (_wanderTimer > 0.0f) return;

	_wanderTimer = 2.0f; 

	PlayerRef target = roomRef->FindNearestPlayer(posX, posY, posZ, 12.0f);

	S_MONSTER_STATE statePkt;
	statePkt.monsterId = monsterId;

	if (target != nullptr)
	{
		statePkt.state = MONSTER_STATE::STATE_CHASE;
		statePkt.targetId = target->playerId; 
		statePkt.destX = target->posX;
		statePkt.destY = target->posY;
		statePkt.destZ = target->posZ;

		posX = target->posX;
		posZ = target->posZ;
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
}