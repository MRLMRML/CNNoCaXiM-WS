#pragma once
#include "DRAM.h"
#include "Channel.h"
#include "Controller.h"
#include "Clock.h"
#include <memory>

class DRAMNode
{
public:
	DRAMNode() = default;

	DRAMNode(const int NID, const int PEID)
		: m_controller{ std::make_unique<Controller>(NID, PEID)},
		m_DRAM{ std::make_unique<DRAM>() },
		m_slaveChannel{ std::make_unique<Channel>() },
		m_localClock{ std::make_shared<Clock>() }
	{
		m_slaveChannel->setUpConnection(m_controller->m_masterInterface, m_DRAM->m_slaveInterface);
		m_controller->m_localClock = m_localClock;
		m_DRAM->m_localClock = m_localClock;
		m_slaveChannel->m_localClock = m_localClock;
	}

	~DRAMNode()
	{
		m_slaveChannel->terminateConnection();
	}

	void runOneStep();
	void updateEnable();

	std::unique_ptr<Controller> m_controller{ nullptr };
	std::unique_ptr<DRAM> m_DRAM{ nullptr };
	std::unique_ptr<Channel> m_slaveChannel{ nullptr };
	std::shared_ptr<Clock> m_localClock{ nullptr };

private:
};