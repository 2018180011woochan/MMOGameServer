#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "ClientPacketHandler.h"
#include "RoomManager.h"
#include "Player.h"
void GameSession::OnConnected()
{
	GameSessionManager::Instance().Add(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnected()
{
	GameSessionManager::Instance().Remove(static_pointer_cast<GameSession>(shared_from_this()));
	cout << "플레이어 " <<  _currentPlayer->playerId << " 종료" << endl;
	if (_currentPlayer != nullptr)
	{
		RoomRef room = RoomManager::Instance().GetRoom(_currentPlayer->curRoomID);
		if (room != nullptr)
		{
			room->Leave(_currentPlayer);
		}
		_currentPlayer = nullptr; 
	}
}

void GameSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	// TODO : packetId 대역 체크
	ClientPacketHandler::HandlePacket(session, buffer, len);
}

void GameSession::OnSend(int32 len)
{
}