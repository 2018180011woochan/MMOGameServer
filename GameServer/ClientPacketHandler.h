#pragma once
#include "../Common/Packet/PacketProtocol.h"
#include <functional>
using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_C_LOGIN(PacketSessionRef& session, C_LOGIN* pkt);
bool Handle_C_MOVE(PacketSessionRef& session, C_MOVE* pkt);
bool Handle_C_STANCE(PacketSessionRef& session, C_STANCE* pkt);
bool Handle_C_JUMP(PacketSessionRef& session, C_JUMP* pkt);
bool Handle_C_ATTACK(PacketSessionRef& session, C_ATTACK* pkt);
bool Handle_C_DASH(PacketSessionRef& session, C_DASH* pkt);
bool Handle_C_HIT_MONSTER(PacketSessionRef& session, C_HIT_MONSTER* pkt);

class ClientPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_C_LOGIN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<C_LOGIN>(Handle_C_LOGIN, session, buffer, len); };
		GPacketHandler[PKT_C_MOVE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<C_MOVE>(Handle_C_MOVE, session, buffer, len); };
		GPacketHandler[PKT_C_STANCE] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<C_STANCE>(Handle_C_STANCE, session, buffer, len); };
		GPacketHandler[PKT_C_JUMP] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<C_JUMP>(Handle_C_JUMP, session, buffer, len); };
		GPacketHandler[PKT_C_ATTACK] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<C_ATTACK>(Handle_C_ATTACK, session, buffer, len); };
		GPacketHandler[PKT_C_DASH] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<C_DASH>(Handle_C_DASH, session, buffer, len); };
		GPacketHandler[PKT_C_HIT_MONSTER] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<C_HIT_MONSTER>(Handle_C_HIT_MONSTER, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len) 
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}

public:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType* pkt = reinterpret_cast<PacketType*>(buffer + sizeof(PacketHeader));
		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(sizeof(T));
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);

		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;

		::memcpy(&header[1], &pkt, dataSize);

		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};