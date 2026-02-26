#include "pch.h"
#include "ClientPacketHandler.h"
#include "Player.h"
#include "Monster.h"
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

	player->inventory[0] = 1;   
	player->inventory[1] = 100; 
	player->inventory[2] = 200;

	for (int i = 0; i < 16; i++)
	{
		S_UPDATE_INVEN invenPkt;
		invenPkt.slotIndex = i;
		invenPkt.itemId = player->inventory[i]; 

		auto invenBuffer = ClientPacketHandler::MakeSendBuffer(invenPkt, PKT_S_UPDATE_INVEN);
		session->Send(invenBuffer); 
	}

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
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	PlayerRef player = gameSession->GetPlayer();
	if (player == nullptr) return false;

	player->posX = pkt->posX;
	player->posY = pkt->posY;
	player->posZ = pkt->posZ;
	player->rotY = pkt->rotY;

	S_MOVE sPkt;
	sPkt.playerId = player->playerId;
	sPkt.posX = player->posX;
	sPkt.posY = player->posY;
	sPkt.posZ = player->posZ;
	sPkt.rotY = player->rotY;
	sPkt.isRunning = pkt->isRunning;

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(sPkt, PKT_S_MOVE);

	RoomRef room = RoomManager::Instance().GetRoom(player->curRoomID);
	if (room != nullptr)
	{
		room->Broadcast(sendBuffer);
	}

	return true;
}

bool Handle_C_STANCE(PacketSessionRef& session, C_STANCE* pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	PlayerRef player = gameSession->GetPlayer();
	if (player == nullptr) return false;

	S_STANCE sPkt;
	sPkt.playerId = player->playerId;
	sPkt.isStance = pkt->isStance;

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(sPkt, PKT_S_STANCE);

	RoomRef room = RoomManager::Instance().GetRoom(player->curRoomID);
	if (room != nullptr)
	{
		room->Broadcast(sendBuffer);
	}

	return true;
}

bool Handle_C_JUMP(PacketSessionRef& session, C_JUMP* pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	PlayerRef player = gameSession->GetPlayer();
	if (player == nullptr) return false;

	S_JUMP sPkt;
	sPkt.playerId = player->playerId;

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(sPkt, PKT_S_JUMP);

	RoomRef room = RoomManager::Instance().GetRoom(player->curRoomID);
	if (room != nullptr)
	{
		room->Broadcast(sendBuffer);
	}

	return true;
}

bool Handle_C_ATTACK(PacketSessionRef& session, C_ATTACK* pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	PlayerRef player = gameSession->GetPlayer();
	if (player == nullptr) return false;

	S_ATTACK sPkt;
	sPkt.playerId = player->playerId;
	sPkt.attackType = pkt->attackType; 

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(sPkt, PKT_S_ATTACK);

	RoomRef room = RoomManager::Instance().GetRoom(player->curRoomID);
	if (room != nullptr)
	{
		room->Broadcast(sendBuffer);
	}

	return true;
}

bool Handle_C_DASH(PacketSessionRef& session, C_DASH* pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	PlayerRef player = gameSession->GetPlayer();

	if (player == nullptr) return false;

	RoomRef room = RoomManager::Instance().GetRoom(player->curRoomID);
	if (room == nullptr) return false;

	S_DASH sPkt;
	sPkt.playerId = player->playerId;

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(sPkt, PKT_S_DASH);

	room->Broadcast(sendBuffer);

	return true;
}

bool Handle_C_HIT_MONSTER(PacketSessionRef& session, C_HIT_MONSTER* pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	PlayerRef player = gameSession->GetPlayer();
	if (player == nullptr) return false;
	RoomRef room = RoomManager::Instance().GetRoom(player->curRoomID);
	if (room == nullptr) return false;

	MonsterRef monster = room->GetMonster(pkt->monsterId);
	if (monster == nullptr || monster->state == STATE_DEAD) return false;

	monster->hp -= pkt->damage;
	if (monster->hp < 0.f) monster->hp = 0.f;

	S_HIT_MONSTER sPkt;
	sPkt.monsterId = monster->monsterId;
	sPkt.damage = pkt->damage;
	sPkt.currentHp = monster->hp;
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(sPkt, PKT_S_HIT_MONSTER);
	room->Broadcast(sendBuffer);

	if (monster->hp <= 0.f)
	{
		monster->ChangeState(STATE_DEAD);
	}

	return true;
}

