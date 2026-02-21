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

#pragma pack()





