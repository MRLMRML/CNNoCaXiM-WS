#pragma once
#include "DataStructures.h"
#include "MasterInterface.h"
#include "Port.h"
#include "Log.h"
#include "Clock.h"
#include "Timer.h"
#include <algorithm>
#include <cmath>

class Controller
{
public:
	Controller() = default;
	Controller(const int NID, const int PEID) : m_NID{ NID }, m_PEID{ PEID } {}

	void runOneStep();

	void viewPacket(const Packet& packet);
	void viewFlit(const Flit& flit);

	void sendReadWeightRequest();
	void receiveReadWeightResponse();
	void receiveWriteWeightRequest();

	void sendReadInputRequest();
	void reformInputData(const std::vector<DATA_PRECISION>& inputData);
	void receiveReadInputResponse();

	void sendPacket(const Packet& packet);
	void dismantlePacket(const Packet& packet); // break down packet into flits
	void sendFlit();

	bool receiveFlit();
	void assemblePacket(); // receive flits into m_flitReorderBuffer and form packet after Tail flit received
	void receivePacket(const Packet& packet);

	void sendWriteWeightResponse(const Packet& packet);
	void sendWriteOutputRequest(const Packet& packet);
	void receiveWriteOutputResponse();

public:
	ControllerState m_controllerState{};
	int m_weightDataIndex{};
	int m_inputDataIndex{};
	int m_reformedInputDataIndex{};
	Port m_port{};
	MasterInterface m_masterInterface{ };
	int m_NID{}; // node ID of DRAM
	int m_PEID{}; // node ID of PE
	std::vector<std::vector<DATA_PRECISION>> m_reformedInputData{};
	std::vector<DATA_PRECISION> m_outputData{};
	std::shared_ptr<Clock> m_localClock{ nullptr };
	std::unique_ptr<Timer> m_timer{ std::make_unique<Timer>() };

private:
	std::deque<Flit> m_sourceQueue{}; // store the flits to be sent
	std::vector<Flit> m_flitReorderBuffer{}; // store the received flits; if received TailFlit/HeadTailFlit, lookup the buffer and form the packet 
};