#include "pch.h"
#include "RoomManager.h"

void RoomManager::AddRoom(uint16 roomId, RoomRef room)
{
	WRITE_LOCK;
	_rooms[roomId] = room;
}

RoomRef RoomManager::GetRoom(uint16 roomId)
{
	READ_LOCK;	// Lock 획득
	auto it = _rooms.find(roomId);
	if (it != _rooms.end())
		return it->second;	// shared_ptr 반환 (참조 카운트 + 1)
	return nullptr;
}	// 함수 종료시 Lock 해제

void RoomManager::Update(float deltaTime)
{
	for (auto& pair : _rooms)
	{
		RoomRef room = pair.second;
		if (room != nullptr)
		{
			room->Update(deltaTime);
		}
	}
}
