#pragma once
#include "../Common/Packet/PacketProtocol.h"
#include <functional>
using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);

class ServerPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		//GPacketHandler[PKT_S_LOGIN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_LOGIN>(Handle_S_LOGIN, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	//static SendBufferRef MakeSendBuffer(Protocol::C_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_C_LOGIN); }
	static SendBufferRef MakeSendBuffer(C_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_C_LOGIN); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	//template<typename T>
	//static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	//{
	//	const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
	//	const uint16 packetSize = dataSize + sizeof(PacketHeader);

	//	SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
	//	PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
	//	header->size = packetSize;
	//	header->id = pktId;
	//	ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
	//	sendBuffer->Close(packetSize);

	//	return sendBuffer;
	//}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(sizeof(T));
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);

		// 1. 헤더 채우기
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;

		// 2. 헤더 바로 뒤 메모리 공간에 구조체 데이터 복사
		::memcpy(&header[1], &pkt, dataSize);

		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};