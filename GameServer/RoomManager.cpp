#include "pch.h"
#include "RoomManager.h"

void RoomManager::AddRoom(uint16 roomId, RoomRef room)
{
	std::unique_lock<std::shared_mutex> lock(_managerMutex);
	_rooms[roomId] = room;
}

RoomRef RoomManager::GetRoom(uint16 roomId)
{
	std::shared_lock<std::shared_mutex> lock(_managerMutex); 
	auto it = _rooms.find(roomId);
	if (it != _rooms.end())
		return it->second;	// shared_ptr 반환 (참조 카운트 + 1)
	return nullptr;
}	

void RoomManager::Update(float deltaTime)
{
	std::shared_lock<std::shared_mutex> lock(_managerMutex);
	for (auto& pair : _rooms)
	{
		RoomRef room = pair.second;
		if (room != nullptr)
		{
			room->Update(deltaTime);
		}
	}
}
