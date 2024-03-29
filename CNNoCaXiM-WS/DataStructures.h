#pragma once
#include "Parameters.h"
#include "Log.h"
#include <vector>
#include <array>
#include <deque>
#include <string>
#include <iostream>

// << overloading for std::vector
template <typename T> std::ostream& operator<<(std::ostream& stream, const std::vector<T>& vector)
{
	for (auto element : vector)
	{
		stream << element << " ";
	}
	return stream;
}

// << overloading for std::deque
template <typename T> std::ostream& operator<<(std::ostream& stream, const std::deque<T>& deque)
{
	for (auto element : deque)
	{
		stream << element << " ";
	}
	return stream;
}

struct MemoryLine
{
	int address{};
	std::vector<DATA_PRECISION> data{};
};

enum class PortType
{
	Unselected,
	NorthPort,
	SouthPort,
	WestPort,
	EastPort,
	TerminalPort
};

std::ostream& operator<<(std::ostream& stream, const PortType& portType);

struct VirtualChannelArbitrationRecorderLine
{
	bool operator==(const VirtualChannelArbitrationRecorderLine& record) const;
	PortType port{};
	int virtualChannel{};
	int virtualChannelPriority{};
};

struct SwitchArbitrationRecorderLine
{
	bool operator==(const SwitchArbitrationRecorderLine& record) const;
	PortType port{};
	PortType outputPortSwitched{};
	int switchPriorityGlobal{};
};

enum class VirtualChannelState
{
	I, // Idle, virtual channel is empty; also used in downstream
	R, // Routing
	V, // Virtual channel allocation; also used in downstream
	A // Active, virtual channel is occupied; also used in downstream
};

std::ostream& operator<<(std::ostream& stream, const VirtualChannelState& virtualChannelState);

enum class PacketType
{
	Default,
	ReadRequest,
	ReadResponse,
	WriteRequest,
	WriteResponse
};

std::ostream& operator<<(std::ostream& stream, const PacketType& packetType);

struct Packet
{
	int destination{};
	int xID{};
	PacketType RWQB{};
	int MID{};
	int SID{};
	int SEQID{};
	int AxADDR{};
	std::vector<DATA_PRECISION> xDATA{};
};

std::ostream& operator<<(std::ostream& stream, const Packet& packet);

struct PacketReorderBufferLine
{
	int SEQID{};
	std::vector<DATA_PRECISION> xData{};
};

enum class FlitType
{
	DefaultFlit,
	HeadFlit,
	BodyFlit,
	TailFlit,
	HeadTailFlit
};

std::ostream& operator<<(std::ostream& stream, const FlitType& flitType);

struct Flit
{
	Flit() = default;
	bool operator==(const Flit& record) const;

	// HeadFlit
	Flit(const PortType port,
		const int virtualChannel,
		const FlitType flitType,
		const int destination,
		const int xID,
		const PacketType RWQB,
		const int MID,
		const int SID,
		const int SEQID)
		:
		port{ port }, 
		virtualChannel{ virtualChannel },
		flitType{ flitType },
		destination{ destination },
		xID{ xID },
		RWQB{ RWQB },
		MID{ MID },
		SID{ SID },
		SEQID{ SEQID } {}

	// BodyFlit
	Flit(const PortType port,
		const int virtualChannel,
		const FlitType flitType,
		const int bodyFlitNo,
		const int xID,
		const int MID,
		const int SEQID)
		:
		port{ port },
		virtualChannel{ virtualChannel },
		flitType{ flitType },
		bodyFlitNo{ bodyFlitNo },
		xID{ xID },
		MID{ MID },
		SEQID{ SEQID } {}

	// TailFlit
	Flit(const PortType port,
		const int virtualChannel,
		const FlitType flitType,
		const int xID,
		const int MID,
		const int SEQID,
		const int AxADDR,
		const std::vector<DATA_PRECISION> xDATA)
		:
		port{ port },
		virtualChannel{ virtualChannel },
		flitType{ flitType },
		xID{ xID },
		MID{ MID },
		SEQID{ SEQID },
		AxADDR{ AxADDR },
		xDATA{ xDATA } {}

	// HeadTailFlit
	Flit(const PortType port,
		const int virtualChannel,
		const FlitType flitType,
		const Packet packet)
		:
		port{ port },
		virtualChannel{ virtualChannel },
		flitType{ flitType },
		destination{ packet.destination },
		xID{ packet.xID },
		RWQB{ packet.RWQB },
		MID{ packet.MID },
		SID{ packet.SID },
		SEQID{ packet.SEQID },
		AxADDR{ packet.AxADDR },
		xDATA{ packet.xDATA } {}

	PortType port{PortType::Unselected};
	int virtualChannel{-1};
	FlitType flitType{ FlitType::DefaultFlit };
	int bodyFlitNo{-1};
	int destination{-1};
	int xID{-1};
	PacketType RWQB{PacketType::Default};
	int MID{-1};
	int SID{-1};
	int SEQID{-1};
	int AxADDR{-1};
	std::vector<DATA_PRECISION> xDATA{};
};

std::ostream& operator<<(std::ostream& stream, const Flit& flit);

struct Credit
{
	bool isTailFlit{};
	int virtualChannel{};
};

enum class PEState
{
	W,
	I
};

std::ostream& operator<<(std::ostream& stream, const PEState& peState);

enum class ControllerState
{
	N,
	W,
	K,
	I,
	O,
	B
};

std::ostream& operator<<(std::ostream& stream, const ControllerState& controllerState);

enum class NIState
{
	W,
	K,
	I,
	O
};

std::ostream& operator<<(std::ostream& stream, const NIState& niState);

enum class DRAMState
{
	W,
	I,
	O
};

std::ostream& operator<<(std::ostream& stream, const DRAMState& dramState);

struct RouterID
{
	int y{}, x{};
};

std::ostream& operator<<(std::ostream& stream, const RouterID& routerID);

struct MappingTableLine
{
	RouterID routerID{};
	int NID{};
};

struct NetworkPerformance
{
	float averageLatency{};
	float throughput{};
};