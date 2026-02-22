#pragma once

class Player
{
public:
	uint64 playerId = 0;
	string name;
	float posX, posY, posZ, rotY;
	uint16 curRoomID;
	weak_ptr<GameSession> ownerSession;
};

