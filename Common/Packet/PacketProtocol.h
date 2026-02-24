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
	uint64 dummyId; 
};

struct S_LOGIN
{
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

#pragma pack()





