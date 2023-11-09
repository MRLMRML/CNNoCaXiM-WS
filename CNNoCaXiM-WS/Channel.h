#pragma once
#include "MasterInterface.h"
#include "SlaveInterface.h"
#include "Clock.h"
#include <memory>

class Channel
{
public:
	Channel() = default;

	void setUpConnection(MasterInterface& masterInterface,
		SlaveInterface& slaveInterface);

	void terminateConnection();

	void resetHandshakes();

	void runOneStep();

private:
	void transferReadAddress();
	void transferReadData();
	void transferWriteAddress();
	void transferWriteData();
	void transferWriteResponse();

public:
	std::shared_ptr<Clock> m_localClock{ nullptr };

private:
	MasterInterface* m_masterInterfacePointer{ nullptr };
	SlaveInterface* m_slaveInterfacePointer{ nullptr };
};