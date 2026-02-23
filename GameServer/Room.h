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

private:
	USE_LOCK;
	map<uint64, PlayerRef> _players;
	map<int32, MonsterRef> _monsters;
};
