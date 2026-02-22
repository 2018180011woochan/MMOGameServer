#include "pch.h"
#include "ClientPacketHandler.h"
#include "Player.h"
#include "GameSession.h"
#include "RoomManager.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];
static std::atomic<int32> GPlayerIdGenerator = 1;
// 직접 컨텐츠 작업자

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_C_LOGIN(PacketSessionRef& session, C_LOGIN* pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

	cout << "Client Login Request Received! DummyID: " << pkt->dummyId << endl;
	int32 newPlayerId = GPlayerIdGenerator.fetch_add(1);

	S_LOGIN sPkt;
	sPkt.playerId = newPlayerId;
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(sPkt, PKT_S_LOGIN);
	session->Send(sendBuffer);

	float randomOffsetX = ((float)rand() / RAND_MAX) * 4.0f - 2.0f;
	float randomOffsetZ = ((float)rand() / RAND_MAX) * 4.0f - 2.0f;

	PlayerRef player = MakeShared<Player>();
	player->playerId = newPlayerId;
	player->curRoomID = ROOM::ROOM_1; // TODO: 임시로 1번 방 배정, 방id나 위치는 나중에 db에서 가져올것
	player->posX = 59.81f + randomOffsetX;
	player->posY = -9.0f;
	player->posZ = -25.58f + randomOffsetZ;
	player->rotY = 0.0f;

	// 세션, 플레이어 1:1 연결
	gameSession->SetPlayer(player);
	player->ownerSession = gameSession;

	RoomRef room = RoomManager::Instance().GetRoom(player->curRoomID);
	if (room != nullptr)
	{
		cout << player->curRoomID << "번 방을 찾았습니다! 입장(Enter) 진행!" << endl;
		room->Enter(player);
	}
	else
	{
		cout << "치명적 에러: " << player->curRoomID << "번 방이 존재하지 않습니다! (nullptr)" << endl;
	}

	return true;
}

bool Handle_C_MOVE(PacketSessionRef& session, C_MOVE* pkt)
{
	cout << pkt->posX << ", " << pkt->posY << ", " << pkt->posZ << endl;
	// TODO: 이 좌표를 S_MOVE 패킷으로 재조립해서 같은 맵에 있는 다른 세션들에게 Broadcast 해야 함.
	return true;
}
