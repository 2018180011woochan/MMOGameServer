#pragma once

struct DroppedItem
{
	int32 itemId;
	float posX;
	float posY;
	float posZ;
};

class Room : public std::enable_shared_from_this<Room>
{
public:
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);

	void EnterMonster(MonsterRef monster);
	void LeaveMonster(int32 monsterId);

	void Update(float deltaTime);

	MonsterRef GetMonster(int32 monsterId);
	PlayerRef GetPlayer(uint64 playerId);

	void SetDropItem(int32 idx, DroppedItem item) { _droppedItems[idx] = item; }
	void RemoveDropItem(int32 idx) { _droppedItems.erase(idx); }

	PlayerRef FindNearestPlayer(float x, float y, float z, float range);	// 특정 좌표에서 가장 가까운 플레이어를 찾는 함수

	void SpawnMonster(MonsterType type, float x, float y, float z);
private:
	USE_LOCK;
	map<uint64, PlayerRef> _players;
	map<int32, MonsterRef> _monsters;
	map<int32, DroppedItem> _droppedItems;

	std::atomic<int32> _monsterIdGenerator{ 100 };

public:
	bool isPortalOpened = false;
};
