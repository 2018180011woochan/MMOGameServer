#pragma once

#include "../../ServerCore/Types.h"
#include "PacketEnum.h"
#include "PacketStruct.h"


#pragma pack(1)
struct PacketHeader
{
	uint16 size;
	uint16 id; 
};

struct C_LOGIN
{
	char accountName[NICKNAME_SIZE];
	char password[NICKNAME_SIZE];
}; 

struct S_LOGIN
{
	int32 success;
	int32 playerId; // 서버가 발급해주는 고유 ID
};

struct C_MOVE
{
	float posX;
	float posY;
	float posZ;
	float rotY;
	int32 isRunning;
};

struct S_MOVE
{
	int32 playerId; // 누가 이동했는지 식별
	float posX;
	float posY;
	float posZ;
	float rotY;
	int32 isRunning;
};

struct S_ENTER_GAME
{
	int32 playerId;
	float posX;
	float posY;
	float posZ;
	float rotY;
};

struct S_LEAVE_GAME
{
	int32 playerId;
};

struct C_STANCE
{
	int32 isStance; // 1 발도, 0 납도
};

struct S_STANCE
{
	int32 playerId;
	int32 isStance;
};

struct C_JUMP
{
	int32 dummy; // C#과 C++ 구조체 크기를 맞추기 위한 더미 데이터
};

struct S_JUMP
{
	int32 playerId;
};

struct C_ATTACK
{
	ATTACK_TYPE attackType; // 1, 2, 3 중 하나
};

struct S_ATTACK
{
	int32 playerId;
	ATTACK_TYPE attackType;
};

struct S_MONSTER_STATE
{
	int32 monsterId;
	MONSTER_STATE state;

	int32 targetId = -1;

	float destX = 0.f;
	float destY = 0.f;
	float destZ = 0.f;
};

struct C_DASH
{
	int32 dummy;
};

struct S_DASH
{
	int32 playerId;
};

struct C_HIT_MONSTER
{
	int32 monsterId;
	float damage;
};

struct S_HIT_MONSTER
{
	int32 monsterId;
	float damage;
	float currentHp;
};

struct C_HIT_PLAYER
{
	int32 monsterId;
	float damage;
	int32 isBlocked; 
};

struct S_HIT_PLAYER
{
	int32 playerId;
	int32 monsterId;
	float damage;
	float currentHp; 
	int32 isBlocked; 
};

struct S_SPAWN_MONSTER
{
	int32 monsterId;
	int32 monsterType;
	float posX;
	float posY;
	float posZ;
};

struct C_USE_ITEM
{
	int32 slotIndex;
};

struct S_UPDATE_INVEN
{
	int32 slotIndex;
	int32 itemId;
};

struct S_EQUIP_ITEM
{
	int32 playerId;
	int32 equipSlot;
	int32 itemId;
};

struct C_PICKUP_ITEM
{
	int32 itemId;
	int32 droppedMonsterId;
};

struct S_PICKUP_ITEM
{
	int32 droppedMonsterId;
};

struct S_OPEN_PORTAL
{
	int32 isOpened; 
};

struct S_SPAWN_ITEM
{
	int32 itemId;
	int32 droppedMonsterId;
	float posX;
	float posY;
	float posZ;
};

struct C_ENTER_PORTAL
{
	int32 dummy;
};

struct S_ENTER_PORTAL
{
	int32 destRoomId;
};
#pragma pack()





