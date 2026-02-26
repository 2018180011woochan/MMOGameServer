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

	int32 inventory[16] = { 0, }; // 16칸짜리 배열. 각 칸에는 ItemID가 들어감 (0이면 빈칸)
	int32 equipWeapon = 0; // 장착 중인 무기 번호
	int32 equipArmor = 0;  // 장착 중인 갑옷 번호
	int32 equipShield = 0; // 장착 중인 방패 번호
};

