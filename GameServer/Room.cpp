#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "GameSession.h"
#include "ClientPacketHandler.h"
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
}

void Room::Leave(PlayerRef player)
{
	WRITE_LOCK;
	_players.erase(player->playerId);
}

void Room::Broadcast(SendBufferRef sendBuffer)
{
	//WRITE_LOCK;
	//for (auto& p : _players)
	//{
	//	p.second->ownerSession->Send(sendBuffer);
	//}

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