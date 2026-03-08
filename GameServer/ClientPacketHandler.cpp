#include "pch.h"
#include "ClientPacketHandler.h"
#include "Player.h"
#include "Monster.h"
#include "GameSession.h"
#include "RoomManager.h"
#include "DBConnectionPool.h"
#include "DBManager.h"
#include "Room.h"

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
	cout << "[클라 요청] 로그인 시도 ID: " << pkt->accountName << " / PW: " << pkt->password << endl;

	AccountDBData accountData;

	bool isLoginSuccess = GDBManager->LoginAccount(pkt->accountName, pkt->password, accountData);

	S_LOGIN sPkt;
	sPkt.success = isLoginSuccess ? 1 : 0;

	if (isLoginSuccess)
		sPkt.playerId = accountData.accountId;
	else
		sPkt.playerId = 0;

	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(sPkt, PKT_S_LOGIN);
	session->Send(sendBuffer);

	if (isLoginSuccess)
	{
		float randomOffsetX = ((float)rand() / RAND_MAX) * 4.0f - 2.0f;
		float randomOffsetZ = ((float)rand() / RAND_MAX) * 4.0f - 2.0f;

		PlayerRef player = MakeShared<Player>();
		player->playerId = accountData.accountId;
		// =======================================================
		// [테스트용] MainScene 스폰
		 player->curRoomID = ROOM::ROOM_1; 
		 player->posX = 59.81f + randomOffsetX;
		 player->posY = -9.0f;
		 player->posZ = -25.58f + randomOffsetZ;

		// [테스트용] BossScene1 다이렉트 스폰
		//player->curRoomID = ROOM::ROOM_2; // BossScene1 방 번호
		//player->posX = 1.24f + randomOffsetX;
		//player->posY = 0.0f;
		//player->posZ = 18.83f + randomOffsetZ;
		// =======================================================

		player->rotY = 0.0f;

		for (int i = 0; i < 16; i++)
		{
			player->inventory[i] = 0;
		}

		std::vector<ItemDBData> myItems;
		if (GDBManager->LoadInventory(player->playerId, myItems))
		{
			for (const auto& item : myItems)
			{
				if (item.slotIndex >= 0 && item.slotIndex < 16)
				{
					player->inventory[item.slotIndex] = item.itemId;
				}
			}
			cout << "[서버 로그] " << player->playerId << "번 유저 인벤토리 로드 완료 (총 " << myItems.size() << "개)" << endl;
		}

		for (int i = 0; i < 16; i++)
		{
			S_UPDATE_INVEN invenPkt;
			invenPkt.slotIndex = i;
			invenPkt.itemId = player->inventory[i];

			auto invenBuffer = ClientPacketHandler::MakeSendBuffer(invenPkt, PKT_S_UPDATE_INVEN);
			session->Send(invenBuffer);
		}

		gameSession->SetPlayer(player);
		player->ownerSession = gameSession;

		RoomRef room = RoomManager::Instance().GetRoom(player->curRoomID);
		if (room != nullptr)
		{
			room->Enter(player);
			cout << "[서버 로그] 로그인 성공! 방 입장 완료. (PlayerID: " << player->playerId << ")" << endl;
		}
	}
	else
	{
		cout << "로그인 실패! (DB에 없거나 비밀번호가 틀림)" << endl;
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
	cout << "[서버 수신 로그] 플레이어가 " << pkt->monsterId << "번 몬스터를 공격" << endl;
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	PlayerRef player = gameSession->GetPlayer();
	if (player == nullptr) return false;
	RoomRef room = RoomManager::Instance().GetRoom(player->curRoomID);
	if (room == nullptr) return false;

	MonsterRef monster = room->GetMonster(pkt->monsterId);
	if (monster == nullptr || monster->state == STATE_DEAD) return false;

	monster->hp -= pkt->damage;
	if (monster->hp < 0.f) monster->hp = 0.f;

	monster->OnDamaged(pkt->damage);

	S_HIT_MONSTER sPkt;
	sPkt.monsterId = monster->monsterId;
	sPkt.damage = pkt->damage;
	sPkt.currentHp = monster->hp;
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(sPkt, PKT_S_HIT_MONSTER);
	room->Broadcast(sendBuffer);

	if (monster->hp <= 0.f)
	{
		monster->ChangeState(STATE_DEAD);

		DroppedItem item;
		item.itemId = ITEM::ITEM_POTION;
		item.posX = monster->posX;
		item.posY = monster->posY;
		item.posZ = monster->posZ;
		room->SetDropItem(monster->monsterId, item);

		if (monster->type == MonsterType::MONSTER_TYPE_GOLEM) {
			room->isPortalOpened = true;

			S_OPEN_PORTAL portalPkt;
			portalPkt.isOpened = 1;
			auto portalBuffer = ClientPacketHandler::MakeSendBuffer(portalPkt, PKT_S_OPEN_PORTAL);

			room->Broadcast(portalBuffer);

			cout << "[서버 로그] 보스 처치 확인! 방의 포탈을 개방합니다." << endl;
		}
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
		<< " 피격! 남은 HP: " << player->hp << endl;

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

	if (GDBManager->DeleteInventoryItem(player->playerId, slot))
	{
		if (itemId == 1) // 포션
		{
			player->hp += 50.f;
			if (player->hp > player->maxHp) player->hp = player->maxHp;
			player->inventory[slot] = 0;
			cout << "[서버 로그] 플레이어 " << player->playerId << "가 포션을 마셨습니다! HP: " << player->hp << endl;

			S_USE_ITEM usePkt;
			usePkt.playerId = player->playerId;
			usePkt.itemId = itemId;
			usePkt.currentHp = player->hp;

			auto useBuffer = ClientPacketHandler::MakeSendBuffer(usePkt, PKT_S_USE_ITEM);
			room->Broadcast(useBuffer);
		}

		S_UPDATE_INVEN invenPkt;
		invenPkt.slotIndex = slot;
		invenPkt.itemId = player->inventory[slot];

		auto invenBuffer = ClientPacketHandler::MakeSendBuffer(invenPkt, PKT_S_UPDATE_INVEN);
		session->Send(invenBuffer);
	}
	else
	{
		cout << "[서버 로그] DB 삭제 실패! 아이템 사용이 취소되었습니다." << endl;
	}

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

	if (GDBManager->InsertInventoryItem(player->playerId, lootedItemId, emptySlotIndex))
	{
		player->inventory[emptySlotIndex] = lootedItemId;
		cout << "[서버 로그] " << player->playerId << "번 유저가 " << lootedItemId << "번 아이템을 " << emptySlotIndex << "번 슬롯에 획득" << endl;

		S_UPDATE_INVEN invenPkt;
		invenPkt.slotIndex = emptySlotIndex;
		invenPkt.itemId = lootedItemId;

		auto invenBuffer = ClientPacketHandler::MakeSendBuffer(invenPkt, PKT_S_UPDATE_INVEN);
		session->Send(invenBuffer);

		RoomRef room = RoomManager::Instance().GetRoom(player->curRoomID);
		if (room != nullptr)
		{
			room->RemoveDropItem(pkt->droppedMonsterId);

			S_PICKUP_ITEM pickupPkt;
			pickupPkt.droppedMonsterId = pkt->droppedMonsterId;
			auto pickupBuffer = ClientPacketHandler::MakeSendBuffer(pickupPkt, PKT_S_PICKUP_ITEM);
			room->Broadcast(pickupBuffer);
		}
	}
	else
	{
		cout << "[서버 로그] DB 저장 실패! 아이템 획득이 취소되었습니다." << endl;
	}

	return true;
}

bool Handle_C_ENTER_PORTAL(PacketSessionRef& session, C_ENTER_PORTAL* pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	PlayerRef player = gameSession->GetPlayer();
	if (player == nullptr) return false;

	int32 currentRoomId = player->curRoomID;
	int32 nextRoomId = ROOM::ROOM_1;

	if (currentRoomId == ROOM::ROOM_1) nextRoomId = ROOM::ROOM_2;
	else if (currentRoomId == ROOM::ROOM_2) nextRoomId = ROOM::ROOM_3;
	else return false;

	RoomRef oldRoom = RoomManager::Instance().GetRoom(currentRoomId);
	RoomRef newRoom = RoomManager::Instance().GetRoom(nextRoomId);

	if (oldRoom == nullptr || newRoom == nullptr)
	{
		cout << "[서버 오류] 이동할 룸이 존재하지 않습니다!" << endl;
		return false;
	}

	oldRoom->Leave(player);

	player->curRoomID = nextRoomId;
	float randomOffsetX = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
	float randomOffsetZ = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;

	if (nextRoomId == ROOM::ROOM_2)
	{
		player->posX = 1.24f + randomOffsetX;
		player->posY = 0.0f;
		player->posZ = 18.83f + randomOffsetZ;
	}
	else if (nextRoomId == ROOM::ROOM_3)
	{
		player->posX = 7.39f + randomOffsetX;
		player->posY = 0.0f;
		player->posZ = 13.6f + randomOffsetZ;
	}

	S_ENTER_PORTAL scenePkt;
	scenePkt.destRoomId = nextRoomId;
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(scenePkt, PKT_S_ENTER_PORTAL);
	session->Send(sendBuffer);

	newRoom->Enter(player);

	cout << "[서버 로그] " << player->playerId << "번 유저가 " << nextRoomId << "번 룸으로 이동 완료!" << endl;

	return true;
}
