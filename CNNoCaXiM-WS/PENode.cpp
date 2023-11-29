#include "PENode.h"

void PENode::runOneStep()
{
	m_NI->runOneStep();
	m_masterChannel->runOneStep();
	m_slaveChannel->runOneStep();
	m_PE->runOneStep();
	m_localClock->synchronizeExecutionClock();
	m_localClock->synchronizeTriggerClock();
}

void PENode::updateEnable()
{
	m_NI->m_port.updateEnable();
}