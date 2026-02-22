#include "pch.h"
#include "ClientPacketHandler.h"
#include "Player.h"
#include "Room.h"
#include "GameSession.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

// 직접 컨텐츠 작업자

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_C_LOGIN(PacketSessionRef& session, C_LOGIN* pkt)
{
	cout << "Client Login Request Received! DummyID: " << pkt->dummyId << endl;
	
	S_LOGIN sPkt;
	sPkt.playerId = 999; // 테스트용으로 999번 플레이어 ID 발급

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(sPkt, PKT_S_LOGIN);
	session->Send(sendBuffer);
	return true;
}

bool Handle_C_MOVE(PacketSessionRef& session, C_MOVE* pkt)
{

	// TODO: 이 좌표를 S_MOVE 패킷으로 재조립해서 같은 맵에 있는 다른 세션들에게 Broadcast 해야 함.
	return true;
}
