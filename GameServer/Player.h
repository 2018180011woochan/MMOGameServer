#pragma once

class Player
{
public:
	Player();
public:
	uint64 playerId = 0;
	string name;
	float posX, posY, posZ, rotY;
	float hp, maxHp;
	uint16 curRoomID;
	weak_ptr<GameSession> ownerSession;
};

