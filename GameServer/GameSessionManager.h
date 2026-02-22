#pragma once

class GameSession;

class GameSessionManager
{
public:
	static GameSessionManager& Instance()
	{
		static GameSessionManager instance;
		return instance;
	}

public:
	void Add(GameSessionRef session);
	void Remove(GameSessionRef session);
	void Broadcast(SendBufferRef sendBuffer);

private:
	USE_LOCK;
	Set<GameSessionRef> _sessions;
};