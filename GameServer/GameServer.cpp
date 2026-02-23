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
#include <tchar.h>

int main()
{
	ClientPacketHandler::Init();

	RoomManager::Instance().AddRoom(ROOM::ROOM_1, MakeShared<Room>());
	RoomManager::Instance().AddRoom(ROOM::ROOM_2, MakeShared<Room>());
	RoomManager::Instance().AddRoom(ROOM::ROOM_3, MakeShared<Room>());

	MonsterRef skeleton = make_shared<Skeleton>();
	skeleton->monsterId = 100; // 고유 ID 부여
	skeleton->posX = 60.f;    
	skeleton->posZ = -5.f;
	RoomManager::Instance().GetRoom(ROOM::ROOM_1)->EnterMonster(skeleton);

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