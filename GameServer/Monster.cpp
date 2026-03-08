#include "pch.h"
#include "Monster.h"
#include "ClientPacketHandler.h"
#include "RoomManager.h"
#include "../Common/Packet/PacketProtocol.h"

Monster::Monster()
{
}

Monster::~Monster()
{
}

void Monster::Update(float deltaTime)
{
	if (hp <= 0.0f || state == STATE_DEAD) return;
	stateTimer += deltaTime;

	switch (state)
	{
	case STATE_IDLE: ProcessIdle(deltaTime); break;
	case STATE_WANDER: ProcessWander(deltaTime); break;
	case STATE_CHASE: ProcessChase(deltaTime); break;
	case STATE_ATTACK: ProcessAttack(deltaTime); break;
	case STATE_DEAD: ProcessDead(deltaTime); break;
	}
}

void Monster::ProcessIdle(float deltaTime)
{
	if (stateTimer >= 2.0f)
	{
		ChangeState(STATE_WANDER);
	}
}

void Monster::ProcessChase(float deltaTime)
{
	float distance = 5.0f; // TODO : 임시 거리 

	if (distance <= attackRange)
	{
		ChangeState(STATE_ATTACK);
	}
	else if (distance > detectionRange * 1.2f)
	{
		targetPlayerId = -1; 
		ChangeState(STATE_WANDER);
	}
}

void Monster::ProcessAttack(float deltaTime)
{
	if (stateTimer >= 1.5f)
	{
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
