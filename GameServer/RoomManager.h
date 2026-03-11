#pragma once
#include "Room.h"

class RoomManager
{
public:
	static RoomManager& Instance()
	{
		static RoomManager instance;
		return instance;
	}

public:
	void AddRoom(uint16 roomId, RoomRef room);	// 서버 시작 시 방을 미리 만듬
	RoomRef GetRoom(uint16 roomId);				// 플레이어가 특정 방을 찾을때

	void Update(float deltaTime);

private:
	std::shared_mutex _managerMutex;
	map<uint16, RoomRef> _rooms;
};

