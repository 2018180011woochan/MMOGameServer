#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "Monster.h"
#include "GameSession.h"
#include "ClientPacketHandler.h"
#include "Room.h"
#include "../Common/Packet/PacketProtocol.h"
Room GRoom;

void Room::Enter(PlayerRef player)
{
	cout << "Room::Enter 호출됨! 플레이어 ID: " << player->playerId << endl;
	WRITE_LOCK;
	_players[player->playerId] = player;

	S_ENTER_GAME myEnterPkt;
	myEnterPkt.playerId = player->playerId;
	myEnterPkt.posX = player->posX;
	myEnterPkt.posY = player->posY;
	myEnterPkt.posZ = player->posZ;
	myEnterPkt.rotY = player->rotY;
	auto myEnterSendBuffer = ClientPacketHandler::MakeSendBuffer(myEnterPkt, PKT_S_ENTER_GAME);

	if (auto mySession = player->ownerSession.lock())
	{
		mySession->Send(myEnterSendBuffer);
	}

	for (auto& pair : _players) {
		PlayerRef otherPlayer = pair.second;

		auto otherSession = otherPlayer->ownerSession.lock();
		if (otherSession == nullptr) continue;

		if (otherPlayer->playerId != player->playerId)
		{
			// 기존 유저들에게 내 정보 전송
			otherSession->Send(myEnterSendBuffer);

			// 기존 유저들의 정보 전송 나한테
			S_ENTER_GAME otherEnterPkt;
			otherEnterPkt.playerId = otherPlayer->playerId;
			otherEnterPkt.posX = otherPlayer->posX;
			otherEnterPkt.posY = otherPlayer->posY;
			otherEnterPkt.posZ = otherPlayer->posZ;
			otherEnterPkt.rotY = otherPlayer->rotY;

			auto otherSendBuffer = ClientPacketHandler::MakeSendBuffer(otherEnterPkt, PKT_S_ENTER_GAME);

			// 내 세션이 살아있다면 나에게 전송
			if (auto mySession = player->ownerSession.lock())
			{
				mySession->Send(otherSendBuffer);
			}
		}
	}

	for (auto& pair : _monsters)
	{
		MonsterRef monster = pair.second;

		S_MONSTER_STATE sPkt;
		sPkt.monsterId = monster->monsterId;
		sPkt.state = monster->state; // 몬스터의 현재 상태!
		sPkt.targetId = monster->targetPlayerId;
		sPkt.destX = monster->destX;
		sPkt.destY = monster->destY;
		sPkt.destZ = monster->destZ;

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(sPkt, PKT_S_MONSTER_STATE);

		if (auto mySession = player->ownerSession.lock())
		{
			mySession->Send(sendBuffer);
		}
	}
}

void Room::Leave(PlayerRef player)
{
	WRITE_LOCK;
	_players.erase(player->playerId);

	S_LEAVE_GAME leavePkt;
	leavePkt.playerId = player->playerId;
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(leavePkt, PKT_S_LEAVE_GAME);

	for (auto& pair : _players)
	{
		if (auto session = pair.second->ownerSession.lock())
		{
			session->Send(sendBuffer);
		}
	}
}

void Room::Broadcast(SendBufferRef sendBuffer)
{
	READ_LOCK;

	for (auto& pair : _players)
	{
		PlayerRef p = pair.second;
		if (auto session = p->ownerSession.lock())
		{
			session->Send(sendBuffer);
		}
	}
}

void Room::EnterMonster(MonsterRef monster)
{
	WRITE_LOCK;
	_monsters[monster->monsterId] = monster;
	monster->roomId = 1;
	monster->room = shared_from_this();
}

void Room::LeaveMonster(int32 monsterId)
{
	WRITE_LOCK;
	_monsters.erase(monsterId);
}

void Room::Update(float deltaTime)
{
	WRITE_LOCK;

	if (_players.empty()) return;

	for (auto& pair : _monsters)
	{
		MonsterRef monster = pair.second;

		if (monster != nullptr)
		{
			monster->Update(deltaTime);
		}
	}
}
