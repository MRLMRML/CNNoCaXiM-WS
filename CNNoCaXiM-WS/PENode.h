#pragma once
#include "PE.h"
#include "Channel.h"
#include "NI.h"
#include "Clock.h"
#include <memory>

class PENode
{
public:
	PENode() = default;

	PENode(const int NID)
		: m_NI{ std::make_unique<NI>(NID) },
		m_PE{ std::make_unique<PE>() },
		m_masterChannel{ std::make_unique<Channel>() },
		m_localClock{ std::make_shared<Clock>() }
	{
		m_masterChannel->setUpConnection(m_PE->m_masterInterface, m_NI->m_slaveInterface);
		m_NI->m_localClock = m_localClock;
		m_PE->m_localClock = m_localClock;
		m_masterChannel->m_localClock = m_localClock;
	}

	PENode(const int NID,
		DATA_PRECISION(*activationFunction)(DATA_PRECISION&))
		: m_NI{ std::make_unique<NI>(NID) },
		m_PE{ std::make_unique<PE>(activationFunction) },
		m_masterChannel{ std::make_unique<Channel>() },
		m_localClock{ std::make_shared<Clock>() }
	{
		m_masterChannel->setUpConnection(m_PE->m_masterInterface, m_NI->m_slaveInterface);
		m_NI->m_localClock = m_localClock;
		m_PE->m_localClock = m_localClock;
		m_masterChannel->m_localClock = m_localClock;
	}

	~PENode()
	{
		m_masterChannel->terminateConnection();
	}

	void runOneStep();
	void updateEnable();

	std::unique_ptr<NI> m_NI{ nullptr };
	std::unique_ptr<PE> m_PE{ nullptr };
	std::unique_ptr<Channel> m_masterChannel{ nullptr };
	std::shared_ptr<Clock> m_localClock{ nullptr };

private:
};