bool Handle_C_HIT_PLAYER(PacketSessionRef& session, C_HIT_PLAYER* pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	PlayerRef player = gameSession->GetPlayer();
	if (player == nullptr) return false;

	RoomRef room = RoomManager::Instance().GetRoom(player->curRoomID);
	if (room == nullptr) return false;

	if (pkt->isBlocked == 0)
	{
		player->hp -= pkt->damage;
		if (player->hp < 0.f) player->hp = 0.f;
	}

	cout << "[서버 로그] 플레이어 " << player->playerId
		<< " 피격! 남은 HP: " << player->hp
		<< " (방어여부: " << pkt->isBlocked << ")" << endl;

	S_HIT_PLAYER sPkt;
	sPkt.playerId = player->playerId;
	sPkt.monsterId = pkt->monsterId;
	sPkt.damage = pkt->damage;
	sPkt.currentHp = player->hp;
	sPkt.isBlocked = pkt->isBlocked;

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(sPkt, PKT_S_HIT_PLAYER);
	room->Broadcast(sendBuffer);

	return true;
}

bool Handle_C_USE_ITEM(PacketSessionRef& session, C_USE_ITEM* pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	PlayerRef player = gameSession->GetPlayer();
	if (player == nullptr) return false;

	RoomRef room = RoomManager::Instance().GetRoom(player->curRoomID);
	if (room == nullptr) return false;

	int32 slot = pkt->slotIndex;

	if (slot < 0 || slot >= 16) return false;

	int32 itemId = player->inventory[slot];
	if (itemId == 0) return false; 

	if (itemId == 1)
	{
		player->hp += 50.f;
		if (player->hp > player->maxHp) player->hp = player->maxHp;

		player->inventory[slot] = 0;

		cout << "[서버 로그] 플레이어 " << player->playerId << "가 포션을 마셨습니다! HP: " << player->hp << endl;

		// TODO: 클라이언트에게 변경된 HP 패킷 보내기 (S_HIT_PLAYER 재활용 하거나 새로 만들기)
	}
	// 검(100) 장착 로직
	else if (itemId == 100)
	{
		// 나중에는 기존에 장착 중인 무기가 있다면 인벤토리 빈칸으로 돌려보내는 로직 추가해야 함

		player->equipWeapon = 100; 
		player->inventory[slot] = 0; 

		cout << "[서버 로그] 플레이어 " << player->playerId << "가 100번 철검을 장착했습니다!" << endl;

		S_EQUIP_ITEM equipPkt;
		equipPkt.playerId = player->playerId;
		equipPkt.equipSlot = 0; 
		equipPkt.itemId = 100;

		auto equipBuffer = ClientPacketHandler::MakeSendBuffer(equipPkt, PKT_S_EQUIP_ITEM);
		room->Broadcast(equipBuffer);
	}

	S_UPDATE_INVEN invenPkt;
	invenPkt.slotIndex = slot;
	invenPkt.itemId = player->inventory[slot]; 

	auto invenBuffer = ClientPacketHandler::MakeSendBuffer(invenPkt, PKT_S_UPDATE_INVEN);
	session->Send(invenBuffer);

	return true;
}

bool Handle_C_PICKUP_ITEM(PacketSessionRef& session, C_PICKUP_ITEM* pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	PlayerRef player = gameSession->GetPlayer();
	if (player == nullptr) return false;

	int32 lootedItemId = pkt->itemId;

	int32 emptySlotIndex = -1;
	for (int i = 0; i < 16; i++)
	{
		if (player->inventory[i] == 0)
		{
			emptySlotIndex = i;
			break; 
		}
	}

	if (emptySlotIndex == -1)
	{
		cout << "[서버 로그] " << player->playerId << "번 유저 가방 꽉 참! 루팅 실패!" << endl;
		return true; 
	}

	player->inventory[emptySlotIndex] = lootedItemId;
	cout << "[서버 로그] " << player->playerId << "번 유저가 " << lootedItemId << "번 아이템을 " << emptySlotIndex << "번 슬롯에 획득!" << endl;

	S_UPDATE_INVEN invenPkt;
	invenPkt.slotIndex = emptySlotIndex;
	invenPkt.itemId = lootedItemId;

	auto invenBuffer = ClientPacketHandler::MakeSendBuffer(invenPkt, PKT_S_UPDATE_INVEN);
	session->Send(invenBuffer);

	return true;
}
