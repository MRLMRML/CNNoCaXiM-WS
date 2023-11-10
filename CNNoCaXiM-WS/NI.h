#pragma once
#include "DataStructures.h"
#include "MasterInterface.h"
#include "SlaveInterface.h"
#include "Port.h"
#include "Log.h"
#include "Clock.h"
#include "Timer.h"
#include <algorithm>
#include <cmath>

class NI
{
public:
	NI() = default;
	NI(const int NID) : m_NID{ NID } {}

	void runOneStep();

	void viewPacket(const Packet& packet);
	void viewFlit(const Flit& flit);

	bool receiveFlit();
	void assemblePacket(); // receive flits into m_flitReorderBuffer and form packet after Tail flit received
	void receivePacket(const Packet& packet);

	void sendReadResponse(const Packet& packet);
	
	void receiveWriteOutputRequest();

	void sendPacket(const Packet& packet);
	void dismantlePacket(const Packet& packet); // break down packet into flits
	void sendFlit();

public:
	NIState m_niState{};
	Port m_port{};
	SlaveInterface m_slaveInterface{ };
	int m_NID{}; // node ID of this PE
	int m_DRAMID{}; // node ID of DRAM
	int m_xID{ -1 };
	std::shared_ptr<Clock> m_localClock{ nullptr };
	std::unique_ptr<Timer> m_timer{ std::make_unique<Timer>() };

private:
	std::deque<Flit> m_sourceQueue{}; // store the flits to be sent
	std::vector<Flit> m_flitReorderBuffer{}; // store the received flits; if received TailFlit/HeadTailFlit, lookup the buffer and form the packet 
};