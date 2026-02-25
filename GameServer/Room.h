#pragma once

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
	PlayerRef FindNearestPlayer(float x, float y, float z, float range);	// 특정 좌표에서 가장 가까운 플레이어를 찾는 함수

	void SpawnMonster(MonsterType type, float x, float y, float z);
private:
	USE_LOCK;
	map<uint64, PlayerRef> _players;
	map<int32, MonsterRef> _monsters;

	std::atomic<int32> _monsterIdGenerator{ 100 };
};
