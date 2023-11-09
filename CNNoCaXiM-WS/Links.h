#pragma once
#include "Link.h"
#include "Log.h"
#include <set>

class Links
{
public:
	Links() = default;
	virtual ~Links() = default;

	void setUpConnection(Port& inputPort, Port& outputPort);

	void terminateConnection(Port& inputPort, Port& outputPort);

	void terminateAllConnections();

	void updateEnable();

	// Timing
	void synchronizeTriggerClocks();
	void synchronizeExecutionClocks();

	virtual void runOneStep();
	std::set<Link> m_connections{};
private:
};