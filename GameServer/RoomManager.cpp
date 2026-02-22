#include "pch.h"
#include "RoomManager.h"

void RoomManager::AddRoom(uint16 roomId, RoomRef room)
{
	WRITE_LOCK;
	_rooms[roomId] = room;
}

RoomRef RoomManager::GetRoom(uint16 roomId)
{
	READ_LOCK;
	auto it = _rooms.find(roomId);
	if (it != _rooms.end())
		return it->second;
	return nullptr;
}
