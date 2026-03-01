#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "BufferWriter.h"
#include "ClientPacketHandler.h"
#include "RoomManager.h"
#include "Monster.h"
#include "Skeleton.h"
#include "Golem.h"
#include"DBConnectionPool.h"
#include <tchar.h>

int main()
{
	ASSERT_CRASH(GDBConnectionPool->Connect(4, L"DRIVER={MySQL ODBC 9.6 UNICODE Driver};SERVER=127.0.0.1;PORT=3307;DATABASE=GameDB;UID=root;PWD=1234;"));

	ClientPacketHandler::Init();

	RoomManager::Instance().AddRoom(ROOM::ROOM_1, MakeShared<Room>());
	RoomManager::Instance().AddRoom(ROOM::ROOM_2, MakeShared<Room>());
	RoomManager::Instance().AddRoom(ROOM::ROOM_3, MakeShared<Room>());

	RoomRef room1 = RoomManager::Instance().GetRoom(ROOM::ROOM_1);

	room1->SpawnMonster(MONSTER_TYPE_SKELETON, 60.f, 0.f, -5.f);
	room1->SpawnMonster(MONSTER_TYPE_GOLEM, 58.f, 0.f, 25.f);

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, // TODO : SessionManager 등
		100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}				
			});
	}	
	while (true)
	{
		float deltaTime = 0.1f;

		RoomManager::Instance().Update(deltaTime);

		this_thread::sleep_for(100ms);
	}

	GThreadManager->Join();
}