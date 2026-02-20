#pragma once

#include "../../ServerCore/Types.h"
#include "PacketEnum.h"
#include "PacketStruct.h"


#pragma pack(1)
struct PacketHeader
{
	uint16 size;
	uint16 id; // 프로토콜ID (ex. 1=로그인, 2=이동요청)
};

struct C_LOGIN
{
	uint64 dummyId; // 테스트용 데이터 (예: 카카오톡 고유 ID 등)
};

#pragma pack()





